#ifndef _RIGI_TCP_SESSION_H_
#define _RIGI_TCP_SESSION_H_

#include "Rigi_Def.hpp"

namespace Rigitaeda
{
	class Rigi_SessionPool;

	class Rigi_TCPSession
	{
	public:
		Rigi_TCPSession();
		virtual ~Rigi_TCPSession();

	private:
		// SessionPool 에서는 접근 하도록 하자
		friend class Rigi_SessionPool;

		int m_nReceive_Packet_Size;
		char *m_pReceive_Packet_Buffer;

		SOCKET_TCP *m_pSocket;
		Rigi_SessionPool *m_pSessionPool;
		std::string m_strIP_Client;
		void * m_pTCPMgr;

		void Handler_Receive( 	__in const boost::system::error_code& _error, 
								__in size_t _bytes_transferred);

		void Handler_Send( 	__in const boost::system::error_code& _error, 
							__in size_t _bytes_transferred);

		void Close( __in const boost::system::error_code& _error );

		inline void BufferClear()
		{
			memset(m_pReceive_Packet_Buffer, 0, m_nReceive_Packet_Size);
		}
	public:
		// -----------------------------------------------------------
		// Event(콜백)
		virtual void OnEvent_Receive(	__in char *_pData,
										__in size_t _nData_len );
		virtual void OnEvent_Sended ( 	__in size_t _bytes_transferre );

		virtual void OnEvent_Close(){ std::cout << "Rigi_TCPSession OnEvent_Close !!" << std::endl; };
		virtual bool OnEvent_Init()	{	return true;	};
		// -----------------------------------------------------------

        // 버퍼 크기는 new TCPSession 을 할때 해야 함으로 public 으로 아무때나 호출 할수 없게 private 으로 한다
		void Make_Receive_Packet_Size( __in int _nPacket_Buffer_Size );

		void Async_Receive();

		size_t Sync_Send ( __in const char* _pData, __in size_t _nSize);
		void   ASync_Send( __in const char* _pData, __in size_t _nSize);

		SOCKET_TCP * GetSocket( )
		{
			return m_pSocket;
		}

		void SetSocket( __in SOCKET_TCP *_pSocket )
		{
			m_pSocket = _pSocket;
		}

		const char *Get_SessionIP();

		const char *GetPacket_Receive()
		{
			return m_pReceive_Packet_Buffer;
		}

		void SetSessionPool( __in Rigi_SessionPool *_pSessionPool );
		const Rigi_SessionPool * GetSessionPool();

		void Set_TCPMgr( __in void * _pMgr );
		virtual void * Get_TCPMgr();

		void Close();
	};
}

#endif