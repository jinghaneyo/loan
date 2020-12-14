// regex 헤더 안에 이미 __in, __out 정의 되어있다
// 해서 Rigi_Def.hpp 보다 먼저 선언을 해야 한다
#include <regex>
#include "TCP_Session.hpp"
#include "TCP_Mgr.hpp"
#include "loan.pb.h"
#include <google/protobuf/descriptor.h>
#include <google/protobuf/dynamic_message.h>
#include <google/protobuf/io/zero_copy_stream_impl.h>
#include <google/protobuf/io/tokenizer.h>
#include <google/protobuf/message.h>

using namespace google::protobuf;
using namespace google::protobuf::io;
using namespace google::protobuf::compiler;

TCP_Session::TCP_Session()
{ 
}

TCP_Session::~TCP_Session() 
{ 
}

void TCP_Session::OnEvent_Receive(	__in char *_pData,
									__in size_t _nData_len )
{
	//std::cout << "[TCP_Session::Receive] >> " << _pData << std::endl;

	loan::MsgLog *pPacket = new loan::MsgLog();
	pPacket->ParseFromString(_pData);

	// std::cout << "[RECV] << msg_type = " << std::to_string(pPacket->msg_type()) << " | msg_cmd = " << std::to_string(pPacket->msg_cmd()) << " | service_name = " << pPacket->service_name() << std::endl;
	// std::cout << "[RECV] << logcontents = " << pPacket->logcontents() << std::endl;

	if(false == Task_Filter( pPacket ))
		delete pPacket;

	// 응답 주기
	// std::string strPacket = "[{'cmd':'run'},{'data': 'test pakcet'}]";
	// Send( strPacket.c_str(), strPacket.length() );
}

void TCP_Session::OnEvent_Close()
{
	std::cout << "[TCP_Session::OnClose] >> " << Get_SessionIP() << std::endl;
}

bool TCP_Session::Task_Filter( __in loan::MsgLog *_pPacket )
{
	TCP_Mgr<TCP_Session> *pMgr = (TCP_Mgr<TCP_Session> *)Get_TCPMgr();
	const std::map<std::string, DATA_POLICY> *pPolicy = pMgr->GetPolicy();
	if(true == pPolicy->empty())
		return false;

	bool bIsPushQ = false;
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

				// if( std::regex_search(_pPacket->get_logcontents(), result, reg_ex) )
				// {
				// 	bPass = true;
				// 	break;
				// }
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
				// for( auto &d_svc : _pPacket->vec_service )
				// {
				// 	if(service == d_svc)
				// 	{
				// 		bPass = true;
				// 		break;
				// 	}
				// }

				if( true == bPass )
					break;
			}
		}
		if( false == bPass )
			continue;

		if (true == bPass)
		{
			// Push_Data 안에 delete를 자동 호출 해준다. 따로 delete 를 호출 하지 말자 
			pMgr->Push_Data( policy.first.c_str(), _pPacket );

			bIsPushQ = true;
		}
	}

	//std::cout << "[RECV] << " << _pPacket->mutable_logcontents() << std::endl;

	return bIsPushQ;
}

bool TCP_Session::OnEvent_Init()
{
	TCP_Mgr<TCP_Session> *pMgr = (TCP_Mgr<TCP_Session> *)Get_TCPMgr();

	m_strIP_Port = Get_SessionIP();
	m_strIP_Port += ":";
	m_strIP_Port += std::to_string( pMgr->Get_Port() );

	return true;
}