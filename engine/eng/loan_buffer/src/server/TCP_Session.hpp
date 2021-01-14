#ifndef TCP_SESSION_H_
#define TCP_SESSION_H_

#include "loan.pb.h"
#include "Rigi_Server.hpp"
#include "../data/MsgLog_Q.hpp"
#include "../data/Data_Policy.hpp"

enum class MsgLog_Type : int
{
	CROLLING = 1,		// 크롤링 로그 수집
	ANALYZER,		// 로그 분석
	MAX
};

enum class MsgLog_Cmd_Crolling: int
{
	SENDLING = 1,		// 로그 전송 중
	STOP_REQU,		// 로그 전송 중지 요청 
	STOP_RESP,		// 로그 전송 중지 요청에 대한 답변
	START_REQU,		// 로그 전송 시작 요청 
	START_RESP,		// 로그 전송 시작 요청에 대한 답변
	MAX
};

class TCP_Session : public Rigitaeda::Rigi_TCPSession
{
public:
	TCP_Session();
	virtual ~TCP_Session();
private:
	std::string m_strIP_Port;

	MsgLog_Q 	*m_pLogQ;
	DATA_POLICY *m_pPolicy;
public:
	// ---------------------------------------------------------------
	// 이벤트 함수
	virtual bool OnEvent_Init();
	virtual void OnEvent_Receive(	__in char *_pData,
									__in size_t _nData_len );
	virtual void OnEvent_Close();
	// ---------------------------------------------------------------

	// 일반 함수
	bool Task_Filter( 	__in loan::MsgLog &_oPacket );
	bool Input_Filter( 	__in loan::MsgLog &_oPacket );

	void Set_LogQ( __in MsgLog_Q *_pLogQ );
};

#endif