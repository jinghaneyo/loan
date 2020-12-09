#ifndef DATA_PACKET_H_
#define DATA_PACKET_H_

#include <string>
#include <vector>
#include <map>

struct DATA_PACKET
{
public:
	DATA_PACKET() {};
	~DATA_PACKET() {};

	std::vector<std::string> vec_service;
	std::string strData;
};

#endif