#include <stdio.h>
#include <thread>
#include "Conf_Yaml.hpp"
#include "protobuf/loan.pb.h"
#include "Run.hpp"
#include <glog/logging.h>

bool Event_Init()
{
	return true;
}

void Event_Close()
{
}

void Event_Receive( __in char *_pData, __in size_t _nData_len )
{
	// loan::MsgLog msgLog;
	// msgLog.ParseFromString(_pData);

	// if (false == msgLog.service_name().empty())
	// 	std::cout << "[Event_Receive] << Service Name : " << msgLog.service_name() << " << Log contents : " << msgLog.logcontents() << std::endl;
}

// bool TCP_ClientMgr::Input_Filter( __in loan::MsgLog &_Packet )
// {
// 	if(nullptr == m_pLogQ || nullptr == m_pPolicy)
// 	{
// 		//ASSERT(0 && "[TCP_Session::Input_Filter] m_pLogQ is nullptr");
// 		return false;
// 	}

// 	// 큐에 설정 리미트에 도달하면 중지 명령을 보내자 
// 	if(m_pLogQ->GetQ_LimitSize() == (int)m_pLogQ->GetQ_Size())
// 	{
// 		loan::MsgLog msg_stop;
// 		msg_stop.set_msg_type( MSG_TYPE_GEN );
// 		//msg_stop.set_msg_cmd( (int)MsgLog_Cmd_Crolling::STOP_REQU );
// 		msg_stop.set_msg_cmd( 1 );

// 		std::string strSendData;
// 		msg_stop.SerializeToString(&strSendData);
// 		// Sync_Send( strSendData.c_str(), msg_stop.ByteSizeLong() );
// 	}

// 	// full 크기에 도착하면 버린다
// 	if(m_pLogQ->GetQ_FullSize() == (int)m_pLogQ->GetQ_Size())
// 		return false;

// 	bool bIsPushQ = false;
// 	bool bPass = false;
// 	std::string strIP_Port;
// 	/*
// 	for(auto &policy : *m_pPolicy)
// 	{
// 		// ip port 검색
// 		// 아무것도 없으면 무조건 다 받는다
// 		if( true == policy.second.vec_ip_port.empty())
// 			bPass = true;
// 		else
// 		{
// 			for(auto &ip_port : policy.second.vec_ip_port )
// 			{
// 				if( ip_port == m_strIP_Port )
// 				{
// 					bPass = true;
// 					break;
// 				}
// 			}
// 		}
// 		if( false == bPass )
// 			continue;

// 		// 정규식 검색
// 		// 아무것도 없으면 무조건 다 받는다
// 		if( true == policy.second.vec_regex.empty())
// 			bPass = true;
// 		else
// 		{
// 			for(auto &reg : policy.second.vec_regex)
// 			{
// 				std::regex reg_ex(reg);
// 				std::smatch result;

// 				if( std::regex_search(_Packet.logcontents(), result, reg_ex) )
// 				{
// 					bPass = true;
// 					break;
// 				}
// 			}
// 		}
// 		if( false == bPass )
// 			continue;

// 		// 서비스 로그
// 		// 아무것도 없으면 무조건 다 받는다
// 		if( true == policy.second.vec_service.empty())
// 			bPass = true;
// 		else
// 		{
// 			for(auto &service : policy.second.vec_service)
// 			{
// 				if(service == _Packet.service_name())
// 				{
// 					bPass = true;
// 					break;
// 				}
// 			}
// 		}
// 		if( false == bPass )
// 			continue;

// 		if (true == bPass)
// 		{
// 			std::string *pstrSendData = new std::string();
// 			_Packet.SerializeToString(&(*pstrSendData));

// 			// Push_Data 안에 delete를 자동 호출 해준다. 따로 delete 를 호출 하지 말자 
// 			m_pLogQ->Push_back( policy.first.c_str(), pstrSendData );

// 			bIsPushQ = true;
// 		}
// 	}
// //*/
// 	return bIsPushQ;
// }

int main( int argc, char* argv[])
{
	GOOGLE_PROTOBUF_VERIFY_VERSION;

    std::cout << "argc = " << argc << std::endl;

	int nPort = 5555;
	if( 2 == argc)
		nPort = atoi(argv[1]);

    // google::InitGoogleLogging(argv[0]);   
 	// google::SetLogDestination( google::GLOG_INFO, "./DUMP." );  
	// google::EnableLogCleaner(3);

	std::string strCurrentPath;
	char strBuffer[1024] = { 0, };
	strCurrentPath = getcwd( strBuffer, sizeof(strBuffer) );
	std::string strPath_Conf = strCurrentPath + "/conf.yaml";

    DATA_POLICY Policy;
    bool bRet = Conf_Yaml::Load_yaml(strPath_Conf.c_str(), &Policy);
    if( false == bRet )
	{
        return 1;
	}
	std::cout << "[MAIN][SUCC] Conf_Yaml::Load_yaml" << std::endl;

 	std::thread Thr_Client;
	MsgLog_Q LogQ;
	TCP_ClientMgr clientMgr(&LogQ, &Policy);
	TCP_ServerMgr<TCP_Session> serverMgr(&LogQ, &Policy);

	Loan_Run run;
	run.Run( Policy, &clientMgr, Thr_Client, &serverMgr, nPort );

	std::cout << "[FINISH] << loan_analyzer stop" << std::endl;
	run.Stop_All(&clientMgr, &serverMgr, Thr_Client, LogQ);

	google::protobuf::ShutdownProtobufLibrary();

    return 0;
}