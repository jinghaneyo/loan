#ifndef LOAN_POLICY_H_
#define LOAN_POLICY_H_

#include <string>
#include <vector>
#include <map>
#include <deque>

typedef std::string 				STR_IP_PORT;
typedef std::vector<STR_IP_PORT> 	VEC_IP_PORT;

struct ServerInfo
{
	std::string strProtocol = "tcp";
	std::string strPort = "3333";
};

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

	ServerInfo  						m_ServerInfo;

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

#endif