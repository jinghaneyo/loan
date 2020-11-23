#ifndef _RIGI_PROTOCOL_H_
#define _RIGI_PROTOCOL_H_

#include <iostream>
#include <algorithm>
#include <string>
#include <array>
#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include <boost/system/error_code.hpp>

#include "Rigi_Def.hpp"

namespace Rigitaeda
{
	enum class PROTOCOL
	{
		TCP,
		UDP,
		MAX
	};

	class Rigi_Socket
	{
	public:
		Rigi_Socket( __in boost::asio::ip::tcp::socket *_pSocket );
		Rigi_Socket( __in boost::asio::ip::udp::socket *_pSocket );
		~Rigi_Socket();

	private:
		boost::asio::ip::tcp::socket *	m_pSockTCP = nullptr;
		boost::asio::ip::udp::socket *	m_pSockUDP = nullptr;
		PROTOCOL						m_eProtocol = PROTOCOL::TCP;
	};
}

#endif