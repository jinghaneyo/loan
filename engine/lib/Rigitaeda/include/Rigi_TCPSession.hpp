#ifndef _RIGI_TCP_SESSION_H_
#define _RIGI_TCP_SESSION_H_

#include "Rigi_Def.hpp"
#include "Rigi_Setting.hpp"

namespace Rigitaeda
{
	class Rigi_SessionPool;

	class Rigi_TCPSession
	{
	public:
		Rigi_TCPSession();
		virtual ~Rigi_TCPSession();

	private:
		std::array<char, MAX_MESSAGE_LEN> m_ReceiveBuffer;
		SOCKET_TCP *m_pSocket;
		Rigi_SessionPool *m_pSessionPool;
		std::string m_strIP_Client;
		void * m_pTCPMgr;

		void Handler_Receive( 	__in const boost::system::error_code& _error, 
								__in size_t _bytes_transferred);

		void Handler_Send( 	__in const boost::system::error_code& _error, 
							__in size_t _bytes_transferred);

		void Close( __in const boost::system::error_code& _error );

	public:
		void Init();

		// -----------------------------------------------------------
		// Event(콜백)
		virtual void OnEvent_Receive(	__in char *_pData,
										__in size_t _nData_len );

		virtual void OnEvent_Close(){};
		// -----------------------------------------------------------

		void Async_Receive();

		void Send( __in const char* _pData, __in size_t _nSize);
		void Async_Send( __in const char* _pData, __in size_t _nSize);

		SOCKET_TCP * GetSocket( )
		{
			return m_pSocket;
		}

		void SetSocket( __in SOCKET_TCP *_pSocket )
		{
			m_pSocket = _pSocket;
		}

		const char *GetIP_Remote();

		const char *GetPacket_Receive()
		{
			return m_ReceiveBuffer.data();
		}

		void SetSessionPool( __in Rigi_SessionPool *_pSessionPool );
		const Rigi_SessionPool * GetSessionPool();

		void Set_TCPMgr( __in void * _pMgr );
		virtual void * Get_TCPMgr();

		void Close();

		_OUTPUT_TYPE GetType_Output();
	};
}

#endif