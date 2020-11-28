﻿#ifndef DEF_COMMON_H_
#define DEF_COMMON_H_

#ifndef MAX_MESSAGE_LEN
#define MAX_MESSAGE_LEN		1024	
#endif

#ifndef VER_MAJOR
#define VER_MAJOR			"1.0.0"
#endif

#ifndef VER_MINOR
#define VER_MINOR			"20.11.19"
#endif

#include <iostream>
#include <algorithm>
#include <string>
#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include <boost/system/error_code.hpp>
#include <glog/logging.h>

#ifndef _WIN32

	#ifndef __in
	#define __in
	#endif

	#ifndef __inout
	#define __inout
	#endif

	#ifndef __out
	#define __out
	#endif

#endif

#ifndef DEF_PTOTOCOL
#define DEF_PTOTOCOL
enum class PROTOCOL
{
	TCP,
	UDP,
	MAX
};
#endif

typedef boost::asio::ip::tcp::socket 	SOCKET_TCP;
typedef boost::asio::ip::udp::socket 	SOCKET_UDP;

typedef std::function<void(SOCKET_TCP *, char *pData, size_t nSize)> Event_Received_TCP;
typedef std::function<void(SOCKET_UDP *, char *pData, size_t nSize)> Event_Received_UDP;
//typedef std::function<void(std::vector<SOCKET_PTR> *pSocketList, SOCKET_PTR, char *pData, size_t nSize)> Event_Received;

#endif