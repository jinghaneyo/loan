#ifndef _RIGI_SESSION_POOL_H_
#define _RIGI_SESSION_POOL_H_

#include "Rigi_Def.hpp"
#include "Rigi_TCPSession.hpp"
//#include "Rigi_UDPSession.hpp"

namespace Rigitaeda
{
	class Rigi_SessionPool 
	{
	public:
		Rigi_SessionPool();
		virtual ~Rigi_SessionPool();
	private:
		int m_nMaxClient;
		int m_nReceive_Packet_Size;

		std::map<Rigi_TCPSession *, Rigi_TCPSession *>  m_mapTCP;
		//std::map<Rigi_UDPSession *, Rigi_UDPSession *>  m_mapUDP;
	public:
		void Set_MaxClient( __in int _nMaxClient );
		void Set_Receive_Packet_Size( __in int _nReceive_Packet_Size );

		bool Add_Session( 	__in Rigi_TCPSession *_pSession,
							__in SOCKET_TCP *_pSocket );
		bool Close_Session( __in Rigi_TCPSession *_pSession );
		// bool Add_Session( __in Rigi_UDPSession *_pSession );
		// bool Close_Session( __in Rigi_UDPSession *_pSession );

		void Clear();
	};
};

#endif