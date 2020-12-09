#include "TCP_Session.hpp"
#include "TCP_Mgr.hpp"
#include <regex>

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

	// 응답 주기
	std::string strPacket = "[{'cmd':'run'},{'data': 'test pakcet'}]";
	Send( strPacket.c_str(), strPacket.length() );
}

void TCP_Session::OnEvent_Close()
{
	std::cout << "[TCP_Session::OnClose] >> " << Get_SessionIP() << std::endl;
}

void TCP_Session::Task_Filter()
{
	TCP_Mgr<TCP_Session> *pMgr = (TCP_Mgr<TCP_Session> *)Get_TCPMgr();
	const std::map<std::string, DATA_POLICY> *pPolicy = pMgr->GetPolicy();
	if(true == pPolicy->empty())
		return;

	DATA_PACKET *pData = new DATA_PACKET();

	bool bPass = false;
	std::string strIP_Port;
	for(auto &policy : *pPolicy)
	{
		// ip port 검색
		// 아무것도 없으면 무조건 다 받는다
		if( true == policy.second.vec_ip_port.empty())
			bPass = true;
		else
		{
			for(auto &ip_port : policy.second.vec_ip_port )
			{
				if( ip_port == m_strIP_Port )
				{
					bPass = true;
					break;
				}
			}
		}
		if( false == bPass )
			continue;

		// 정규식 검색
		// 아무것도 없으면 무조건 다 받는다
		if( true == policy.second.vec_regex.empty())
			bPass = true;
		else
		{
			for(auto &reg : policy.second.vec_regex)
			{
				std::regex reg_ex(reg);
				std::smatch result;

				if( std::regex_search(pData->strData, result, reg_ex) )
				{
					bPass = true;
					break;
				}
			}
		}
		if( false == bPass )
			continue;

		// 서비스 로그
		// 아무것도 없으면 무조건 다 받는다
		if( true == policy.second.vec_service.empty())
			bPass = true;
		else
		{
			for(auto &service : policy.second.vec_service)
			{
				for( auto &d_svc : pData->vec_service )
				{
					if(service == d_svc)
					{
						bPass = true;
						break;
					}
				}

				if( true == bPass )
					break;
			}
		}
		if( false == bPass )
			continue;

		if (true == bPass)
			// Push_Data 안에 delete를 자동 호출 해준다. 따로 delete 를 호출 하지 말자 
			pMgr->Push_Data( policy.first.c_str(), pData );
	}
}

void TCP_Session::OnEvent_Init()
{
	m_strIP_Port = Get_SessionIP();
	m_strIP_Port += ":";
	m_strIP_Port += std::to_string( Get_Port() );
}