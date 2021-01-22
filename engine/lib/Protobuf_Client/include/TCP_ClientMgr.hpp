#ifndef TCP_CLIENT_MGR_H_
#define TCP_CLIENT_MGR_H_

#include <mutex>
#include <deque>
#include <thread>
#include "Data_Policy.hpp"
#include "MsgLog_Q.hpp"
#include "Session_RoundRobin.hpp"
#include "Session_FailOver.hpp"
#include "Session_FailBack.hpp"
#include "TCP_Client.hpp"
#include <functional>
#include <fstream>

typedef std::function<bool( __in loan::MsgLog &_Packet )>  Callback_Filter;

enum class SEND_RULE
{
	NONE,
	ROUND_ROBIN,
	FAIL_OVER,
	FAIL_BACK,
	SAVE_FILE,
	MAX
};

class TCP_ClientMgr
{
public:
	TCP_ClientMgr( __in MsgLog_Q *_pLogQ, __in DATA_POLICY *_pPolicy );
	virtual ~TCP_ClientMgr();
private:
	boost::asio::io_service m_io_service;

	Session_Pool 	*m_pSendSession;
	std::ofstream	m_of_Data;

	MsgLog_Q	*m_pLogQ;
	DATA_POLICY *m_pPolicy;

	bool		m_bRun_Thread;
	std::thread	m_thr_conn;
	std::thread	m_thr_send;

	Callback_Filter		m_Callback_Filter;

	SEND_RULE	m_eSend_Rule;
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

	bool SendPacket( __in std::string *_pData );

	// ---------------------------------------------------------------------
	// 라운드로빈 관련 
	bool Add_Eng_RoundRobin( __in const char *_pszServerIP, __in const char *_pszPort );
	// ---------------------------------------------------------------------

	// 전달할 분석 엔진 추가
	bool Add_Eng_FailOver_Active( __in const char *_pszServerIP, __in const char *_pszPort );
	bool Add_Eng_FailOver_Standby( __in const char *_pszServerIP, __in const char *_pszPort );

	bool Add_Eng_FailBack_Active( __in const char *_pszServerIP, __in const char *_pszPort );
	bool Add_Eng_FailBack_Standby( __in const char *_pszServerIP, __in const char *_pszPort );

	bool Set_SaveFile( __in const char *_pszFilePath );

	DATA_POLICY * Get_Policy()	{	return m_pPolicy;	};

	// bool Input_Filter( __in loan::MsgLog &_Packet );
	void Add_Handler_Filter( __in Callback_Filter &&_Func )
	{
		m_Callback_Filter = std::move(_Func);
	}
};

#endif