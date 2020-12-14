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

typedef std::vector<DATA_PACKET *> VEC_DATA_PACKET_PTR;
typedef std::deque<DATA_PACKET *> DEQUE_DATA_PACKET_PTR;

#endif