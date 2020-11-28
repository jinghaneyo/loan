#ifndef _RIGI_TCP_SESSION_H_
#define _RIGI_TCP_SESSION_H_

#include "Rigi_Def.hpp"

namespace Rigitaeda
{
	class Rigi_TCPSession
	{
	public:
		Rigi_TCPSession( 	__in boost::asio::io_service& _io_service, 
							__in SOCKET_TCP *_pSocket);
		virtual ~Rigi_TCPSession();

	private:
		std::array<char, MAX_MESSAGE_LEN> m_ReceiveBuffer;
		SOCKET_TCP *m_pSocket;
		std::string m_strIP_Client;

		Event_Received_TCP m_Event_Receive;

		void Handler_Send( 	__in const boost::system::error_code& _error, 
							__in size_t _bytes_transferred);

		void Handler_Receive( 	__in const boost::system::error_code& _error, 
								__in size_t _bytes_transferred);

	public:
		void Init();

		void SetEvent_Receive( __in Event_Received_TCP &&_Event );

		void Async_Receive();

		void PostSend( __in const char* _pData, __in size_t _nSize);

		SOCKET_TCP * GetSocket( )
		{
			return m_pSocket;
		}

		std::string && GetIP_Remote();

		void Close( __in const boost::system::error_code& _error );

		void Close();
	};
}

#endif