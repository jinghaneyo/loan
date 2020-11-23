#ifndef _RIGI_SESSION_H_
#define _RIGI_SESSION_H_

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
class Rigi_Session
{
public:
	Rigi_Session( __in boost::asio::io_service& _io_service, 
				  __in boost::asio::ip::tcp::socket *_pSocket);

	virtual ~Rigi_Session();

private:
	std::array<char, MAX_MESSAGE_LEN> m_ReceiveBuffer;
	boost::asio::ip::tcp::socket *m_pSocket = nullptr;

	void Handler_Send( __in const boost::system::error_code& _error, 
						__in size_t _bytes_transferred);

	void Handler_Receive( __in const boost::system::error_code& _error, 
							__in size_t _bytes_transferred);

public:
	void Init();

	void PostReceive();

	void PostSend( __in const char* _pData, __in size_t _nSize);

	boost::asio::ip::tcp::socket *GetSocket()
	{
		return m_pSocket;
	}

	std::string && GetIP_Remote()
	{
		boost::asio::ip::tcp::endpoint remote_ep = m_pSocket->remote_endpoint();
		boost::asio::ip::address remote_ad = remote_ep.address();
		std::string s = remote_ad.to_string();

		return std::move(s);
	}

	void Close( __in const boost::system::error_code& _error );

	void Close();
};
}

#endif