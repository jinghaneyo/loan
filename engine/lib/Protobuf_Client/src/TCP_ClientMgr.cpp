// regex 헤더 안에 이미 __in, __out 정의 되어있다
// 해서 Rigi_Def.hpp 보다 먼저 선언을 해야 한다
#include <regex>
#include <chrono>
#include "TCP_ClientMgr.hpp"
#include "Policy.hpp"
#include "MsgLog_Q.hpp"
#include "Session_RoundRobin.hpp"
#include "Session_FailOver.hpp"
#include "Session_FailBack.hpp"
#include "TCP_Client.hpp"
#include "loan_util.hpp"

TCP_ClientMgr::TCP_ClientMgr( __in MsgLog_Q *_pLogQ, __in POLICY *_pPolicy ) : m_pLogQ(_pLogQ), m_pPolicy(_pPolicy)
{ 
	m_bRun_Thread = false;
	m_pSendSession = nullptr;
	m_Event_Send_Filter = nullptr;
	m_eSend_Rule = SEND_RULE::NONE;
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
		ASSERT(0 && "[TCP_ClientMgr::Async_Run] m_pLogQ is nullptr");
		return ;
	}

    m_io_context.restart();
	boost::system::error_code ec;
	m_bRun_Thread = true;

	// -------------------------------------------------------
	// 서버 접속 스레드
	if(SEND_RULE::SAVE_FILE != m_eSend_Rule && SEND_RULE::NONE != m_eSend_Rule)
	{
		m_thr_conn = std::thread( [&]()
		{
			while(true == m_bRun_Thread)
			{
				m_pSendSession->Reconnect_DisConnect_Pool();

				// 100ms 단위로 쪼개서 sleep → Stop() 시 최대 100ms 안에 종료
				for(int i = 0; i < 100 && m_bRun_Thread; ++i)
					std::this_thread::sleep_for( std::chrono::milliseconds(100) );
			}
		});
		// detach() 제거: Stop()에서 join()으로 정상 종료
	}
	// -------------------------------------------------------

	// -------------------------------------------------------
	// 데이터 전송 스레드
	m_thr_send = std::thread( [&]()
	{
		bool bRet_Send = false;
		bool bFilter = false;

		while(true == m_bRun_Thread)
		{
			bRet_Send = false;
			bFilter = (nullptr == m_Event_Send_Filter);

			std::string *pLog = m_pLogQ->Pop_front();
			if(nullptr != pLog)
			{
				if(nullptr != m_Event_Send_Filter)
					bFilter = m_Event_Send_Filter( *pLog );

				if(true == bFilter)
				{
					if(SEND_RULE::SAVE_FILE == m_eSend_Rule)
						bRet_Send = Write_Data( pLog );
					else if(SEND_RULE::NONE != m_eSend_Rule)
						bRet_Send = SendPacket( pLog );

					if(false == bRet_Send)
					{
						// 전송 실패 시 큐에 반환 — 소유권을 큐에 돌려주므로 delete 하지 않음
						m_pLogQ->Push_front( pLog );
						pLog = nullptr;
					}
				}

				delete pLog;  // bRet_Send true이거나 필터 거부 시에만 실행
			}

			if(false == bRet_Send)
				std::this_thread::sleep_for( std::chrono::milliseconds(1) );
		}
	});
	// detach() 제거: Stop()에서 join()으로 정상 종료
	// -------------------------------------------------------

	m_io_context.run(ec);

	return ;
}

void TCP_ClientMgr::Set_LogQ( __in MsgLog_Q *_pLogQ )
{
	m_pLogQ = _pLogQ;
}

bool TCP_ClientMgr::SendPacket( __in std::string *_pstrProtobufRaw )
{
	// 0x17 0x17 을추가해서 패킷의 끝임을 알려주자
	if( 0x17 != _pstrProtobufRaw->at(_pstrProtobufRaw->length() - 2) && 0x17 != _pstrProtobufRaw->at(_pstrProtobufRaw->length() - 1) )
	{
		*_pstrProtobufRaw += 0x17;
		*_pstrProtobufRaw += 0x17;
	}

	while(true)
	{
		TCP_Client * pSession = (TCP_Client *)m_pSendSession->Get_Send_Session();
		if(nullptr != pSession)
		{
			// pSession->ASync_Send( _pData->c_str(), _pData->length() );
			// return true;
			int nSend_Length = pSession->Sync_Send( _pstrProtobufRaw->c_str(), _pstrProtobufRaw->length() );
			if(nSend_Length == (int)_pstrProtobufRaw->length())
			{
				std::cout << "[TCP_ClientMgr::SendPacket][SUCC] SEND >> " << _pstrProtobufRaw->c_str() << std::endl;
				return true;
			}

			std::cout << "[TCP_ClientMgr::SendPacket][FAIL] >> " << _pstrProtobufRaw->c_str() << std::endl;

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

	if(m_thr_conn.joinable()) m_thr_conn.join();
	if(m_thr_send.joinable()) m_thr_send.join();

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
	if( SEND_RULE::ROUND_ROBIN != m_eSend_Rule )
	{
		if( nullptr != m_pSendSession )
		{
			delete m_pSendSession;
			m_pSendSession = nullptr;
		}
	}

	m_eSend_Rule = SEND_RULE::ROUND_ROBIN;

	if( nullptr == m_pSendSession )
		m_pSendSession = new Session_RoundRobin( m_pPolicy, &m_io_context );

	TCP_Client *pClient = new TCP_Client(10240);
	pClient->Add_Event_Handler_Close( std::bind(&TCP_ClientMgr::OnEvent_Close, this, pClient) );

	std::cout << "[Add_Eng_RoundRobin] IP = " << _pszServerIP << " | Port = " << _pszPort << std::endl;

	// 일단 세션 풀을 만들기 위해서 연결을 시도하자 
	if( true == pClient->Connect( _pszServerIP, _pszPort, m_io_context ) )
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
	if( SEND_RULE::FAIL_OVER != m_eSend_Rule )
	{
		if( nullptr != m_pSendSession )
		{
			delete m_pSendSession;
			m_pSendSession = nullptr;
		}
	}

	m_eSend_Rule = SEND_RULE::FAIL_OVER;

	if( nullptr == m_pSendSession )
		m_pSendSession = new Session_FailOver( m_pPolicy, &m_io_context );

	TCP_Client *pClient = new TCP_Client(10240);
	pClient->Add_Event_Handler_Close( std::bind(&TCP_ClientMgr::OnEvent_Close, this, pClient) );
	pClient->Set_Session_Type(true);

	std::cout << "[Add_Eng_FailOver_Active] IP = " << _pszServerIP << " | Port = " << _pszPort << std::endl;

	// 일단 세션 풀을 만들기 위해서 연결을 시도하자 
	if( true == pClient->Connect( _pszServerIP, _pszPort, m_io_context ) )
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
	if( SEND_RULE::FAIL_OVER != m_eSend_Rule )
	{
		if( nullptr != m_pSendSession )
		{
			delete m_pSendSession;
			m_pSendSession = nullptr;
		}
	}

	m_eSend_Rule = SEND_RULE::FAIL_OVER;

	if( nullptr == m_pSendSession )
		m_pSendSession = new Session_FailOver( m_pPolicy, &m_io_context );

	TCP_Client *pClient = new TCP_Client(10240);
	pClient->Add_Event_Handler_Close( std::bind(&TCP_ClientMgr::OnEvent_Close, this, pClient) );
	pClient->Set_Session_Type(false);

	std::cout << "[Add_Eng_FailOver_Standby] IP = " << _pszServerIP << " | Port = " << _pszPort << std::endl;

	// 일단 세션 풀을 만들기 위해서 연결을 시도하자 
	if( true == pClient->Connect( _pszServerIP, _pszPort, m_io_context ) )
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
	if( SEND_RULE::FAIL_BACK != m_eSend_Rule )
	{
		if( nullptr != m_pSendSession )
		{
			delete m_pSendSession;
			m_pSendSession = nullptr;
		}
	}

	m_eSend_Rule = SEND_RULE::FAIL_BACK;

	if( nullptr == m_pSendSession )
		m_pSendSession = new Session_FailBack( m_pPolicy, &m_io_context );

	TCP_Client *pClient = new TCP_Client(10240);
	pClient->Add_Event_Handler_Close( std::bind(&TCP_ClientMgr::OnEvent_Close, this, pClient) );
	pClient->Set_Session_Type(true);

	std::cout << "[Add_Eng_FailBack_Active] IP = " << _pszServerIP << " | Port = " << _pszPort << std::endl;

	// 일단 세션 풀을 만들기 위해서 연결을 시도하자 
	if( true == pClient->Connect( _pszServerIP, _pszPort, m_io_context ) )
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
	if( SEND_RULE::FAIL_BACK != m_eSend_Rule )
	{
		if( nullptr != m_pSendSession )
		{
			delete m_pSendSession;
			m_pSendSession = nullptr;
		}
	}

	m_eSend_Rule = SEND_RULE::FAIL_BACK;

	if( nullptr == m_pSendSession )
		m_pSendSession = new Session_FailBack( m_pPolicy, &m_io_context );

	TCP_Client *pClient = new TCP_Client(10240);
	pClient->Add_Event_Handler_Close( std::bind(&TCP_ClientMgr::OnEvent_Close, this, pClient) );
	pClient->Set_Session_Type(false);

	std::cout << "[Add_Eng_FailBack_Standby] IP = " << _pszServerIP << " | Port = " << _pszPort << std::endl;

	// 일단 세션 풀을 만들기 위해서 연결을 시도하자 
	if( true == pClient->Connect( _pszServerIP, _pszPort, m_io_context ) )
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

bool TCP_ClientMgr::Set_SaveFile( __in const char *_pszFilePath, __in const char *_pszLocale )
{
	if( SEND_RULE::SAVE_FILE != m_eSend_Rule )
	{
		if( nullptr != m_pSendSession )
		{
			delete m_pSendSession;
			m_pSendSession = nullptr;
		}
	}

	m_eSend_Rule = SEND_RULE::SAVE_FILE;

	m_strSavePath_Pattern = _pszFilePath;
	m_strLocale = _pszLocale;

	std::cout << "[TCP_ClientMgr::Set_SaveFile] Path = " << m_strSavePath_Pattern << " | Locale = " << m_strLocale << std::endl;

	return true;
}

bool TCP_ClientMgr::Write_Data( __in std::string *_pstrProtobufRaw )
{
	if( SEND_RULE::SAVE_FILE != m_eSend_Rule )
		return false;

	if(nullptr != m_Event_Write)
		return m_Event_Write( m_of_Data, *_pstrProtobufRaw );
	else
	{
		if(true == m_strSavePath_Pattern.empty())
			return false;

		if(false == m_of_Data.is_open())
		{
			std::string strFilePath = Replace_Macro(m_strSavePath_Pattern);

			if( false == OpenFile( strFilePath.c_str(), m_strLocale.c_str(), m_of_Data ) )
				return false;
		}

		try
		{
			m_of_Data << _pstrProtobufRaw->c_str() << std::endl;
			std::cout << "[TCP_ClientMgr::Write_Data] Data << " << _pstrProtobufRaw->c_str() << std::endl;
		}
		catch(std::exception const &e)
		{
			std::cerr << "[Exception][TCP_ClientMgr::Write_Data] Err = " << e.what() << "\n";
			return false;
		}
		catch(...)
		{
			std::cerr << "[Exception][TCP_ClientMgr::Write_Data] Err = Unknown" <<  "\n";
			return false;
		}

		return true;
	}
}

POLICY * TCP_ClientMgr::Get_Policy()	
{	
	return m_pPolicy;	
}