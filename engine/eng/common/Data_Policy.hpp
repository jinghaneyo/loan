#ifndef DATA_POLICY_H_
#define DATA_POLICY_H_

#include "Policy.hpp"

typedef std::map<std::string, std::string>		MAP_LOG_CONFIG;
typedef std::vector<MAP_LOG_CONFIG>				VEC_LOG_CONFIG;

class DATA_POLICY : public POLICY 
{
public:
	DATA_POLICY() { };
	~DATA_POLICY() {};

	std::string 							m_strSavePath_Pattern;
	std::map<std::string, VEC_LOG_CONFIG>  	m_mapLogService;

	std::vector<std::string> vec_regex;
	std::map<std::string, std::string> map_replace;
	std::vector<std::string> vec_service;
};

#endif