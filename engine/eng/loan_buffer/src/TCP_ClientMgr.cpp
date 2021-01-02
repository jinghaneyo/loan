// regex 헤더 안에 이미 __in, __out 정의 되어있다
// 해서 Rigi_Def.hpp 보다 먼저 선언을 해야 한다
#include <regex>
#include <chrono>
#include "TCP_ClientMgr.hpp"

TCP_ClientMgr::TCP_ClientMgr( __in MsgLog_Q *_pLogQ ) : m_pLogQ(_pLogQ)
{ 
	m_bRun_Thread = false;

	// todo jingtest
	m_vecActive.emplace_back("172.17.0.2:4444");
	m_vecStandby.emplace_back("172.17.0.3:5555");
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
		//ASSERT(0 && "[TCP_ClientMgr::Async_Run] m_pLogQ is nullptr");
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
			if(false == m_bUse_RoundRobin)
				Check_Connect_Session();
			else
				Check_Connect_Session_Failover();

			std::this_thread::sleep_for( std::chrono::seconds(nSec_wait) );
		}
	});
	m_thr_conn.detach();
	// -------------------------------------------------------

	// -------------------------------------------------------
	// 데이터 전송 스레드
	m_thr_send = std::thread( [&]()
	{
		bool bRet_Send = false;
		while(true == m_bRun_Thread)
		{
			bRet_Send = false;
			std::string *pLog = m_pLogQ->Pop_front( "172.17.0.2:4444" );
			if(nullptr != pLog) 
			{
				if(true == m_bUse_RoundRobin)
					bRet_Send = SendPacket_Round_Robin(pLog);
				else
					bRet_Send = SendPacket_Fail_Over(pLog);

				// 실패인 경우는 모든 세션이 전송 실패인 경우이다. 따라서, 현재 데이터는 다시 전송할 수 있도록 큐의 앞에 넣어 주도록 하자
				if(false == bRet_Send)
					m_pLogQ->Push_front("172.17.0.2:4444", pLog);
				else
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

const Rigitaeda::Rigi_ClientTCP * TCP_ClientMgr::Connect_Session( __in const char *_pszServerIP, __in int _nPort )
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

	// 이미 등록이 되어 있는지 확인
	auto itr = m_DqSessionPool_Connect.begin();
	while(itr != m_DqSessionPool_Connect.end())
	{
		Rigitaeda::Rigi_ClientTCP *pSession = *itr;
		if( (pSession->Get_ServerIP() == _pszServerIP) && (pSession->Get_Port() == std::to_string(_nPort) ) )
			return false;
		itr++;
	}
	// 이미 등록이 되어 있는지 확인
	itr = m_DqSessionPool_DisConnect.begin();
	while(itr != m_DqSessionPool_DisConnect.end())
	{
		Rigitaeda::Rigi_ClientTCP *pSession = *itr;
		if( (pSession->Get_ServerIP() == _pszServerIP) && (pSession->Get_Port() == std::to_string(_nPort) ) )
			return false;
		itr++;
	}

	Rigitaeda::Rigi_ClientTCP *pClient = new Rigitaeda::Rigi_ClientTCP(10240);
	pClient->Add_EventHandler_Close( std::bind(&TCP_ClientMgr::OnEvent_Close, this, pClient) );

	// if(false == m_bUse_RoundRobin)
	// {
	// 	const std::lock_guard<std::mutex> lock3(m_mtxSessionPool_Active);
	// 	const std::lock_guard<std::mutex> lock4(m_mtxSessionPool_StandBy);

	// 	m_DqSessionPool_Active.emplace_back(pClient);
	// }
	// else
	{
		// 일단 세션 풀을 만들기 위해서 연결을 시도하자 
		if( true == pClient->Connect( _pszServerIP, _nPort, m_io_service ) )
			m_DqSessionPool_Connect.emplace_back(pClient);
		else
			m_DqSessionPool_DisConnect.emplace_back(pClient);
	}

	return true;
}

bool TCP_ClientMgr::Del_Eng( __in const char *_pszServerIP, __in int _nPort )
{
	// const std::lock_guard<std::mutex> lock1(m_mtxSessionPool_Connect);
	// const std::lock_guard<std::mutex> lock2(m_mtxSessionPool_DisConnect);

	// std::string strIP_Port = _pszServerIP;
	// strIP_Port += ":";
	// strIP_Port += std::to_string(_nPort);

	// auto find1 = m_mapSessionPool_Connect.find(strIP_Port);
	// if(find1 != m_mapSessionPool_Connect.end())
	// {
	// 	m_mapSessionPool_Connect.erase(find1);
	// 	return true;
	// }

	// auto find2 = m_mapSessionPool_DisConnect.find(strIP_Port);
	// if(find2 != m_mapSessionPool_DisConnect.end())
	// {
	// 	m_mapSessionPool_DisConnect.erase(find2);
	// 	return true;
	// }

	return false;
}

bool TCP_ClientMgr::Chg_Eng( 	__in const char *_pszServerIP_Old, __in int _nPort_Old,
								__in const char *_pszServerIP_New, __in int _nPort_New )
{
	// const std::lock_guard<std::mutex> lock1(m_mtxSessionPool_Connect);
	// const std::lock_guard<std::mutex> lock2(m_mtxSessionPool_DisConnect);

	// std::string strIP_Port_old = _pszServerIP_Old;
	// strIP_Port_old += ":";
	// strIP_Port_old += std::to_string(_nPort_Old);

	// std::string strIP_Port_new = _pszServerIP_New;
	// strIP_Port_new += ":";
	// strIP_Port_new += std::to_string(_nPort_New);

	// auto find1 = m_mapSessionPool_Connect.find(strIP_Port_old);
	// if(find1 != m_mapSessionPool_Connect.end())
	// {
	// 	Rigitaeda::Rigi_ClientTCP *pClient = find1->second;

	// 	m_mapSessionPool_Connect.erase(find1);

	// 	m_mapSessionPool_Connect.insert(std::make_pair(strIP_Port_new, pClient));
	// 	return true;
	// }

	// auto find2 = m_mapSessionPool_Connect.find(strIP_Port_old);
	// if(find2 != m_mapSessionPool_Connect.end())
	// {
	// 	Rigitaeda::Rigi_ClientTCP *pClient = find2->second;

	// 	m_mapSessionPool_Connect.erase(find2);

	// 	m_mapSessionPool_Connect.insert(std::make_pair(strIP_Port_new, pClient));
	// 	return true;
	// }

	return false;
}

void TCP_ClientMgr::Set_LogQ( __in MsgLog_Q *_pLogQ )
{
	m_pLogQ = _pLogQ;
}

bool TCP_ClientMgr::SendPacket_Round_Robin( __in std::string *_pData )
{
	while(true)
	{
		Rigitaeda::Rigi_ClientTCP * pSession = GetSession_Round_Robin();
		if(nullptr != pSession)
		{
			// pSession->ASync_Send( _pData->c_str(), _pData->length() );
			// return true;

			int nLength = pSession->Sync_Send( _pData->c_str(), _pData->length() );
			if(nLength > 0)
			{
				std::cout << "Send Packet >> " << *_pData << std::endl;
				return true;
			}

			std::cout << "[FAIL] Send Packet >> " << *_pData << std::endl;

			// 여기로 왔다는건 전송 실패인 경우
			// 다음 세션으로 다시 전송 시도한다 
			Del_SessionPool_Connected( pSession->Get_ServerIP(), pSession->Get_Port() );
			Add_SessionPool_DisConnected( pSession );
		}
		else
			return false;
	}

	return false;
}

void TCP_ClientMgr::OnEvent_Close( __in Rigitaeda::Rigi_ClientTCP *_pSession )
{
	std::cout << "[TCP_ClientMgr::OnEvent_Close] <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< OnClosed !! " << std::endl;

	if(false != m_bUse_Failover)
		Del_SessionPool_Connected( _pSession->Get_ServerIP(), _pSession->Get_Port() );

	Add_SessionPool_DisConnected( _pSession );
}

void TCP_ClientMgr::Check_Connect_Session()
{
	const std::lock_guard<std::mutex> lock(m_mtxSessionPool_DisConnect);

	auto itr = m_DqSessionPool_DisConnect.begin();
	while(itr != m_DqSessionPool_DisConnect.end())
	{
		Rigitaeda::Rigi_ClientTCP * pSession = *itr;
		if(false == pSession->IsConnected() )
		{
			std::cout << "[Reconnect] << IP = " << pSession->Get_SessionIP() << " | Port = " << pSession->Get_Port() << std::endl;

			if(true == pSession->Reconnect( m_io_service ) )
			{
				Add_SessionPool_Connected( pSession );

				itr = m_DqSessionPool_DisConnect.erase(itr);
				continue;
			}
		}

		itr++;
	}
}

void TCP_ClientMgr::Check_Connect_Session_Failover()
{
	const std::lock_guard<std::mutex> lock(m_mtxSessionPool_DisConnect);

	std::vector< std::deque<Rigitaeda::Rigi_ClientTCP *>::iterator > vecDel;

	auto itr = m_DqSessionPool_DisConnect.begin();
	while(itr != m_DqSessionPool_DisConnect.end())
	{
		Rigitaeda::Rigi_ClientTCP * pSession = *itr;
		if(false == pSession->IsConnected() )
		{
			std::cout << "[Reconnect] << IP = " << pSession->Get_SessionIP() << " | Port = " << pSession->Get_Port() << std::endl;

			if(true == pSession->Reconnect( m_io_service ) )
			{
				// fail-back 일 경우는 추가하면 안된다
				// pool에 삭제하지 않고 유지한다
				if(false != m_bUse_Failover)
					Add_SessionPool_Connected( pSession );

				vecDel.emplace_back(itr);
			}
		}

		itr++;
	}

	for(auto &itr : vecDel)
		m_DqSessionPool_DisConnect.erase(itr);
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

	auto itr = m_DqSessionPool_Connect.begin();
	while(itr != m_DqSessionPool_Connect.end())
	{
		Rigitaeda::Rigi_ClientTCP * pSession = *itr;
		delete pSession;
		itr++;
	}
	m_DqSessionPool_Connect.clear();

	itr = m_DqSessionPool_DisConnect.begin();
	while(itr != m_DqSessionPool_DisConnect.end())
	{
		Rigitaeda::Rigi_ClientTCP * pSession = *itr;
		delete pSession;
		itr++;
	}
	m_DqSessionPool_DisConnect.clear();
}

void TCP_ClientMgr::Add_SessionPool_Connected(  __in Rigitaeda::Rigi_ClientTCP *_pSession )
{
	const std::lock_guard<std::mutex> lock1(m_mtxSessionPool_Connect);

	m_DqSessionPool_Connect.emplace_back( _pSession );

	std::cout << "[Add_SessionPool_Connected] << IP = " << _pSession->Get_ServerIP() << " | Port = " << _pSession->Get_Port() << std::endl;
}

void TCP_ClientMgr::Del_SessionPool_Connected(  __in std::string _strServeIP, __in std::string _strServerPort )
{
	const std::lock_guard<std::mutex> lock1(m_mtxSessionPool_Connect);

	auto itr = m_DqSessionPool_Connect.begin();
	while(itr != m_DqSessionPool_Connect.end())
	{
		Rigitaeda::Rigi_ClientTCP * pSession = *itr;
		if( (pSession->Get_ServerIP() == _strServeIP) && (pSession->Get_Port() == _strServerPort ) )
		{
			itr = m_DqSessionPool_Connect.erase(itr);
			std::cout << "[Del_SessionPool_Connected] << IP = " << _strServeIP << " | Port = " << _strServerPort << std::endl;
			return;
		}
	}
}

void TCP_ClientMgr::Add_SessionPool_DisConnected(  	__in Rigitaeda::Rigi_ClientTCP *_pSession )
{
	const std::lock_guard<std::mutex> lock1(m_mtxSessionPool_DisConnect);

	m_DqSessionPool_DisConnect.emplace_back( _pSession );

	std::cout << "[Add_SessionPool_DisConnected] << IP = " << _pSession->Get_ServerIP() << " | Port = " << _pSession->Get_Port() << std::endl;
}

void TCP_ClientMgr::Del_SessionPool_DisConnected(  	__in std::string _strServerIP, __in std::string _strServerPort )
{
	const std::lock_guard<std::mutex> lock1(m_mtxSessionPool_DisConnect);

	auto itr = m_DqSessionPool_DisConnect.begin();
	while(itr != m_DqSessionPool_DisConnect.end())
	{
		Rigitaeda::Rigi_ClientTCP * pSession = *itr;
		if( (pSession->Get_ServerIP() == _strServerIP) && (pSession->Get_Port() == _strServerPort ) )
		{
			itr = m_DqSessionPool_DisConnect.erase(itr);
			std::cout << "[Del_SessionPool_DisConnected] << IP = " << _strServerIP << " | Port = " << _strServerPort << std::endl;
			return;
		}
	}
}

Rigitaeda::Rigi_ClientTCP * TCP_ClientMgr::GetSession_Round_Robin()
{
	const std::lock_guard<std::mutex> lock1(m_mtxSessionPool_Connect);
	if( true == m_DqSessionPool_Connect.empty())
		return nullptr;

	auto itr = m_DqSessionPool_Connect.begin();
	Rigitaeda::Rigi_ClientTCP * pSession = *itr;

	// 1개만 남을땐 굳이 돌릴 필요 없다
	if( 1 == (int)m_DqSessionPool_Connect.size())
		return pSession;

	// 처음걸 뽑아서 리턴해주고 제일 뒤로 돌린다
	// 순환 구조
	m_DqSessionPool_Connect.erase(itr);
	m_DqSessionPool_Connect.emplace_back(pSession);

	return pSession;
}

Rigitaeda::Rigi_ClientTCP * TCP_ClientMgr::GetSession_begin()
{
	const std::lock_guard<std::mutex> lock1(m_mtxSessionPool_Connect);
	if( true == m_DqSessionPool_Connect.empty())
		return nullptr;

	return *(m_DqSessionPool_Connect.begin());
}

bool TCP_ClientMgr::SendPacket_Fail_Over( __in std::string *_pLog )
{
	if(true == m_bUse_Failover)
	{
		Rigitaeda::Rigi_ClientTCP * pSession = GetSession_begin();

		while(nullptr != pSession)
		{
			//pSession->ASync_Send( _pLog->c_str(), _pLog->length() );
			int nLength = pSession->Sync_Send( _pLog->c_str(), _pLog->length() );
			if(nLength > 0)
			{
				std::cout << "[%s] Send Packet >> " << *_pLog << std::endl;
				return true;
			}

			OnEvent_Close(pSession);
		}

		return false;
	}
	else
	{
		// fail back
		// active 가 전송 실패 시 s/b 로 전송했다가 active 가 세션 연결 시 다시 active 로 전송
		if( true == m_DqSessionPool_Connect[SESSION_ACTIVE]->IsConnected() )
		{
			//m_DqSessionPool_Connect[SESSION_ACTIVE]->ASync_Send( _pLog->c_str(), _pLog->length() );
			int nLength = m_DqSessionPool_Connect[SESSION_ACTIVE]->Sync_Send( _pLog->c_str(), _pLog->length() );
			if(nLength > 0)
			{
				std::cout << "[Active] Send Packet >> " << *_pLog << std::endl;
				return true;
			}

			OnEvent_Close(m_DqSessionPool_Connect[SESSION_ACTIVE]);
		}
		else
		{
			if( true == m_DqSessionPool_Connect[SESSION_STANDBY]->IsConnected() )
			{
				//m_DqSessionPool_Connect[SESSION_STANDBY]->ASync_Send( _pLog->c_str(), _pLog->length() );
				int nLength = m_DqSessionPool_Connect[SESSION_STANDBY]->Sync_Send( _pLog->c_str(), _pLog->length() );
				if(nLength > 0)
				{
					std::cout << "[StandBy] Send Packet >> " << *_pLog << std::endl;
					return true;
				}
			}

			OnEvent_Close(m_DqSessionPool_Connect[SESSION_STANDBY]);
		}
	}

	std::cout << "[FAIL] Send Packet >> " << *_pLog << std::endl;

	return false;
}

bool TCP_ClientMgr::Add_Eng_Failover_Active( __in const char *_pszServerIP, __in int _nPort )
{	
	return Add_Eng(_pszServerIP, _nPort);
}

bool TCP_ClientMgr::Add_Eng_Failover_Standby( __in const char *_pszServerIP, __in int _nPort )
{	
	return Add_Eng(_pszServerIP, _nPort);
}

bool TCP_ClientMgr::Del_Eng_Failover( __in const char *_pszServerIP, __in int _nPort )
{
	// const std::lock_guard<std::mutex> lock1(m_mtxSessionPool_Connect);
	// const std::lock_guard<std::mutex> lock2(m_mtxSessionPool_DisConnect);

	// std::string strIP_Port = _pszServerIP;
	// strIP_Port += ":";
	// strIP_Port += std::to_string(_nPort);

	// auto find1 = m_mapSessionPool_Connect.find(strIP_Port);
	// if(find1 != m_mapSessionPool_Connect.end())
	// {
	// 	m_mapSessionPool_Connect.erase(find1);
	// 	return true;
	// }

	// auto find2 = m_mapSessionPool_DisConnect.find(strIP_Port);
	// if(find2 != m_mapSessionPool_DisConnect.end())
	// {
	// 	m_mapSessionPool_DisConnect.erase(find2);
	// 	return true;
	// }

	return false;
}

bool TCP_ClientMgr::Chg_Eng_Failover( 	__in const char *_pszServerIP_Old, __in int _nPort_Old,
								__in const char *_pszServerIP_New, __in int _nPort_New )
{
	// const std::lock_guard<std::mutex> lock1(m_mtxSessionPool_Connect);
	// const std::lock_guard<std::mutex> lock2(m_mtxSessionPool_DisConnect);

	// std::string strIP_Port_old = _pszServerIP_Old;
	// strIP_Port_old += ":";
	// strIP_Port_old += std::to_string(_nPort_Old);

	// std::string strIP_Port_new = _pszServerIP_New;
	// strIP_Port_new += ":";
	// strIP_Port_new += std::to_string(_nPort_New);

	// auto find1 = m_mapSessionPool_Connect.find(strIP_Port_old);
	// if(find1 != m_mapSessionPool_Connect.end())
	// {
	// 	Rigitaeda::Rigi_ClientTCP *pClient = find1->second;

	// 	m_mapSessionPool_Connect.erase(find1);

	// 	m_mapSessionPool_Connect.insert(std::make_pair(strIP_Port_new, pClient));
	// 	return true;
	// }

	// auto find2 = m_mapSessionPool_Connect.find(strIP_Port_old);
	// if(find2 != m_mapSessionPool_Connect.end())
	// {
	// 	Rigitaeda::Rigi_ClientTCP *pClient = find2->second;

	// 	m_mapSessionPool_Connect.erase(find2);

	// 	m_mapSessionPool_Connect.insert(std::make_pair(strIP_Port_new, pClient));
	// 	return true;
	// }

	return false;
}