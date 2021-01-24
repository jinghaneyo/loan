#ifndef SESSION_POOL_H_
#define SESSION_POOL_H_

#include "Rigi_ClientTCP.hpp"
#include "Policy.hpp"

#ifndef SESSION_ACTIVE
#define SESSION_ACTIVE		0
#endif
#ifndef SESSION_STANDBY	
#define SESSION_STANDBY 	1	
#endif

class Session_Pool
{
public:
	Session_Pool( __in POLICY *_pPolicy ):m_pPolicy(_pPolicy) 
	{
		m_pPolicy = _pPolicy;
	}
	virtual ~Session_Pool(){};
private:
	POLICY *m_pPolicy = nullptr;
public:
	virtual bool Add_SessionPool_Connected( __in Rigitaeda::Rigi_ClientTCP *_pSession, __in bool _bActive ) = 0;
	virtual bool Add_SessionPool_DisConnected( __in Rigitaeda::Rigi_ClientTCP *_pSession, __in bool _bActive ) = 0;
	virtual bool Del_SessionPool_Connected( __in Rigitaeda::Rigi_ClientTCP *_pSession, __in bool _bActive ) = 0;
	virtual bool Del_SessionPool_DisConnected( __in Rigitaeda::Rigi_ClientTCP *_pSession, __in bool _bActive ) = 0;

	// virtual bool IsExist_SessionPool_Connected( __in Rigitaeda::Rigi_ClientTCP *_pSession,
	// 											__in const std::lock_guard<std::mutex> &_lock ) = 0;
	// virtual bool IsExist_SessionPool_DisConnected( 	__in Rigitaeda::Rigi_ClientTCP *_pSession,
	// 												__in const std::lock_guard<std::mutex> &_lock ) = 0;

	virtual Rigitaeda::Rigi_ClientTCP * Get_Send_Session() = 0;

	virtual bool Reconnect_DisConnect_Pool() = 0;

	virtual void Clear() = 0;

	POLICY * Get_Policy()	{	return m_pPolicy;	};
};

#endif