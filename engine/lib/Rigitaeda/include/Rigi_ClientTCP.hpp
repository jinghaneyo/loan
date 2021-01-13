#ifndef _RIGI_CLIENT_TCP_H_
#define _RIGI_CLIENT_TCP_H_

#include "Rigi_Def.hpp"
#include "Rigi_TCPSession.hpp"
#include <boost/asio/steady_timer.hpp>
#include <functional>

namespace Rigitaeda
{
	typedef std::function<void( __in Rigi_TCPSession *)> Callback_Event_Close;

	class Rigi_ClientTCP : public Rigi_TCPSession
	{
	public:
		Rigi_ClientTCP( __in int _nReceive_Packet_Size = 1024 )
		{
			m_bConnected = false;

			Make_Receive_Packet_Size(_nReceive_Packet_Size);
		}
		
		virtual ~Rigi_ClientTCP()
		{
			Rigi_TCPSession::Close();
			m_bConnected = false;
		}
	private:
		bool		m_bConnected;
		std::string m_strServerIP;
		std::string m_strServerPort;
	public:
		bool Connect( 	__in const char *_pszHost, 
						__in const char *_pszPort,
						__in boost::asio::io_service &io_service )
		{
			boost::asio::ip::tcp::resolver resolver(io_service);
			boost::asio::ip::tcp::resolver::query query( _pszHost, _pszPort );
			boost::asio::ip::tcp::resolver::iterator endpoint_iterator = resolver.resolve(query);

			SetSocket( new SOCKET_TCP(io_service) );

			boost::system::error_code ec;
			boost::asio::connect( *GetSocket(), endpoint_iterator, ec );
			if (ec)
			{	
				m_bConnected = false;

				Close();

				std::cout << "CONNECT >> FAIL" << std::endl;
				return false;
			}
			else
			{	
				m_bConnected = true;

				m_strServerIP 	= _pszHost;
				m_strServerPort = _pszPort;

				std::cout << "CONNECT >> SUCC" << std::endl;

				Async_Receive();

				return true;
			}

			return true;
		}

		bool Connect( 	__in const char *_pszHost, 
						__in int _nPort, 
						__in boost::asio::io_service &io_service )
		{
			m_strServerIP = _pszHost;
			m_strServerPort = std::to_string(_nPort);

			if( true == Connect( _pszHost, std::to_string(_nPort).c_str(), io_service ) )
				return true;
			else
				return false;
		}

		bool Reconnect( __in boost::asio::io_service &io_service )
		{
			try
			{
				Close();

				// 원래 소켓은 세션풀에서 삭제를 하도록 되어 있는데 현재 세션풀을 사용하지 않으니 직접 삭제해 준다
				if(nullptr != GetSocket())
				{
					delete GetSocket();
					SetSocket(nullptr);
				}

				//std::cout << "[Reconnect] << IP  =" << m_strServerIP << " | Port = " << m_strServerPort <<std::endl;
				if( true == Connect( m_strServerIP.c_str(), m_strServerPort.c_str(), io_service ) )
					return true;
				else
					return false;
			}
			catch( std::exception &e)
			{
				std::cout << "[Exception][Reconnect] << " << e.what() <<std::endl;
			}
			return false;
		}

		bool IsConnected()
		{
			return m_bConnected;
		}

		std::string & Get_ServerIP()
		{
			return m_strServerIP;
		}

		std::string & Get_Port()
		{
			return m_strServerPort;
		}
	};
};

#endif