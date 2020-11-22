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

class Rigi_Session
{
public:
	Rigi_Session( __in boost::asio::io_service& _io_service, 
				  __in boost::asio::ip::tcp::socket *_pSocket);

	virtual ~Rigi_Session();

private:
	//std::array<char, MAX_MESSAGE_LEN> m_ReceiveBuffer;
	char m_ReceiveBuffer[MAX_MESSAGE_LEN] = {0,};
	boost::asio::ip::tcp::socket *m_pSocket = nullptr;

	bool m_bIsAbleSocket = false;

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

	void Close( __in const boost::system::error_code& _error );

	void Close();

	bool IsUseAbleSocket()
	{
		return m_bIsAbleSocket;
	}

	void SetUsesAbleSocket( __in bool _bAble )
	{
		m_bIsAbleSocket = _bAble;
	}
};

#endif