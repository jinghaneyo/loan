#ifndef TCP_CLIENT_MGR_H_
#define TCP_CLIENT_MGR_H_

#include <mutex>
#include <deque>
#include <thread>
#include "Data_Policy.hpp"
#include "Rigi_ClientTCP.hpp"
#include "MsgLog_Q.hpp"

typedef std::string 	STR_IP_PORT;
typedef std::vector<Rigitaeda::Rigi_ClientTCP *>	VEC_RIGI_SESSION;
typedef std::chrono::system_clock::time_point 		STD_TIME;

#define SESSION_ACTIVE		0
#define SESSION_STANDBY 	1	

class TCP_ClientMgr
{
public:
	TCP_ClientMgr( __in MsgLog_Q *_pLogQ, __in DATA_POLICY *_pPolicy );
	virtual ~TCP_ClientMgr();
private:
	boost::asio::io_service m_io_service;

	std::mutex									m_mtxSessionPool_DisConnect;
	std::deque<Rigitaeda::Rigi_ClientTCP *>  	m_DqSessionPool_DisConnect;

	std::mutex									m_mtxSessionPool_Connect;
	std::deque<Rigitaeda::Rigi_ClientTCP *>  	m_DqSessionPool_Connect;

	std::mutex									m_mtxSessionPool_Connect_Active;
	std::deque<Rigitaeda::Rigi_ClientTCP *>  	m_DqSessionPool_Connect_Active;
	std::mutex									m_mtxSessionPool_Connect_Standby;
	std::deque<Rigitaeda::Rigi_ClientTCP *>  	m_DqSessionPool_Connect_Standby;
	int											m_nFailOver_Active_Standby;

	MsgLog_Q	*m_pLogQ;
	DATA_POLICY *m_pPolicy;

	bool		m_bRun_Thread;
	std::thread	m_thr_conn;
	std::thread	m_thr_send;

	void Add_SessionPool_Connected( __in Rigitaeda::Rigi_ClientTCP *_pSession );
	void Del_SessionPool_Connected( __in std::string _strServerIP, __in std::string _strServerPort );
	void Add_SessionPool_DisConnected( __in Rigitaeda::Rigi_ClientTCP *_pSession );
	void Del_SessionPool_DisConnected( __in std::string _strServerIP, __in std::string _strServerPort );

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

	const Rigitaeda::Rigi_ClientTCP * Connect_Session( __in const char *_pszServerIP, __in int _nPort );

	Rigitaeda::Rigi_ClientTCP * GetSession_Round_Robin();
	Rigitaeda::Rigi_ClientTCP * GetSession_begin_Active_FailOver();
	Rigitaeda::Rigi_ClientTCP * GetSession_begin_Standby_FailOver();
	Rigitaeda::Rigi_ClientTCP * GetSession_begin_FailOver();

	// 전달할 분석 엔진 추가
	bool Add_Eng( __in const char *_pszServerIP, __in int _nPort );
	// 전달할 분석 엔진 삭제
	bool Del_Eng( __in const char *_pszServerIP, __in int _nPort );
	// 전달할 분석 엔진 변경
	bool Chg_Eng( 	__in const char *_pszServerIP_Old, __in int _nPort_Old,
					__in const char *_pszServerIP_New, __in int _nPort_New );

	bool SendPacket_Round_Robin( __in std::string *_pData );



	// 전달할 분석 엔진 추가
	bool Add_Eng_Failover_Active( __in const char *_pszServerIP, __in int _nPort );
	bool Add_Eng_Failover_Standby( __in const char *_pszServerIP, __in int _nPort );
	// 전달할 분석 엔진 삭제
	bool Del_Eng_Failover( __in const char *_pszServerIP, __in int _nPort );
	// 전달할 분석 엔진 변경
	bool Chg_Eng_Failover( 	__in const char *_pszServerIP_Old, __in int _nPort_Old,
					__in const char *_pszServerIP_New, __in int _nPort_New );

	bool Add_Eng_RoundRobin( __in const char *_pszServerIP, __in const char *_pszPort );
	bool Add_Eng_FailOver_Active( __in const char *_pszServerIP, __in const char *_pszPort );
	bool Add_Eng_FailOver_Standby( __in const char *_pszServerIP, __in const char *_pszPort );
	bool Add_Eng_FailBack_Active( __in const char *_pszServerIP, __in const char *_pszPort );
	bool Add_Eng_FailBack_Standby( __in const char *_pszServerIP, __in const char *_pszPort );

	bool SendPacket_Fail_Over( __in std::string *_pLog );

	void Check_Connect_Session_RoundRobin();
	void Check_Connect_Session_FailOver();
};

#endif