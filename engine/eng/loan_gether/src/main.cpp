#include <stdio.h>
#include <thread>
#include "Conf_Yaml.hpp"
#include "protobuf/loan.pb.h"
#include "Run.hpp"
#include <glog/logging.h>

void Read_Tail( __in std::ifstream &_fsLog, __inout MsgLog_Q &_LogQ )
{
	try
	{
		std::string strline("");
		while (true) 
		{
			while(true)
			{
				strline = "";
				if(getline(_fsLog, strline))
				{
					loan::MsgLog msgLog;
					msgLog.set_msg_type("MSG_TYPE_GEN");
					msgLog.set_msg_cmd(1);
					msgLog.set_service_name("crolling");
					msgLog.set_logcontents(strline);

					std::cout << strline << std::endl;

					std::string *pstrLine = new std::string();
					msgLog.SerializeToString(&(*pstrLine));

					_LogQ.Push_back(pstrLine);
				}
				else
					break;
			}

			if (!_fsLog.eof()) 
				break; 
				
			_fsLog.clear();
		}
	}
	catch(std::exception const& e)
	{
		std::cerr << "[Exception][Read_Tail] Err = " << e.what() << "\n";
	}
	catch(...)
	{
		std::cerr << "[Exception][Read_Tail] Err = Unknown " << "\n";
	}

	_fsLog.close();
      
	return ;
}

std::thread Run_Tail( __in const char *_pszFilePath, __inout MsgLog_Q &_LogQ )
{
	std::thread thr = std::thread( [&]() 
	{
		while(true)
		{
			std::ifstream fsLog;
			//fsLog.open( _pszFilePath, std::ios::in | std::ios::ate);
			fsLog.open( _pszFilePath, std::ios::in );

			if (!fsLog.is_open()) 
			{
				std::cout << _pszFilePath << " is not exist." << std::endl;
				sleep(1000);
				continue;
			}

			std::cout << "[SUCC] file open (" << _pszFilePath << ")" << std::endl;
			Read_Tail(fsLog, _LogQ);
		}
	});
	thr.detach();

	return thr;
}

int main( int argc, char* argv[])
{
	GOOGLE_PROTOBUF_VERIFY_VERSION;

    // google::InitGoogleLogging("DUMP");   
 	// google::SetLogDestination( google::GLOG_INFO, "./DUMP." );  
	//google::EnableLogCleaner(3);

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

	std::thread Thr_tail = Run_Tail( "/share/engine/log.txt", LogQ );

	Loan_Run run;
	run.Run( Policy, &clientMgr, Thr_Client, nullptr, -1 );

    std::cout << "[FINISH] << loan_gether stop" << std::endl;
	run.Stop_All(&clientMgr, nullptr, Thr_Client, LogQ);

	google::protobuf::ShutdownProtobufLibrary();

    return 0;
}