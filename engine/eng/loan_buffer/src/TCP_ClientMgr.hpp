#ifndef TCP_CLIENT_MGR_H_
#define TCP_CLIENT_MGR_H_

#include <mutex>
#include <deque>
#include <thread>
#include "Def.hpp"
#include "Data_Policy.hpp"
#include "Rigi_ClientTCP.hpp"
#include "MsgLog_Q.hpp"
#include "Session_RoundRobin.hpp"
#include "Session_FailOver.hpp"
#include "TCP_Client.hpp"

class TCP_ClientMgr
{
public:
	TCP_ClientMgr( __in MsgLog_Q *_pLogQ, __in DATA_POLICY *_pPolicy );
	virtual ~TCP_ClientMgr();
private:
	boost::asio::io_service m_io_service;

	Session_Pool *m_pSendSession;

	MsgLog_Q	*m_pLogQ;
	DATA_POLICY *m_pPolicy;

	bool		m_bRun_Thread;
	std::thread	m_thr_conn;
	std::thread	m_thr_send;

public:
	// // ---------------------------------------------------------------
	// // 이벤트 함수
	// virtual bool OnEvent_Init();
	void OnEvent_Close( __in Rigitaeda::Rigi_ClientTCP *_pSession );
	// // ---------------------------------------------------------------

	void Run();
	void Stop();

	void Clear_Eng();

	void Set_LogQ( __in MsgLog_Q *_pLogQ );

	// ---------------------------------------------------------------------
	// 라운드로빈 관련 
	bool Add_Eng_RoundRobin( __in const char *_pszServerIP, __in const char *_pszPort );
	bool SendPacket_Round_Robin( __in std::string *_pData );
	// ---------------------------------------------------------------------

	// 전달할 분석 엔진 추가
	bool Add_Eng_FailOver_Active( __in const char *_pszServerIP, __in const char *_pszPort );
	bool Add_Eng_FailOver_Standby( __in const char *_pszServerIP, __in const char *_pszPort );
	bool SendPacket_FailOver( __in std::string *_pLog );

	bool Add_Eng_FailBack_Active( __in const char *_pszServerIP, __in const char *_pszPort );
	bool Add_Eng_FailBack_Standby( __in const char *_pszServerIP, __in const char *_pszPort );
	bool SendPacket_FailBack( __in std::string *_pLog );
};

#endif