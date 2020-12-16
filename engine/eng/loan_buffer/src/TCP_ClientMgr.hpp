#ifndef TCP_CLIENT_MGR_H_
#define TCP_CLIENT_MGR_H_

#include <mutex>
#include "Rigi_ClientTCP.hpp"
#include "MsgLog_Q.hpp"

typedef std::string 	STR_IP_PORT;

class TCP_ClientMgr: public Rigitaeda::Rigi_ClientTCP
{
public:
	TCP_ClientMgr();
	virtual ~TCP_ClientMgr();
private:
	boost::asio::io_service m_io_service;

	std::mutex											m_mtxSession;
	std::map<STR_IP_PORT, Rigitaeda::Rigi_ClientTCP *>  m_mapSessionPool;

	MsgLog_Q	*m_pLogQ;
public:
	// // ---------------------------------------------------------------
	// // 이벤트 함수
	// virtual bool OnEvent_Init();
	// virtual void OnEvent_Receive(	__in char *_pData,
	// 								__in size_t _nData_len );
	// // ---------------------------------------------------------------

	void Async_Run();
	void Await_Run();
	Rigitaeda::Rigi_ClientTCP * Connect_Session( __in const char *_pszServerIP, __in int _nPort );
	Rigitaeda::Rigi_ClientTCP * Get_Session( __in const char *_pszServerIP, __in int _nPort );

	// 전달할 분석 엔진 추가
	bool Add_Analyzer_Eng( __in const char *_pszServerIP, __in int _nPort );
	// 전달할 분석 엔진 삭제
	bool Del_Analyzer_Eng( __in const char *_pszServerIP, __in int _nPort );
	// 전달할 분석 엔진 변경
	bool Chg_Analyzer_Eng( 	__in const char *_pszServerIP_Old, __in int _nPort_Old,
							__in const char *_pszServerIP_New, __in int _nPort_New );

	void Set_LogQ( __in MsgLog_Q *_pLogQ );
};

#endif