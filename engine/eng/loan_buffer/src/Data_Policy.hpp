#ifndef DATA_POLICY_H_
#define DATA_POLICY_H_

#include <string>
#include <vector>
#include <map>

struct DATA_POLICY
{
public:
	DATA_POLICY() {};
	~DATA_POLICY() {};

	std::vector<std::string> vec_ip_port;
	std::vector<std::string> vec_regex;
	std::map<std::string, std::string> map_replace;
	std::vector<std::string> vec_service;
};

typedef std::vector<DATA_PACKET *> VEC_DATA_PACKET_PTR;

#endif