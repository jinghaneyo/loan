#ifndef TCP_CLIENT_H_
#define TCP_CLIENT_H_

#include "Rigi_ClientTCP.hpp"

class TCP_Client : public Rigitaeda::Rigi_ClientTCP
{
public:
	TCP_Client( __in int _nReceive_Packet_Size = 1024 ) 
	{
		m_bActive = true;

		Make_Receive_Packet_Size( _nReceive_Packet_Size );
	}
	virtual ~TCP_Client(){};
private:
	bool m_bActive;
public:
	void Set_Session_Type( __in bool _bActive )
	{
		m_bActive = _bActive;
	}

	bool Get_Session_Type()
	{
		return m_bActive;
	}
};

#endif