#include "Rigi_Server.hpp"
#include "Rigi_Setting.hpp"

using namespace Rigitaeda;
class TCP_Session : public Rigitaeda::Rigi_TCPSession
{
public:
	TCP_Session() 
	{ 
	}
	virtual ~TCP_Session() { }

	void OnEvent_Receive(	__in char *_pData,
							__in size_t _nData_len )
	{
		std::cout << "[TCP_Session::Receive] >> " << _pData << std::endl;

		// 응답 주기
		std::string strPacket = "[{'cmd':'run'},{'data': 'test pakcet'}]";
		Send( strPacket.c_str(), strPacket.length() );
	}

	void OnEvent_Close()
	{
		std::cout << "[TCP_Session::OnClose] >> " << GetIP_Remote() << std::endl;
	}
};