#ifndef SESSION_FAILBACK_H_
#define SESSION_FAILBACK_H_

#include <mutex>
#include <deque>
#include "Session_RoundRobin.hpp"

class Session_FailBack : public Session_Pool
{
public:
	Session_FailBack(  __in DATA_POLICY *_pPolicy, __in boost::asio::io_service *_pio_service );
	virtual ~Session_FailBack();
	// Session_FailBack( __in Session_FailBack &r ) delete;
private:
	Session_RoundRobin 		*m_pSession_Active;
	Session_RoundRobin 		*m_pSession_Standby;

	boost::asio::io_service *m_pio_service;
public:
	virtual bool Add_SessionPool_Connected( __in TCP_Client *_pSession, __in int _nOption = -1 );
	virtual bool Add_SessionPool_Connected_Active( __in TCP_Client *_pSession );
	virtual bool Add_SessionPool_Connected_Standby( __in TCP_Client *_pSession );

	virtual bool Add_SessionPool_DisConnected( __in TCP_Client *_pSession, __in int _nOption = -1 );
	virtual bool Add_SessionPool_DisConnected_Active( __in TCP_Client *_pSession );
	virtual bool Add_SessionPool_DisConnected_Standby( __in TCP_Client *_pSession );

	virtual bool Del_SessionPool_Connected( __in TCP_Client *_pSession, __in int _nOption = -1 );
	virtual bool Del_SessionPool_Connected_Active( __in TCP_Client *_pSession );
	virtual bool Del_SessionPool_Connected_Standby( __in TCP_Client *_pSession );

	virtual bool Del_SessionPool_DisConnected( __in TCP_Client *_pSession, __in int _nOption = -1 );
	virtual bool Del_SessionPool_DisConnected_Active( __in TCP_Client *_pSession );
	virtual bool Del_SessionPool_DisConnected_Standby( __in TCP_Client *_pSession );

	virtual bool IsExist_SessionPool_Connected_Active( 	__in TCP_Client *_pSession );
	virtual bool IsExist_SessionPool_Connected_Standby( __in TCP_Client *_pSession );
	virtual bool IsExist_SessionPool_DisConnected_Active( 	__in TCP_Client *_pSession );
	virtual bool IsExist_SessionPool_DisConnected_Standby( 	__in TCP_Client *_pSession );

	virtual TCP_Client * Get_Send_Session();

	virtual bool Reconnect_DisConnect_Pool();

	virtual void Clear();
};

#endif