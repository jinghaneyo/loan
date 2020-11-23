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
#include "Rigi_Socket.hpp"

namespace Rigitaeda
{
	class Rigi_Session
	{
	public:
		Rigi_Session( 	__in boost::asio::io_service& _io_service, 
						__in boost::asio::ip::tcp::socket *_pSocket);

		Rigi_Session( 	__in boost::asio::io_service& _io_service, 
						__in boost::asio::ip::udp::socket *_pSocket);

		virtual ~Rigi_Session();

	private:
		std::array<char, MAX_MESSAGE_LEN> m_ReceiveBuffer;
		boost::asio::ip::tcp::socket *	m_pSockTCP = nullptr;
		boost::asio::ip::udp::socket *	m_pSockUDP = nullptr;
		//Rigi_Socket *m_pSocket = nullptr;
		PROTOCOL	 m_eProtocol = PROTOCOL::TCP;

		void Handler_Send( 	__in const boost::system::error_code& _error, 
							__in size_t _bytes_transferred);

		void Handler_Receive( 	__in const boost::system::error_code& _error, 
								__in size_t _bytes_transferred);

	public:
		void Init();

		void PostReceive();

		void PostSend( __in const char* _pData, __in size_t _nSize);

		PROTOCOL GetProtocol()
		{
			return m_eProtocol;
		}

		boost::asio::ip::tcp::socket *GetSocket( )
		{
			return m_pSockTCP;
		}

		std::string && GetIP_Remote()
		{
			std::string strIP = "0.0.0.0";

			//if( PROTOCOL::TCP ==  m_eProtocol )
			{
				boost::asio::ip::tcp::endpoint remote_ep = m_pSockTCP->remote_endpoint();
				boost::asio::ip::address remote_ad = remote_ep.address();
				strIP = remote_ad.to_string();
			}
			// else
			// {
			// 	boost::asio::ip::udp::endpoint remote_ep = m_pSockUDP->remote_endpoint();
			// 	boost::asio::ip::address remote_ad = remote_ep.address();
			// 	strIP = remote_ad.to_string();
			// }

			return std::move(strIP);
		}

		void Close( __in const boost::system::error_code& _error );

		void Close();
	};
}

#endif