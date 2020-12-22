// regex 헤더 안에 이미 __in, __out 정의 되어있다
// 해서 Rigi_Def.hpp 보다 먼저 선언을 해야 한다
#include <regex>
#include <thread>
#include <chrono>
#include "TCP_ClientMgr.hpp"

TCP_ClientMgr::TCP_ClientMgr( __in MsgLog_Q *_pLogQ ) : m_pLogQ(nullptr)
{ 
	m_pLogQ = _pLogQ;
	m_bRun_Thread = false;
}

TCP_ClientMgr::~TCP_ClientMgr() 
{ 
	m_pLogQ = nullptr;

	Clear_Eng();
}

void TCP_ClientMgr::Run()
{
	if(nullptr == m_pLogQ)
	{
		assert(0 && "[TCP_ClientMgr::Async_Run] m_pLogQ is nullptr");
		return ;
	}

    m_io_service.reset();
	boost::system::error_code ec;
	m_bRun_Thread = true;

	// -------------------------------------------------------
	// 서버 접속 스레드 
	int nSec_wait = 1;
	m_thr_conn = std::thread( [&]()
	{
		while(true == m_bRun_Thread)
		{
			Check_Connect_Session();

			std::this_thread::sleep_for( std::chrono::seconds(nSec_wait) );
		}
	});
	m_thr_conn.detach();
	// -------------------------------------------------------

	// -------------------------------------------------------
	// 데이터 전송 스레드
	m_thr_send = std::thread( [&]()
	{
		int nCount = 0;
		bool bRet_Send = false;
		while(true == m_bRun_Thread)
		{
			bRet_Send = false;
			std::string *pLog = m_pLogQ->Pop_Data( "172.17.0.2:4444" );
			if(nullptr != pLog) 
			{
				bRet_Send = SendPacket_Round_Robin(nCount ,pLog);
				delete pLog;
			}

			if(false == bRet_Send)
				usleep(1000);
		}
	});
	m_thr_send.detach();
	// -------------------------------------------------------

	m_io_service.run(ec);

	return ;
}

Rigitaeda::Rigi_ClientTCP * TCP_ClientMgr::Connect_Session( __in const char *_pszServerIP, __in int _nPort )
{
	Rigitaeda::Rigi_ClientTCP *pClient = new Rigitaeda::Rigi_ClientTCP(10240);
	pClient->Add_EventHandler_Close( std::bind(&TCP_ClientMgr::OnEvent_Close, this, pClient) );
	if(true == pClient->Connect( _pszServerIP, _nPort, m_io_service ) )
		return pClient;
	else
		delete pClient;
	return nullptr;
}

bool TCP_ClientMgr::Add_Eng( __in const char *_pszServerIP, __in int _nPort )
{	
	const std::lock_guard<std::mutex> lock1(m_mtxSessionPool_Connect);
	const std::lock_guard<std::mutex> lock2(m_mtxSessionPool_DisConnect);

	std::string strIP_Port = _pszServerIP;
	strIP_Port += ":";
	strIP_Port += std::to_string(_nPort);

	auto find1 = m_mapSessionPool_Connect.find(strIP_Port);
	if(find1 != m_mapSessionPool_Connect.end())
		return false;
	auto find2 = m_mapSessionPool_DisConnect.find(strIP_Port);
	if(find2 != m_mapSessionPool_DisConnect.end())
		return false;

	Rigitaeda::Rigi_ClientTCP *pClient = new Rigitaeda::Rigi_ClientTCP(10240);
	pClient->Add_EventHandler_Close( std::bind(&TCP_ClientMgr::OnEvent_Close, this, pClient) );

	// 일단 세션 풀을 만들기 위해서 연결을 시도하자 
	if( true == pClient->Connect( _pszServerIP, _nPort, m_io_service ) )
		m_mapSessionPool_Connect.insert(std::make_pair(strIP_Port, pClient));
	else
		m_mapSessionPool_DisConnect.insert(std::make_pair(strIP_Port, pClient));

	return true;
}

bool TCP_ClientMgr::Del_Eng( __in const char *_pszServerIP, __in int _nPort )
{
	const std::lock_guard<std::mutex> lock1(m_mtxSessionPool_Connect);
	const std::lock_guard<std::mutex> lock2(m_mtxSessionPool_DisConnect);

	std::string strIP_Port = _pszServerIP;
	strIP_Port += ":";
	strIP_Port += std::to_string(_nPort);

	auto find1 = m_mapSessionPool_Connect.find(strIP_Port);
	if(find1 != m_mapSessionPool_Connect.end())
	{
		m_mapSessionPool_Connect.erase(find1);
		return true;
	}

	auto find2 = m_mapSessionPool_DisConnect.find(strIP_Port);
	if(find2 != m_mapSessionPool_DisConnect.end())
	{
		m_mapSessionPool_DisConnect.erase(find2);
		return true;
	}

	return false;
}

bool TCP_ClientMgr::Chg_Eng( 	__in const char *_pszServerIP_Old, __in int _nPort_Old,
								__in const char *_pszServerIP_New, __in int _nPort_New )
{
	const std::lock_guard<std::mutex> lock1(m_mtxSessionPool_Connect);
	const std::lock_guard<std::mutex> lock2(m_mtxSessionPool_DisConnect);

	std::string strIP_Port_old = _pszServerIP_Old;
	strIP_Port_old += ":";
	strIP_Port_old += std::to_string(_nPort_Old);

	std::string strIP_Port_new = _pszServerIP_New;
	strIP_Port_new += ":";
	strIP_Port_new += std::to_string(_nPort_New);

	auto find1 = m_mapSessionPool_Connect.find(strIP_Port_old);
	if(find1 != m_mapSessionPool_Connect.end())
	{
		Rigitaeda::Rigi_ClientTCP *pClient = find1->second;

		m_mapSessionPool_Connect.erase(find1);

		m_mapSessionPool_Connect.insert(std::make_pair(strIP_Port_new, pClient));
		return true;
	}

	auto find2 = m_mapSessionPool_Connect.find(strIP_Port_old);
	if(find2 != m_mapSessionPool_Connect.end())
	{
		Rigitaeda::Rigi_ClientTCP *pClient = find2->second;

		m_mapSessionPool_Connect.erase(find2);

		m_mapSessionPool_Connect.insert(std::make_pair(strIP_Port_new, pClient));
		return true;
	}

	return false;
}

void TCP_ClientMgr::Set_LogQ( __in MsgLog_Q *_pLogQ )
{
	m_pLogQ = _pLogQ;
}

bool TCP_ClientMgr::SendPacket_Round_Robin( __inout int &_nIndex, __in std::string *_pData )
{
	const std::lock_guard<std::mutex> lock(m_mtxSessionPool_Connect);
	if(true == m_mapSessionPool_Connect.empty())
	{
		std::cout << "Send Packet | mapSession pool is empty " << *_pData << std::endl;
		_nIndex = 0;
		return false;		
	}

	int nCount = 0;
	for(auto &pSession : m_mapSessionPool_Connect)
	{
		std::cout << "Loop session pool >> " << *_pData << std::endl;

		if( m_nCount_Send_Round_Robin == nCount )
		{
			std::cout << "Send Packet >> " << *_pData << std::endl;

			pSession.second->ASync_Send( _pData->c_str(), _pData->length() );

			if( m_nCount_Send_Round_Robin < (int)m_mapSessionPool_Connect.size() )
				m_nCount_Send_Round_Robin++;
			else
				m_nCount_Send_Round_Robin = 0;

			break;
		}
	}

	return true;
}

void TCP_ClientMgr::OnEvent_Close( __in Rigitaeda::Rigi_ClientTCP *_pSession )
{
	std::cout << "[TCP_ClientMgr::OnEvent_Close] <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< OnClosed !! " << std::endl;

	std::string strIP_Port = _pSession->Get_ServerIP();
	strIP_Port += ":";
	strIP_Port = _pSession->Get_Port();

	Del_SessionPool_Connected(strIP_Port);
	Add_SessionPool_DisConnected( strIP_Port, _pSession );
}

void TCP_ClientMgr::Check_Connect_Session()
{
	const std::lock_guard<std::mutex> lock(m_mtxSessionPool_DisConnect);

	std::map<STR_IP_PORT, Rigitaeda::Rigi_ClientTCP *> mapConnected;

	for(auto &map_s : m_mapSessionPool_DisConnect)
	{
		if(false == map_s.second->IsConnected() )
		{
			std::cout << "[Reconnect] << IP = " << map_s.second->Get_SessionIP() << " | Port = " << map_s.second->Get_Port() << std::endl;

			if(true == map_s.second->Reconnect( m_io_service ) )
			{
				Add_SessionPool_Connected( map_s.first, map_s.second );
				mapConnected.insert( std::make_pair(map_s.first, map_s.second) );
			}
		}
	}
}

void TCP_ClientMgr::Stop()
{
	m_bRun_Thread = false;

	m_thr_conn.join();
	m_thr_send.join();

	Clear_Eng();
}

void TCP_ClientMgr::Clear_Eng()
{
	const std::lock_guard<std::mutex> lock1(m_mtxSessionPool_Connect);
	const std::lock_guard<std::mutex> lock2(m_mtxSessionPool_DisConnect);

	for(auto &Session : m_mapSessionPool_Connect)
	{
		Session.second->Close();
		delete Session.second;
	}
	m_mapSessionPool_Connect.clear();

	for(auto &Session : m_mapSessionPool_DisConnect)
	{
		Session.second->Close();
		delete Session.second;
	}
	m_mapSessionPool_DisConnect.clear();
}

void TCP_ClientMgr::Add_SessionPool_Connected(  __in std::string _strIP_Port, 
												__in Rigitaeda::Rigi_ClientTCP *_pSession )
{
	const std::lock_guard<std::mutex> lock1(m_mtxSessionPool_Connect);

	m_mapSessionPool_Connect.insert( std::make_pair(_strIP_Port, _pSession) );
	std::cout << "[Add_SessionPool_Connected] << " << _strIP_Port << std::endl;
}

void TCP_ClientMgr::Del_SessionPool_Connected(  __in std::string _strIP_Port )
{
	const std::lock_guard<std::mutex> lock1(m_mtxSessionPool_Connect);

	auto find = m_mapSessionPool_Connect.find( _strIP_Port );
	if(find != m_mapSessionPool_Connect.end())
	{
		m_mapSessionPool_Connect.erase(find);
		std::cout << "[Del_SessionPool_Connected] << " << _strIP_Port << std::endl;
	}
}

void TCP_ClientMgr::Add_SessionPool_DisConnected(  	__in std::string _strIP_Port, 
													__in Rigitaeda::Rigi_ClientTCP *_pSession )
{
	const std::lock_guard<std::mutex> lock1(m_mtxSessionPool_DisConnect);

	m_mapSessionPool_DisConnect.insert( std::make_pair(_strIP_Port, _pSession) );
	std::cout << "[Add_SessionPool_DisConnected] << " << _strIP_Port << std::endl;
}

void TCP_ClientMgr::Del_SessionPool_DisConnected(  	__in std::string _strIP_Port )
{
	const std::lock_guard<std::mutex> lock1(m_mtxSessionPool_DisConnect);

	auto find = m_mapSessionPool_DisConnect.find( _strIP_Port );
	if(find != m_mapSessionPool_DisConnect.end())
	{
		m_mapSessionPool_DisConnect.erase(find);
		std::cout << "[Del_SessionPool_DisConnected] << " << _strIP_Port << std::endl;
	}
}