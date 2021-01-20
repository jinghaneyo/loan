#ifndef DATA_POLICY_H_
#define DATA_POLICY_H_

#include <string>
#include <vector>
#include <map>
#include <deque>
#include "Session_Pool.hpp"

struct ServerInfo
{
	std::string strProtocol = "tcp";
	std::string strPort = "3333";
};

class DATA_POLICY : public POLICY
{
public:
	DATA_POLICY() { Init(); };
	~DATA_POLICY() {};

	ServerInfo m_ServerInfo;

	std::vector<std::string> vec_regex;
	std::map<std::string, std::string> map_replace;
	std::vector<std::string> vec_service;

	void Init()
	{
		//POLICY::Clear();
	}
};

#endif