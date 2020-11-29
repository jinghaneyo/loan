#include "Rigi_Server.hpp"

class TCP_Session : public Rigitaeda::Rigi_TCPSession
{
public:
	TCP_Session() { }
	virtual ~TCP_Session() { }

	void OnEvent_Receive(	__in char *_pData,
							__in size_t _nData_len )
	{
		std::cout << "[TCP_Session::OnReceive] >> " << _pData << std::endl;
	}

	void OnEvent_Close()
	{
		std::cout << "[TCP_Session::OnClose] >> " << GetIP_Remote() << std::endl;
	}
};