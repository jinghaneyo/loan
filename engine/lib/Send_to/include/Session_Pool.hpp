#ifndef SESSION_POOL_H_
#define SESSION_POOL_H_

#include "Rigi_ClientTCP.hpp"

typedef std::string 				STR_IP_PORT;
typedef std::vector<STR_IP_PORT> 	VEC_IP_PORT;

#define INDEX_ACTIVE	0
#define INDEX_STANDBY	1	

#ifndef SESSION_ACTIVE
#define SESSION_ACTIVE		0
#endif
#ifndef SESSION_STANDBY	
#define SESSION_STANDBY 	1	
#endif

class POLICY
{
public:
	POLICY() { Init(); };
	~POLICY() {};

	VEC_IP_PORT							m_vecRoudRobin;
	std::map<std::string, VEC_IP_PORT>  m_mapFailOver_IP_Port;
	std::vector<int>					m_vecFailOver_Change_Limit;
	std::map<std::string, VEC_IP_PORT>  m_mapFailBack_IP_Port;
	std::vector<int>					m_vecFailBack_Change_Limit;
	int									m_period_retry_connect_time = 5;
	std::string							m_SendRule;

	void Init()
	{
		m_vecRoudRobin.clear();
		m_mapFailOver_IP_Port.clear();
		m_mapFailBack_IP_Port.clear();
		// active, stand-by 기본값 입력
		m_vecFailOver_Change_Limit.emplace_back(0);
		m_vecFailOver_Change_Limit.emplace_back(0);
		// active, stand-by 기본값 입력
		m_vecFailBack_Change_Limit.emplace_back(0);
		m_vecFailBack_Change_Limit.emplace_back(0);
	}
};

class Session_Pool
{
public:
	Session_Pool( __in POLICY *_pPolicy ):m_pPolicy(_pPolicy) 
	{
		m_pPolicy = _pPolicy;
	}
	virtual ~Session_Pool(){};
private:
	POLICY *m_pPolicy = nullptr;
public:
	virtual bool Add_SessionPool_Connected( __in Rigitaeda::Rigi_ClientTCP *_pSession, __in bool _bActive ) = 0;
	virtual bool Add_SessionPool_DisConnected( __in Rigitaeda::Rigi_ClientTCP *_pSession, __in bool _bActive ) = 0;
	virtual bool Del_SessionPool_Connected( __in Rigitaeda::Rigi_ClientTCP *_pSession, __in bool _bActive ) = 0;
	virtual bool Del_SessionPool_DisConnected( __in Rigitaeda::Rigi_ClientTCP *_pSession, __in bool _bActive ) = 0;

	// virtual bool IsExist_SessionPool_Connected( __in Rigitaeda::Rigi_ClientTCP *_pSession,
	// 											__in const std::lock_guard<std::mutex> &_lock ) = 0;
	// virtual bool IsExist_SessionPool_DisConnected( 	__in Rigitaeda::Rigi_ClientTCP *_pSession,
	// 												__in const std::lock_guard<std::mutex> &_lock ) = 0;

	virtual Rigitaeda::Rigi_ClientTCP * Get_Send_Session() = 0;

	virtual bool Reconnect_DisConnect_Pool() = 0;

	virtual void Clear() = 0;

	POLICY * Get_Policy()	{	return m_pPolicy;	};
};

#endif