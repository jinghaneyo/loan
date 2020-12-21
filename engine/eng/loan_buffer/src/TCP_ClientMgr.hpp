#ifndef TCP_CLIENT_MGR_H_
#define TCP_CLIENT_MGR_H_

#include <mutex>
#include "Rigi_ClientTCP.hpp"
#include "MsgLog_Q.hpp"

typedef std::string 	STR_IP_PORT;
typedef std::vector<Rigitaeda::Rigi_ClientTCP *>	VEC_RIGI_SESSION;
typedef std::chrono::system_clock::time_point 		STD_TIME;
class TCP_ClientMgr
{
public:
	TCP_ClientMgr( __in MsgLog_Q *_pLogQ );
	virtual ~TCP_ClientMgr();
private:
	boost::asio::io_service m_io_service;

	std::mutex											m_mtxSession_Pool;
	std::map<STR_IP_PORT, Rigitaeda::Rigi_ClientTCP *>  m_mapSession_Pool;
	//std::map<Rigitaeda::Rigi_ClientTCP *, int>  		m_mapSession_Pool;
	std::mutex											m_mtxSession_Del;
	std::map<STD_TIME, VEC_RIGI_SESSION *>				m_mapSession_Del;

	MsgLog_Q	*m_pLogQ;
public:
	// // ---------------------------------------------------------------
	// // 이벤트 함수
	// virtual bool OnEvent_Init();
	// virtual void OnEvent_Receive(	__in char *_pData,
	// 								__in size_t _nData_len );
	void OnEvent_Close( __in Rigitaeda::Rigi_ClientTCP *_pSession );
	// // ---------------------------------------------------------------

	std::thread Run();

	Rigitaeda::Rigi_ClientTCP * Connect_Session( __in const char *_pszServerIP, __in int _nPort );
	// Rigitaeda::Rigi_ClientTCP * Get_Session( __in const char *_pszServerIP, __in int _nPort );
	Rigitaeda::Rigi_ClientTCP * Get_Session_From_Pool( __in int _nIndex );

	// 전달할 분석 엔진 추가
	bool Add_Eng( __in const char *_pszServerIP, __in int _nPort );
	// 전달할 분석 엔진 삭제
	bool Del_Eng( __in const char *_pszServerIP, __in int _nPort );
	// 전달할 분석 엔진 변경
	bool Chg_Eng( 	__in const char *_pszServerIP_Old, __in int _nPort_Old,
					__in const char *_pszServerIP_New, __in int _nPort_New );

	void Set_LogQ( __in MsgLog_Q *_pLogQ );

	bool SendPacket_Round_Robin( __inout int &_nIndex, __in std::string *_pData );
	void Add_DelSession( __in Rigitaeda::Rigi_ClientTCP *_pSession );

	void Check_Connect_Session();
};

#endif