#include <stdio.h>
#include <thread>
#include "Conf_Yaml.hpp"
#include "protobuf/loan.pb.h"
#include "Run.hpp"
#include <glog/logging.h>

int main( int argc, char* argv[] )
{
	GOOGLE_PROTOBUF_VERIFY_VERSION;

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
	run.Run( Policy, &clientMgr, Thr_Client, &serverMgr, atoi(Policy.m_ServerInfo.strPort.c_str()) );

    std::cout << "[FINISH] << loan_buffer stop" << std::endl;
	run.Stop_All(&clientMgr, &serverMgr, Thr_Client, LogQ);

	google::protobuf::ShutdownProtobufLibrary();

    return 0;
}