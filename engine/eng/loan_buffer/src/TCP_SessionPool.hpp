#ifndef TCP_SESSION_POOL_H_
#define TCP_SESSION_POOL_H_

#include <mutex>
#include <deque>
#include "Rigi_ClientTCP.hpp"

class TCP_SessionPool
{
public:
	TCP_SessionPool() { };
	virtual ~TCP_SessionPool() {};
private:
	std::mutex									m_mtxSessionPool_Connect;
	std::deque<Rigitaeda::Rigi_ClientTCP *>  	m_DqSessionPool_Connect;
	std::mutex									m_mtxSessionPool_DisConnect;
	std::deque<Rigitaeda::Rigi_ClientTCP *>  	m_DqSessionPool_DisConnect;
public:
	bool Add_SessionPool_Connect( __in Rigitaeda::Rigi_ClientTCP *_pSession );
	bool Add_SessionPool_DisConnect( __in Rigitaeda::Rigi_ClientTCP *_pSession );
	bool Del_SessionPool_Connected( __in Rigitaeda::Rigi_ClientTCP *_pSession );
	bool Del_SessionPool_DisConnected( __in Rigitaeda::Rigi_ClientTCP *_pSession );

	bool IsExist_SessionPool_Connect( __in const char *_pszIP, __in const char *_pszPort );
	bool IsExist_SessionPool_DisConnect( __in const char *_pszIP, __in const char *_pszPort );

	Rigitaeda::Rigi_ClientTCP * GetSession();
};

#endif