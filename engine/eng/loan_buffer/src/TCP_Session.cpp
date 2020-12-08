#include "TCP_Session.hpp"
#include "TCP_Mgr.hpp"

TCP_Session::TCP_Session() 
{ 
}

TCP_Session::~TCP_Session() 
{ 
}

void TCP_Session::OnEvent_Receive(	__in char *_pData,
									__in size_t _nData_len )
{
	Task_Filter();

	TCP_Mgr<TCP_Session> *pMgr = (TCP_Mgr<TCP_Session> *)Get_TCPMgr();
	if( nullptr != pMgr )
	{
		Data_Q data;
		pMgr->Push_Data(Get_SessionIP(), GetPort(), data);
	}

	// 응답 주기
	std::string strPacket = "[{'cmd':'run'},{'data': 'test pakcet'}]";
	Send( strPacket.c_str(), strPacket.length() );
}

void TCP_Session::OnEvent_Close()
{
	std::cout << "[TCP_Session::OnClose] >> " << GetIP_Remote() << std::endl;
}

void TCP_Session::Task_Filter()
{

}