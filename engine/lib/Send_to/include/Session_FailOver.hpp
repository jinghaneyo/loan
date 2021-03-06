#ifndef SESSION_FAILOVER_H_
#define SESSION_FAILOVER_H_

#include <mutex>
#include <deque>
#include "Session_RoundRobin.hpp"

class Session_FailOver : public Session_Pool
{
public:
	Session_FailOver(  __in POLICY *_pPolicy, __in boost::asio::io_service *_pio_service );
	virtual ~Session_FailOver();
	// Session_FailOver( __in Session_FailOver &r ) delete;
private:
	Session_RoundRobin 		*m_pSession_Active;
	Session_RoundRobin 		*m_pSession_Standby;

	boost::asio::io_service *m_pio_service;

	bool 					m_bActive;
public:
	virtual bool Add_SessionPool_Connected( __in Rigitaeda::Rigi_ClientTCP *_pSession, __in bool _Active );
	virtual bool Add_SessionPool_Connected_Active( __in Rigitaeda::Rigi_ClientTCP *_pSession );
	virtual bool Add_SessionPool_Connected_Standby( __in Rigitaeda::Rigi_ClientTCP *_pSession );

	virtual bool Add_SessionPool_DisConnected( __in Rigitaeda::Rigi_ClientTCP *_pSession, __in bool _bActive );
	virtual bool Add_SessionPool_DisConnected_Active( __in Rigitaeda::Rigi_ClientTCP *_pSession );
	virtual bool Add_SessionPool_DisConnected_Standby( __in Rigitaeda::Rigi_ClientTCP *_pSession );

	virtual bool Del_SessionPool_Connected( __in Rigitaeda::Rigi_ClientTCP *_pSession, __in bool _bActive );
	virtual bool Del_SessionPool_Connected_Active( __in Rigitaeda::Rigi_ClientTCP *_pSession );
	virtual bool Del_SessionPool_Connected_Standby( __in Rigitaeda::Rigi_ClientTCP *_pSession );

	virtual bool Del_SessionPool_DisConnected( __in Rigitaeda::Rigi_ClientTCP *_pSession, __in bool _bActive );
	virtual bool Del_SessionPool_DisConnected_Active( __in Rigitaeda::Rigi_ClientTCP *_pSession );
	virtual bool Del_SessionPool_DisConnected_Standby( __in Rigitaeda::Rigi_ClientTCP *_pSession );

	virtual bool IsExist_SessionPool_Connected_Active( 	__in Rigitaeda::Rigi_ClientTCP *_pSession );
	virtual bool IsExist_SessionPool_Connected_Standby( __in Rigitaeda::Rigi_ClientTCP *_pSession );
	virtual bool IsExist_SessionPool_DisConnected_Active( 	__in Rigitaeda::Rigi_ClientTCP *_pSession );
	virtual bool IsExist_SessionPool_DisConnected_Standby( 	__in Rigitaeda::Rigi_ClientTCP *_pSession );

	virtual Rigitaeda::Rigi_ClientTCP * Get_Send_Session();

	virtual bool Reconnect_DisConnect_Pool();

	virtual void Clear();
};

#endif