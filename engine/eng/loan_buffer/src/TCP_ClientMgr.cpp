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
	int nSec_wait = 10;
	m_thr_conn = std::thread( [&]()
	{
		while(true == m_bRun_Thread)
		{
			Check_Connect_Session();

			sleep(nSec_wait);
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
				std::cout << "[Send Packet] << "<< pLog->c_str() << std:: endl;
				bRet_Send = SendPacket_Round_Robin(nCount ,pLog);
			}

			delete pLog;

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
	const std::lock_guard<std::mutex> lock(m_mtxSession_Pool);
	std::string strIP_Port = _pszServerIP;
	strIP_Port += ":";
	strIP_Port += std::to_string(_nPort);

	auto find = m_mapSession_Pool.find(strIP_Port);
	if(find != m_mapSession_Pool.end())
		return false;

	Rigitaeda::Rigi_ClientTCP *pClient = new Rigitaeda::Rigi_ClientTCP(10240);
	pClient->Add_EventHandler_Close( std::bind(&TCP_ClientMgr::OnEvent_Close, this, pClient) );

	// 일단 세션 풀을 만들기 위해서 연결을 시도하자 
	pClient->Connect( _pszServerIP, _nPort, m_io_service );

	m_mapSession_Pool.insert(std::make_pair(strIP_Port, pClient));

	return true;
}

bool TCP_ClientMgr::Del_Eng( __in const char *_pszServerIP, __in int _nPort )
{
	// const std::lock_guard<std::mutex> lock(m_mtxSessionPool);
	
	// std::string strIP_Port = _pszServerIP;
	// strIP_Port += ":";
	// strIP_Port += std::to_string(_nPort);

	// auto find = m_mapSessionPool.find(strIP_Port);
	// if(find == m_mapSessionPool.end())
	// 	return false;

	// m_mapSessionPool.erase(find);
	return true;
}

bool TCP_ClientMgr::Chg_Eng( 	__in const char *_pszServerIP_Old, __in int _nPort_Old,
								__in const char *_pszServerIP_New, __in int _nPort_New )
{
	// const std::lock_guard<std::mutex> lock(m_mtxSessionPool);
	
	// std::string strIP_Port_Old = _pszServerIP_Old;
	// strIP_Port_Old += ":";
	// strIP_Port_Old += std::to_string(_nPort_Old);

	// auto find = m_mapSessionPool.find(strIP_Port_Old);
	// if(find == m_mapSessionPool.end())
	// 	return false;

	// Rigitaeda::Rigi_ClientTCP *pClient = find->first;

	// m_mapSessionPool.erase(find);

	// std::string strIP_Port_New = _pszServerIP_New;
	// strIP_Port_New += ":";
	// strIP_Port_New += std::to_string(_nPort_New);

	// m_mapSessionPool.insert( std::make_pair(strIP_Port_New, pClient) );
	return true;
}

void TCP_ClientMgr::Set_LogQ( __in MsgLog_Q *_pLogQ )
{
	m_pLogQ = _pLogQ;
}

bool TCP_ClientMgr::SendPacket_Round_Robin( __inout int &_nIndex, __in std::string *_pData )
{
	const std::lock_guard<std::mutex> lock(m_mtxSession_Pool);
	if(true == m_mapSession_Pool.empty())
	{
		_nIndex = 0;
		return false;		
	}

	// 인덱스가 전체 크기를 넘어가면 첫번째 인덱스 소켓으로 전송한다 
	if( (_nIndex >= (int)m_mapSession_Pool.size()) | 
		(_nIndex < 0) )
	{
		auto pSession = m_mapSession_Pool.begin();
		pSession->second->ASync_Send( _pData->c_str(), _pData->length() );
		return true;
	}
	else
	{
		int nCount = 0;
		for(auto &pSession : m_mapSession_Pool)
		{
			if(_nIndex == nCount)
			{
				pSession.second->ASync_Send( _pData->c_str(), _pData->length() );

				_nIndex++;

				return true;
			}

			nCount++;
		}

		// 여기까지 왔다는건 인덱스를 찾지못했단느건데 다음번에 처음부터 시작하도록 인덱스를 0으로 설정하자 
		_nIndex = 0;
	}

	return false;
}

void TCP_ClientMgr::OnEvent_Close( __in Rigitaeda::Rigi_ClientTCP *_pSession )
{
	std::cout << "[TCP_ClientMgr::OnEvent_Close] <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< OnClosed !! " << std::endl;
}

void TCP_ClientMgr::Check_Connect_Session()
{
	const std::lock_guard<std::mutex> lock(m_mtxSession_Pool);
	for(auto &map_s : m_mapSession_Pool)
	{
		if(false == map_s.second->IsConnected() )
		{
			std::cout << "[Reconnect] << IP = " << map_s.second->Get_SessionIP() << " | Port = " << map_s.second->Get_Port() << std::endl;

			map_s.second->Reconnect( m_io_service );
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
	const std::lock_guard<std::mutex> lock(m_mtxSession_Pool);
	for(auto &Session : m_mapSession_Pool)
	{
		Session.second->Close();
		delete Session.second;
	}
	m_mapSession_Pool.clear();
}