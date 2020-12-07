#ifndef _RIGI_SESSION_POOL_H_
#define _RIGI_SESSION_POOL_H_

#include "Rigi_Def.hpp"
#include "Rigi_Setting.hpp"
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

		std::map<Rigi_TCPSession *, Rigi_TCPSession *>  m_mapTCP;
		//std::map<Rigi_UDPSession *, Rigi_UDPSession *>  m_mapUDP;

		_OUTPUT_TYPE m_eTypeOutput;
		Rigi_Filter m_Setting_Filter;
		Rigi_Output m_Setting_Output;
	public:
		void Set_MaxClient( __in int _nMaxClient );
		bool Add_Session( 	__in Rigi_TCPSession *_pSession,
							__in SOCKET_TCP *_pSocket );
		bool Close_Session( __in Rigi_TCPSession *_pSession );
		// bool Add_Session( __in Rigi_UDPSession *_pSession );
		// bool Close_Session( __in Rigi_UDPSession *_pSession );

		void Load_Conf( __in const char *_pszPath_conf );
		void Set_SessionPool();

		void Clear();

		_OUTPUT_TYPE GetType_Output()
		{
			return m_eTypeOutput;
		}
		const Rigi_Filter * Get_Setting_Filter()
		{
			return &m_Setting_Filter;
		}
		const Rigi_Output * Get_Setting_Output()
		{
			return &m_Setting_Output;
		}
	};
};

#endif