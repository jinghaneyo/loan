#ifndef TCP_SESSION_H_
#define TCP_SESSION_H_

#include "Rigi_Server.hpp"

class TCP_Session : public Rigitaeda::Rigi_TCPSession
{
public:
	TCP_Session();
	virtual ~TCP_Session();

	void OnEvent_Receive(	__in char *_pData,
							__in size_t _nData_len );

	void OnEvent_Close();

	void Task_Filter();
};

#endif