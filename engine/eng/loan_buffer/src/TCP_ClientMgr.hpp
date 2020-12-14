#ifndef TCP_CLIENT_MGR_H_
#define TCP_CLIENT_MGR_H_

#include "Rigi_ClientTCP.hpp"

typedef std::string 	STR_IP_PORT;
class TCP_ClientMgr: public Rigitaeda::Rigi_ClientTCP
{
public:
	TCP_ClientMgr();
	virtual ~TCP_ClientMgr();
private:
	boost::asio::io_service m_io_service;
	int		m_nPort;

	std::map<STR_IP_PORT, Rigitaeda::Rigi_ClientTCP *> m_mapSessionPool;
public:
	// // ---------------------------------------------------------------
	// // 이벤트 함수
	// virtual bool OnEvent_Init();
	// virtual void OnEvent_Receive(	__in char *_pData,
	// 								__in size_t _nData_len );
	// // ---------------------------------------------------------------

	void Async_Run( __in char *_pszServerIP, __in int _nPort );
	void Await_Run( __in char *_pszServerIP, __in int _nPort );
	Rigitaeda::Rigi_ClientTCP * Connect_Session( __in char *_pszServerIP, __in int _nPort );

	// void OnEvent_Close();

	// void Task_Filter();
};

#endif