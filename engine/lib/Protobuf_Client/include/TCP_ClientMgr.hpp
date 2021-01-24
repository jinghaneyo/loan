#ifndef TCP_CLIENT_MGR_H_
#define TCP_CLIENT_MGR_H_

#include <mutex>
#include <deque>
#include <thread>
#include <functional>
#include <fstream>

#include "Rigi_ClientTCP.hpp"

typedef std::function<bool( __in std::string &_strProtobufRaw )> 	Event_Send_Filter;

enum class SEND_RULE
{
	NONE,
	ROUND_ROBIN,
	FAIL_OVER,
	FAIL_BACK,
	SAVE_FILE,
	MAX
};

class MsgLog_Q;
class POLICY;
class Session_Pool;

class TCP_ClientMgr
{
public:
	TCP_ClientMgr( __in MsgLog_Q *_pLogQ, __in POLICY *_pPolicy );
	virtual ~TCP_ClientMgr();
private:
	boost::asio::io_service m_io_service;

	Session_Pool 	*m_pSendSession;
	std::ofstream	m_of_Data;

	MsgLog_Q	*m_pLogQ;
	POLICY *m_pPolicy;

	bool		m_bRun_Thread;
	std::thread	m_thr_conn;
	std::thread	m_thr_send;

	Event_Send_Filter	m_Event_Send_Filter;

	SEND_RULE			m_eSend_Rule;

	std::string			m_strSavePath_Pattern;
	std::string			m_strLocale;
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

	bool SendPacket( __in std::string *_pstrProtobufRaw );

	// ---------------------------------------------------------------------
	// 라운드로빈 관련 
	bool Add_Eng_RoundRobin( __in const char *_pszServerIP, __in const char *_pszPort );
	// ---------------------------------------------------------------------

	// 전달할 분석 엔진 추가
	bool Add_Eng_FailOver_Active( __in const char *_pszServerIP, __in const char *_pszPort );
	bool Add_Eng_FailOver_Standby( __in const char *_pszServerIP, __in const char *_pszPort );

	bool Add_Eng_FailBack_Active( __in const char *_pszServerIP, __in const char *_pszPort );
	bool Add_Eng_FailBack_Standby( __in const char *_pszServerIP, __in const char *_pszPort );

	bool Set_SaveFile( __in const char *_pszFilePath, __in const char *_pszLocale = "ko_KR.UTF-8" );
	bool OpenFile( __in const char *_pszFilePath, __in const char *_pszLocale );
	bool Write_Data( __in std::string *_pstrProtobufRaw );

	POLICY * Get_Policy();

	void AddEventHandler_Send_Filter( __in Event_Send_Filter &&_Func )
	{
		m_Event_Send_Filter = std::move(_Func);
	}
};

#endif