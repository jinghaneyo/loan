#ifndef _RIGI_CLIENT_TCP_H_
#define _RIGI_CLIENT_TCP_H_

#include "Rigi_Def.hpp"

namespace Rigitaeda
{
	class Rigi_ClientTCP
	{
	public:
		Rigi_ClientTCP()
		{
		}
		
		virtual ~Rigi_ClientTCP()
		{
			if(nullptr != m_pSocket)
			{
				m_pSocket->close();
				delete m_pSocket;
			}
			m_pSocket = nullptr;
		}
	private:
		SOCKET_TCP  *m_pSocket = nullptr;
		bool		m_bConnected = false;

		int Receive()
		{
			//while(true)
			{
				if(false == m_bConnected)
					return -1;

				char szBuffer[10240] = {0,};
				// 버퍼를 이용해 서버로부터 데이터를 받아옵니다.
				boost::system::error_code error;
				size_t len = m_pSocket->read_some(boost::asio::buffer(szBuffer, sizeof(szBuffer)), error);
				if (error != boost::asio::error::eof)
				{
					std::cout << "[Rigi_ClientTCP::Receive] >> " << szBuffer << std::endl;
					OnEvent_Receive( szBuffer, (int)len );
				}
				else
				{
					std::cout << "[Rigi_ClientTCP::Receive][ERROR]" << std::endl;
				}
			}
		}
	public:
		bool Connect( 	__in const char *_pszHost, 
						__in int _nPort,
						__in boost::asio::io_service &io_service )
		{
			boost::asio::ip::tcp::resolver resolver(io_service);
			boost::asio::ip::tcp::resolver::query query( _pszHost, std::to_string(_nPort) );
			boost::asio::ip::tcp::resolver::iterator endpoint_iterator = resolver.resolve(query);

			m_pSocket = new SOCKET_TCP(io_service);

			boost::system::error_code ec;
			boost::asio::connect(*m_pSocket, endpoint_iterator, ec);
			if (ec)
			{	
				m_bConnected = false;
				delete m_pSocket;
				m_pSocket = nullptr;

				std::cout << "CONNECT >> FAIL" << std::endl;
				return false;
			}
			else
			{	
				m_bConnected = true;
				std::cout << "CONNECT >> SUCC" << std::endl;
				return true;
			}
		}

		int SendPacket( __in const char *_pszData, __in int _nDataLen )
		{
			if(false == m_bConnected)
				return -1;

			return m_pSocket->write_some( boost::asio::buffer(_pszData, _nDataLen) );
		}

		virtual void OnEvent_Receive( __in char *_pszData, __in int _nData_len ){};
	};
};

#endif