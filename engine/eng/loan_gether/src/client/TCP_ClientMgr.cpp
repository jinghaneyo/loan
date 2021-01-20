// regex 헤더 안에 이미 __in, __out 정의 되어있다
// 해서 Rigi_Def.hpp 보다 먼저 선언을 해야 한다
#include <regex>
#include <chrono>
#include "TCP_ClientMgr.hpp"

TCP_ClientMgr::TCP_ClientMgr( __in MsgLog_Q *_pLogQ, __in DATA_POLICY *_pPolicy ) : m_pLogQ(_pLogQ), m_pPolicy(_pPolicy)
{ 
	m_bRun_Thread = false;
	// m_nFailOver_Active_Standby = SESSION_ACTIVE;

	m_pSendSession = nullptr;
}

TCP_ClientMgr::~TCP_ClientMgr() 
{ 
	m_pLogQ = nullptr;
	m_pPolicy = nullptr;

	Clear_Eng();
}

void TCP_ClientMgr::Run()
{
	if(nullptr == m_pLogQ || nullptr == m_pPolicy )
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
			m_pSendSession->Reconnect_DisConnect_Pool();

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
			std::string *pLog = m_pLogQ->Pop_front();
			if(nullptr != pLog) 
			{
				bRet_Send = SendPacket(pLog);

				// 실패인 경우는 모든 세션이 전송 실패인 경우이다. 따라서, 현재 데이터는 다시 전송할 수 있도록 큐의 앞에 넣어 주도록 하자
				if(false == bRet_Send)
					m_pLogQ->Push_front(pLog);
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

void TCP_ClientMgr::Set_LogQ( __in MsgLog_Q *_pLogQ )
{
	m_pLogQ = _pLogQ;
}

bool TCP_ClientMgr::SendPacket( __in std::string *_pData )
{
	while(true)
	{
		TCP_Client * pSession = (TCP_Client *)m_pSendSession->Get_Send_Session();
		if(nullptr != pSession)
		{
			// pSession->ASync_Send( _pData->c_str(), _pData->length() );
			// return true;
			int nLength = pSession->Sync_Send( _pData->c_str(), _pData->length() );
			if(nLength == (int)_pData->length())
			{
				std::cout << "[TCP_ClientMgr::SendPacke][SUCC] >> " << *_pData << std::endl;

				return true;
			}

			std::cout << "[TCP_ClientMgr::SendPacke][FAIL] >> " << *_pData << std::endl;

			// 여기로 왔다는건 전송 실패인 경우
			// 다음 세션으로 다시 전송 시도한다 
			bool bActive = pSession->Get_Session_Type();
			m_pSendSession->Del_SessionPool_Connected( pSession, bActive );
			m_pSendSession->Add_SessionPool_DisConnected( pSession, bActive );
		}
		else
			return false;
	}

	return false;
}

void TCP_ClientMgr::OnEvent_Close( __in Rigitaeda::Rigi_ClientTCP *_pSession )
{
	std::cout << "[TCP_ClientMgr::OnEvent_Close] <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< OnClosed !! " << std::endl;

	TCP_Client *pClient = (TCP_Client *)_pSession;
	bool bActive = pClient->Get_Session_Type();
	m_pSendSession->Del_SessionPool_Connected( (TCP_Client *)_pSession, bActive );
	m_pSendSession->Add_SessionPool_DisConnected( (TCP_Client *)_pSession, bActive );
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
	if(nullptr != m_pSendSession)
	{
		m_pSendSession->Clear();
		delete m_pSendSession;
		m_pSendSession = nullptr;
	}
}

bool TCP_ClientMgr::Add_Eng_RoundRobin( __in const char *_pszServerIP, __in const char *_pszPort )
{
	if( nullptr == m_pSendSession )
		m_pSendSession = new Session_RoundRobin( m_pPolicy, &m_io_service );

	TCP_Client *pClient = new TCP_Client(10240);
	pClient->Add_Event_Handler_Close( std::bind(&TCP_ClientMgr::OnEvent_Close, this, pClient) );

	std::cout << "[Add_Eng_RoundRobin] IP = " << _pszServerIP << " | Port = " << _pszPort << std::endl;

	// 일단 세션 풀을 만들기 위해서 연결을 시도하자 
	if( true == pClient->Connect( _pszServerIP, _pszPort, m_io_service ) )
	{
		if(false == m_pSendSession->Add_SessionPool_Connected(pClient, true))
			delete pClient;
	}
	else
	{
		if(false == m_pSendSession->Add_SessionPool_DisConnected(pClient, true))
			delete pClient;
	}

	return true;
}

bool TCP_ClientMgr::Add_Eng_FailOver_Active( __in const char *_pszServerIP, __in const char *_pszPort )
{
	if( nullptr == m_pSendSession )
		m_pSendSession = new Session_FailOver( m_pPolicy, &m_io_service );

	TCP_Client *pClient = new TCP_Client(10240);
	pClient->Add_Event_Handler_Close( std::bind(&TCP_ClientMgr::OnEvent_Close, this, pClient) );
	pClient->Set_Session_Type(true);

	std::cout << "[Add_Eng_FailOver_Active] IP = " << _pszServerIP << " | Port = " << _pszPort << std::endl;

	// 일단 세션 풀을 만들기 위해서 연결을 시도하자 
	if( true == pClient->Connect( _pszServerIP, _pszPort, m_io_service ) )
	{
		if(false == m_pSendSession->Add_SessionPool_Connected(pClient, true))
			delete pClient;
	}
	else
	{
		if(false == m_pSendSession->Add_SessionPool_DisConnected(pClient, true))
			delete pClient;
	}

	return true;
}

bool TCP_ClientMgr::Add_Eng_FailOver_Standby( __in const char *_pszServerIP, __in const char *_pszPort )
{
	if( nullptr == m_pSendSession )
		m_pSendSession = new Session_FailOver( m_pPolicy, &m_io_service );

	TCP_Client *pClient = new TCP_Client(10240);
	pClient->Add_Event_Handler_Close( std::bind(&TCP_ClientMgr::OnEvent_Close, this, pClient) );
	pClient->Set_Session_Type(false);

	std::cout << "[Add_Eng_FailOver_Standby] IP = " << _pszServerIP << " | Port = " << _pszPort << std::endl;

	// 일단 세션 풀을 만들기 위해서 연결을 시도하자 
	if( true == pClient->Connect( _pszServerIP, _pszPort, m_io_service ) )
	{
		if(false == m_pSendSession->Add_SessionPool_Connected(pClient, false))
			delete pClient;
	}
	else
	{
		if(false == m_pSendSession->Add_SessionPool_DisConnected(pClient, false))
			delete pClient;
	}

	return true;
}

bool TCP_ClientMgr::Add_Eng_FailBack_Active( __in const char *_pszServerIP, __in const char *_pszPort )
{
	if( nullptr == m_pSendSession )
		m_pSendSession = new Session_FailBack( m_pPolicy, &m_io_service );

	TCP_Client *pClient = new TCP_Client(10240);
	pClient->Add_Event_Handler_Close( std::bind(&TCP_ClientMgr::OnEvent_Close, this, pClient) );
	pClient->Set_Session_Type(true);

	std::cout << "[Add_Eng_FailBack_Active] IP = " << _pszServerIP << " | Port = " << _pszPort << std::endl;

	// 일단 세션 풀을 만들기 위해서 연결을 시도하자 
	if( true == pClient->Connect( _pszServerIP, _pszPort, m_io_service ) )
	{
		if(false == m_pSendSession->Add_SessionPool_Connected(pClient, true))
			delete pClient;
	}
	else
	{
		if(false == m_pSendSession->Add_SessionPool_DisConnected(pClient, true))
			delete pClient;
	}

	return true;
}

bool TCP_ClientMgr::Add_Eng_FailBack_Standby( __in const char *_pszServerIP, __in const char *_pszPort )
{
	if( nullptr == m_pSendSession )
		m_pSendSession = new Session_FailBack( m_pPolicy, &m_io_service );

	TCP_Client *pClient = new TCP_Client(10240);
	pClient->Add_Event_Handler_Close( std::bind(&TCP_ClientMgr::OnEvent_Close, this, pClient) );
	pClient->Set_Session_Type(false);

	std::cout << "[Add_Eng_FailBack_Standby] IP = " << _pszServerIP << " | Port = " << _pszPort << std::endl;

	// 일단 세션 풀을 만들기 위해서 연결을 시도하자 
	if( true == pClient->Connect( _pszServerIP, _pszPort, m_io_service ) )
	{
		if(false == m_pSendSession->Add_SessionPool_Connected(pClient, false))
			delete pClient;
	}
	else
	{
		if(false == m_pSendSession->Add_SessionPool_DisConnected(pClient, false))
			delete pClient;
	}

	return true;
}