#include "Session_FailBack.hpp"

Session_FailBack::Session_FailBack(  __in POLICY *_pPolicy, __in boost::asio::io_service *_pio_service ) : Session_Pool(_pPolicy)
{
	m_pio_service = _pio_service;

	if(nullptr != m_pio_service)
	{
		m_pSession_Active = new Session_RoundRobin( _pPolicy, _pio_service );
		m_pSession_Standby = new Session_RoundRobin( _pPolicy, _pio_service );
	}
}

Session_FailBack::~Session_FailBack()
{
	Clear();

	if(nullptr != m_pSession_Active)
	{
		delete m_pSession_Active;
		m_pSession_Active = nullptr;
	}
	if(nullptr != m_pSession_Standby)
	{
		delete m_pSession_Standby;
		m_pSession_Standby = nullptr;
	}

	m_pio_service = nullptr;
}

bool Session_FailBack::Add_SessionPool_Connected( __in Rigitaeda::Rigi_ClientTCP *_pSession, __in bool _bActive )
{
	if(true == _bActive)	
		return m_pSession_Active->Add_SessionPool_Connected( _pSession, true );
	else
		return m_pSession_Standby->Add_SessionPool_Connected( _pSession, false );
}

bool Session_FailBack::Add_SessionPool_Connected_Active( __in Rigitaeda::Rigi_ClientTCP *_pSession )
{
	return m_pSession_Active->Add_SessionPool_Connected( _pSession, true );
}

bool Session_FailBack::Add_SessionPool_Connected_Standby( __in Rigitaeda::Rigi_ClientTCP *_pSession )
{
	return m_pSession_Standby->Add_SessionPool_Connected( _pSession, false );
}

bool Session_FailBack::Add_SessionPool_DisConnected( __in Rigitaeda::Rigi_ClientTCP *_pSession, __in bool _bActive )
{
	if(true == _bActive)
		return m_pSession_Active->Add_SessionPool_DisConnected( _pSession, true );
	else
		return m_pSession_Standby->Add_SessionPool_DisConnected( _pSession, false );
}

bool Session_FailBack::Add_SessionPool_DisConnected_Active( __in Rigitaeda::Rigi_ClientTCP *_pSession )

{
	return m_pSession_Active->Add_SessionPool_DisConnected( _pSession, true );
}

bool Session_FailBack::Add_SessionPool_DisConnected_Standby( __in Rigitaeda::Rigi_ClientTCP*_pSession )
{
	return m_pSession_Standby->Add_SessionPool_DisConnected( _pSession, false );
}

bool Session_FailBack::Del_SessionPool_Connected( __in Rigitaeda::Rigi_ClientTCP *_pSession, __in bool _bActive )
{
	if(true == _bActive)
		return m_pSession_Active->Del_SessionPool_Connected( _pSession, true );
	else
		return m_pSession_Standby->Del_SessionPool_Connected( _pSession, false );
}

bool Session_FailBack::Del_SessionPool_Connected_Active( __in Rigitaeda::Rigi_ClientTCP *_pSession )
{
	return m_pSession_Active->Del_SessionPool_Connected( _pSession, true );
}

bool Session_FailBack::Del_SessionPool_Connected_Standby( __in Rigitaeda::Rigi_ClientTCP *_pSession )
{
	return m_pSession_Standby->Del_SessionPool_Connected( _pSession, false );
}

bool Session_FailBack::Del_SessionPool_DisConnected( __in Rigitaeda::Rigi_ClientTCP *_pSession, __in bool _bActive)
{
	if(true == _bActive)
		return m_pSession_Active->Del_SessionPool_DisConnected( _pSession, true );
	else
		return m_pSession_Standby->Del_SessionPool_DisConnected( _pSession, false );
}

bool Session_FailBack::Del_SessionPool_DisConnected_Active( __in Rigitaeda::Rigi_ClientTCP *_pSession )
{
	return m_pSession_Active->Del_SessionPool_DisConnected( _pSession, true );
}

bool Session_FailBack::Del_SessionPool_DisConnected_Standby( __in Rigitaeda::Rigi_ClientTCP *_pSession )
{
	return m_pSession_Standby->Del_SessionPool_DisConnected( _pSession, false );
}

// 3번째 인자에 mutex 를 받으므로써 호출하는 곳에서 강제로 mutx gaurd를 호출하도록 한다
bool Session_FailBack::IsExist_SessionPool_Connected_Active( __in Rigitaeda::Rigi_ClientTCP *_pSession )
{
	return m_pSession_Active->IsExist_SessionPool_Connected( _pSession );
}

bool Session_FailBack::IsExist_SessionPool_Connected_Standby( __in Rigitaeda::Rigi_ClientTCP *_pSession )
{
	return m_pSession_Standby->IsExist_SessionPool_Connected( _pSession );
}

bool Session_FailBack::IsExist_SessionPool_DisConnected_Active( __in Rigitaeda::Rigi_ClientTCP *_pSession )
{
	return m_pSession_Active->IsExist_SessionPool_DisConnected( _pSession );
}

bool Session_FailBack::IsExist_SessionPool_DisConnected_Standby( __in Rigitaeda::Rigi_ClientTCP *_pSession )
{
	return m_pSession_Standby->IsExist_SessionPool_DisConnected( _pSession );
}

Rigitaeda::Rigi_ClientTCP * Session_FailBack::Get_Send_Session()
{
	Rigitaeda::Rigi_ClientTCP * pSession = nullptr;

	// active 쪽 설정 세션 수 확인
	if( Get_Policy()->m_vecFailOver_Change_Limit[SESSION_ACTIVE] <= (int)m_pSession_Active->GetSize_Connected() )
	{
		pSession = m_pSession_Active->Get_Send_Session();
		// active가 null 이면 stand-by 쪽 설정 세션 수 확인
		if(nullptr == pSession)
		{
			if( Get_Policy()->m_vecFailOver_Change_Limit[SESSION_STANDBY] <= (int)m_pSession_Standby->GetSize_Connected() )
				pSession = m_pSession_Standby->Get_Send_Session();
		}
	}
	else
	{
		if( Get_Policy()->m_vecFailOver_Change_Limit[SESSION_STANDBY] <= (int)m_pSession_Standby->GetSize_Connected() )
			pSession = m_pSession_Standby->Get_Send_Session();
	}

	return pSession;
}

void Session_FailBack::Clear()
{
	m_pSession_Active->Clear();
	m_pSession_Standby->Clear();
}

bool Session_FailBack::Reconnect_DisConnect_Pool()
{
	bool bRet_Active = m_pSession_Active->Reconnect_DisConnect_Pool();
	bool bRet_Standby = m_pSession_Standby->Reconnect_DisConnect_Pool();

	if( true == bRet_Active && true == bRet_Standby )
		return true;
	else
		return false;
}