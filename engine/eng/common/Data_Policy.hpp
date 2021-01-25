#ifndef DATA_POLICY_H_
#define DATA_POLICY_H_

#include "Policy.hpp"

class DATA_POLICY : public POLICY 
{
public:
	DATA_POLICY() { };
	~DATA_POLICY() {};

	std::string 						m_strSavePath_Pattern;
	std::map<std::string, std::string>  m_mapLogService;

	std::vector<std::string> vec_regex;
	std::map<std::string, std::string> map_replace;
	std::vector<std::string> vec_service;
};

#endif