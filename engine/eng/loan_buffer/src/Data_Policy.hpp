#ifndef DATA_POLICY_H_
#define DATA_POLICY_H_

#include <string>
#include <vector>
#include <map>
#include <deque>

struct ServerInfo
{
	std::string strProtocol = "tcp";
	std::string strPort = "3333";
};

typedef std::vector<std::string> 		VEC_IP_PORT;

#define INDEX_ACTIVE	0
#define INDEX_STANDBY	1	

class DATA_POLICY
{
public:
	DATA_POLICY() { Init(); };
	~DATA_POLICY() {};

	ServerInfo m_ServerInfo;

	VEC_IP_PORT							m_vecRoudRobin;
	std::map<std::string, VEC_IP_PORT>  m_mapFailOver_IP_Port;
	std::vector<int>					m_vecFailOver_Change_Limit;
	std::map<std::string, VEC_IP_PORT>  m_mapFailBack_IP_Port;
	std::vector<int>					m_vecFailBack_Change_Limit;
	int									m_period_retry_connect_time = 5;
	std::string							m_SendRule;

	std::vector<std::string> vec_regex;
	std::map<std::string, std::string> map_replace;
	std::vector<std::string> vec_service;

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