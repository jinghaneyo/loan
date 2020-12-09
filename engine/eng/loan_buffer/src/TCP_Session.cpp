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
	DATA_PACKET *pData = new DATA_PACKET();

	Task_Filter(*pData);

	TCP_Mgr<TCP_Session> *pMgr = (TCP_Mgr<TCP_Session> *)Get_TCPMgr();
	if( nullptr != pMgr )
	{
		// Push_Data 안에 delete를 호출 해준다 
		pMgr->Push_Data( m_strIP_Port.c_str(), pData );
	}

	// 응답 주기
	std::string strPacket = "[{'cmd':'run'},{'data': 'test pakcet'}]";
	Send( strPacket.c_str(), strPacket.length() );
}

void TCP_Session::OnEvent_Close()
{
	std::cout << "[TCP_Session::OnClose] >> " << GetIP_Remote() << std::endl;
}

void TCP_Session::Task_Filter( __inout DATA_PACKET &_oData )
{

}

void TCP_Session::OnEvent_Init()
{
	m_strIP_Port = Get_SessionIP();
	m_strIP_Port += ":";
	m_strIP_Port += std::to_string( GetPort() );
}