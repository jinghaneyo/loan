#ifndef SESSION_ROUND_ROBIN_H_
#define SESSION_ROUND_ROBIN_H_

#include <mutex>
#include <deque>
#include "Session_Pool.hpp"

class Session_RoundRobin : public Session_Pool
{
public:
	Session_RoundRobin( __in DATA_POLICY *_pPolicy, __in boost::asio::io_service *_pio_service );
	virtual ~Session_RoundRobin();
	// Session_RoundRobin( __in Session_FailOver &r) delete;
private:
	std::mutex									m_mtxSessionPool_Connect;
	std::deque<TCP_Client *>  					m_DqSessionPool_Connect;
	std::mutex									m_mtxSessionPool_DisConnect;
	std::deque<TCP_Client *>  					m_DqSessionPool_DisConnect;

	boost::asio::io_service 					*m_pio_service;
public:
	virtual bool Add_SessionPool_Connected( __in TCP_Client *_pSession, __in int _nOption = -1 );
	virtual bool Add_SessionPool_DisConnected( __in TCP_Client *_pSession, __in int _nOption = -1 );
	virtual bool Del_SessionPool_Connected( __in TCP_Client *_pSession, __in int _nOption = -1 );
	virtual bool Del_SessionPool_DisConnected( __in TCP_Client *_pSession, __in int _nOption = -1 );

	virtual bool IsExist_SessionPool_Connected( __in TCP_Client *_pSession,
												__in const std::lock_guard<std::mutex> *_pGaurd = nullptr );
	virtual bool IsExist_SessionPool_DisConnected( 	__in TCP_Client *_pSession,
													__in const std::lock_guard<std::mutex> *_pGaurd = nullptr );

	virtual TCP_Client * Get_Send_Session();

	virtual bool Reconnect_DisConnect_Pool();

	size_t GetSize_Connected()		{	return m_DqSessionPool_Connect.size();		};
	size_t GetSize_DisConnected()	{	return m_DqSessionPool_DisConnect.size();	};

	virtual void Clear();
};

#endif