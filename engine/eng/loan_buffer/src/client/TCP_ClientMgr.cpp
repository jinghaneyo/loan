// regex 헤더 안에 이미 __in, __out 정의 되어있다
// 해서 Rigi_Def.hpp 보다 먼저 선언을 해야 한다
#include <regex>
#include <chrono>
#include "TCP_ClientMgr.hpp"

TCP_ClientMgr::TCP_ClientMgr( __in MsgLog_Q *_pLogQ, __in DATA_POLICY *_pPolicy ) : m_pLogQ(_pLogQ), m_pPolicy(_pPolicy)
{ 
	m_bRun_Thread = false;

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
		std::string strSendData;
		std::vector<std::string *> vecLines;

		while(true == m_bRun_Thread)
		{
			bRet_Send = false;
			_MsgLog_Data *pLog = m_pLogQ->Pop_front();
			if(nullptr != pLog) 
			{
				if( true == pLog->bProtobuf )
				{
					loan::MsgLog msgLog;
					msgLog.ParseFromString(pLog->pstrLog->c_str());

					if ( MSG_TYPE_GEN == msgLog.msg_type() )
					{
						strSendData = "";
						msgLog.SerializeToString(&strSendData);

						// todo
						//Input_Filter( msgLog );

						bRet_Send = SendPacket(&strSendData);

						// 실패인 경우는 모든 세션이 전송 실패인 경우이다. 따라서, 현재 데이터는 다시 전송할 수 있도록 큐의 앞에 넣어 주도록 하자
						if(false == bRet_Send)
							m_pLogQ->Push_front(pLog->pstrLog, false);
						else
							pLog->Clear();

						delete pLog;
					}
					else
					{
						pLog->Clear();
						delete pLog;
					}
				}
				else
				{
					bRet_Send = SendPacket(pLog->pstrLog);

					// 실패인 경우는 모든 세션이 전송 실패인 경우이다. 따라서, 현재 데이터는 다시 전송할 수 있도록 큐의 앞에 넣어 주도록 하자
					if(false == bRet_Send)
						m_pLogQ->Push_front(pLog->pstrLog, false);
					else
						// 전송 성공했으므로 더이상 로그 메모리는 사용하지않는다. 해서 삭제한다
						pLog->Clear();

					delete pLog;
				}
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
			int nSend_Length = pSession->Sync_Send( _pData->c_str(), _pData->length() );
			if(nSend_Length == (int)_pData->length())
			{
				std::cout << "[TCP_ClientMgr::SendPacket][SUCC] SEND >> " << *_pData << std::endl;
				return true;
			}

			std::cout << "[TCP_ClientMgr::SendPacket][FAIL] >> " << *_pData << std::endl;

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

bool TCP_ClientMgr::Input_Filter( __in loan::MsgLog &_Packet )
{
	if(nullptr == m_pLogQ || nullptr == m_pPolicy)
	{
		//ASSERT(0 && "[TCP_Session::Input_Filter] m_pLogQ is nullptr");
		return false;
	}

	// 큐에 설정 리미트에 도달하면 중지 명령을 보내자 
	if(m_pLogQ->GetQ_LimitSize() == (int)m_pLogQ->GetQ_Size())
	{
		loan::MsgLog msg_stop;
		msg_stop.set_msg_type( MSG_TYPE_GEN );
		//msg_stop.set_msg_cmd( (int)MsgLog_Cmd_Crolling::STOP_REQU );
		msg_stop.set_msg_cmd( 1 );

		std::string strSendData;
		msg_stop.SerializeToString(&strSendData);
		// Sync_Send( strSendData.c_str(), msg_stop.ByteSizeLong() );
	}

	// full 크기에 도착하면 버린다
	if(m_pLogQ->GetQ_FullSize() == (int)m_pLogQ->GetQ_Size())
		return false;

	bool bIsPushQ = false;
	bool bPass = false;
	std::string strIP_Port;
	/*
	for(auto &policy : *m_pPolicy)
	{
		// ip port 검색
		// 아무것도 없으면 무조건 다 받는다
		if( true == policy.second.vec_ip_port.empty())
			bPass = true;
		else
		{
			for(auto &ip_port : policy.second.vec_ip_port )
			{
				if( ip_port == m_strIP_Port )
				{
					bPass = true;
					break;
				}
			}
		}
		if( false == bPass )
			continue;

		// 정규식 검색
		// 아무것도 없으면 무조건 다 받는다
		if( true == policy.second.vec_regex.empty())
			bPass = true;
		else
		{
			for(auto &reg : policy.second.vec_regex)
			{
				std::regex reg_ex(reg);
				std::smatch result;

				if( std::regex_search(_Packet.logcontents(), result, reg_ex) )
				{
					bPass = true;
					break;
				}
			}
		}
		if( false == bPass )
			continue;

		// 서비스 로그
		// 아무것도 없으면 무조건 다 받는다
		if( true == policy.second.vec_service.empty())
			bPass = true;
		else
		{
			for(auto &service : policy.second.vec_service)
			{
				if(service == _Packet.service_name())
				{
					bPass = true;
					break;
				}
			}
		}
		if( false == bPass )
			continue;

		if (true == bPass)
		{
			std::string *pstrSendData = new std::string();
			_Packet.SerializeToString(&(*pstrSendData));

			// Push_Data 안에 delete를 자동 호출 해준다. 따로 delete 를 호출 하지 말자 
			m_pLogQ->Push_back( policy.first.c_str(), pstrSendData );

			bIsPushQ = true;
		}
	}
//*/
	return bIsPushQ;
}