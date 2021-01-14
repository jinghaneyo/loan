#ifndef SESSION_POOL_H_
#define SESSION_POOL_H_

#include "TCP_Client.hpp"
#include "../data/Data_Policy.hpp"

class Session_Pool
{
public:
	Session_Pool( __in DATA_POLICY *_pPolicy ):m_pPolicy(_pPolicy) 
	{
		m_pPolicy = _pPolicy;
	}
	virtual ~Session_Pool(){};
private:
	DATA_POLICY *m_pPolicy = nullptr;
public:
	virtual bool Add_SessionPool_Connected( __in TCP_Client *_pSession, __in int _nOption = -1 ) = 0;
	virtual bool Add_SessionPool_DisConnected( __in TCP_Client *_pSession, __in int _nOption = -1 ) = 0;
	virtual bool Del_SessionPool_Connected( __in TCP_Client *_pSession, __in int _nOption = -1 ) = 0;
	virtual bool Del_SessionPool_DisConnected( __in TCP_Client *_pSession, __in int _nOption = -1 ) = 0;

	// virtual bool IsExist_SessionPool_Connected( __in Rigitaeda::Rigi_ClientTCP *_pSession,
	// 											__in const std::lock_guard<std::mutex> &_lock ) = 0;
	// virtual bool IsExist_SessionPool_DisConnected( 	__in Rigitaeda::Rigi_ClientTCP *_pSession,
	// 												__in const std::lock_guard<std::mutex> &_lock ) = 0;

	virtual TCP_Client * Get_Send_Session() = 0;

	virtual bool Reconnect_DisConnect_Pool() = 0;

	virtual void Clear() = 0;

	DATA_POLICY * Get_Policy()	{	return m_pPolicy;	};
};

#endif