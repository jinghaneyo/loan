#ifndef TCP_SESSION_H_
#define TCP_SESSION_H_

#include "Rigi_Server.hpp"
#include <functional>

typedef std::function<bool( __in std::string &_strProtobufRaw )> 	Event_Receive_Filter;

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

class MsgLog_Q;
class POLICY;

class TCP_Session : public Rigitaeda::Rigi_TCPSession
{
public:
	TCP_Session();
	virtual ~TCP_Session();
private:
	std::string m_strIP_Port;
	std::string m_strTempBuff;

	MsgLog_Q 	*m_pLogQ;
	POLICY *m_pPolicy;

	Event_Receive_Filter	m_Event_RecvFilter;
public:
	// ---------------------------------------------------------------
	// 이벤트 함수
	virtual bool OnEvent_Init();
	virtual void OnEvent_Receive(	__in char *_pData,
									__in size_t _nData_len );
	virtual void OnEvent_Close();
	// ---------------------------------------------------------------

	// 일반 함수
	void Set_LogQ( __in MsgLog_Q *_pLogQ );

	void Split_Protobuf(__in const char *_pszData, 
						__in size_t _nData_len,
						__out std::string &_strTemp );

	void AddEventHandler_Receive_Filter( __in Event_Receive_Filter &&_Func )
	{
		m_Event_RecvFilter = std::move(_Func);
	}
};

#endif