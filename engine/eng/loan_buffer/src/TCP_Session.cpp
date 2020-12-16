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
	loan::MsgLog *pPacket = new loan::MsgLog();
	pPacket->ParseFromString(_pData);
	
	switch((int)pPacket->msg_type())
	{
		case (int)MsgLog_Type::CROLLING:
		{
			if(false == Task_Filter( pPacket ))
				delete pPacket;
		}
		break;
		default:
		// 이외는 잘못된 데이터 
		std::cout << "[RECV] << Not support msg_type = " << std::to_string(pPacket->msg_type()) << std::endl;
		assert(0 && "[RECV] not support msg_type");
		break;
	}
}

void TCP_Session::OnEvent_Close()
{
	std::cout << "[TCP_Session::OnClose] >> " << Get_SessionIP() << std::endl;
}

bool TCP_Session::Task_Filter( __in loan::MsgLog *_pPacket )
{
	switch((int)_pPacket->msg_cmd())
	{
		case (int)MsgLog_Cmd_Crolling::SENDLING:
			return Input_Filter(_pPacket);
		default:
		std::cout << "[RECV] << Not support cmd_type = " << std::to_string(_pPacket->msg_cmd()) << std::endl;
		assert(0 && "[RECV] not support cmd_type");
		return false;
	}

	return false; 
}

bool TCP_Session::Input_Filter( __in loan::MsgLog *_pPacket )
{
	TCP_Mgr<TCP_Session> *pMgr = (TCP_Mgr<TCP_Session> *)Get_TCPMgr();
	const std::map<std::string, DATA_POLICY> *pPolicy = pMgr->GetPolicy();
	if(true == pPolicy->empty())
		return false;

	// 큐에 설정 리미트에 도달하면 중지 명령을 보내자 
	if(pMgr->GetQ_LimitSize() == pMgr->GetQ_Size())
	{
		loan::MsgLog msg_stop;
		msg_stop.set_msg_type( (int)MsgLog_Type::CROLLING );
		msg_stop.set_msg_cmd( (int)MsgLog_Cmd_Crolling::STOP_REQU );

		std::string strSendData;
		msg_stop.SerializeToString(&strSendData);
		Send( strSendData.c_str(), msg_stop.ByteSizeLong() );
	}

	// full 크기에 도착하면 버린다
	if(pMgr->GetQ_FullSize() == pMgr->GetQ_Size())
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

				if( std::regex_search(_pPacket->logcontents(), result, reg_ex) )
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
				if(service == _pPacket->service_name())
				{
					bPass = true;
					break;
				}
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

	// false 리턴 시 delete 를 호출하도록 되어있으므로 PushQ true 경우 false 를 리턴하도록 해야한다 
	return bIsPushQ ? false : true;
}

bool TCP_Session::OnEvent_Init()
{
	TCP_Mgr<TCP_Session> *pMgr = (TCP_Mgr<TCP_Session> *)Get_TCPMgr();

	m_strIP_Port = Get_SessionIP();
	m_strIP_Port += ":";
	m_strIP_Port += std::to_string( pMgr->Get_Port() );

	return true;
}