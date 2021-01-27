#include <stdio.h>
#include <thread>
#include "Conf_Yaml.hpp"
#include "protobuf/loan.pb.h"
#include "Run.hpp"
#include <glog/logging.h>
#include <chrono>

void Read_Tail( __in std::ifstream &_fsLog, 
				__in bool &_bThreadRun,
				__inout MsgLog_Q &_LogQ )
{
	try
	{
		std::string strline("");
		while (true == _bThreadRun) 
		{
			while(true == _bThreadRun)
			{
				strline = "";
				if(getline(_fsLog, strline))
				{
					loan::MsgLog msgLog;
					msgLog.set_msg_type("MSG_TYPE_GEN");
					msgLog.set_msg_cmd(1);
					msgLog.set_log_time_seconds( std::chrono::system_clock::to_time_t(std::chrono::system_clock::now()) );
					//msgLog.set_log_time_seconds( std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count() );
					msgLog.set_service_name("crolling");
					msgLog.set_log_contents(strline);
					//msgLog.set_log_option(strline);

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

std::thread Run_Tail( 	__in const char *_pszFilePath, 
						__in bool &_bThreadRun,
						__inout MsgLog_Q &_LogQ )
{
	std::thread thr = std::thread( [_pszFilePath, &_bThreadRun, &_LogQ]() 
	{
		while(true == _bThreadRun)
		{
			std::cout << "[Run_Tail][Fail] 2" << _pszFilePath << " is not exist." << std::endl;

			std::ifstream fsLog;
			//fsLog.open( _pszFilePath, std::ios::in | std::ios::ate);
			fsLog.open( _pszFilePath, std::ios::in );

			if (false == fsLog.is_open()) 
			{
				std::cout << "[Run_Tail][Fail]" << _pszFilePath << " is not exist." << std::endl;
				sleep(1000);
				continue;
			}

			std::cout << "[SUCC] file open (" << _pszFilePath << ")" << std::endl;
			Read_Tail(fsLog, _bThreadRun, _LogQ);
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

	bool bThreadRun = true;
	std::vector<std::thread> vecThr;
	for( auto &service : Policy.m_mapLogService )
		vecThr.emplace_back( Run_Tail( service.second.c_str(), bThreadRun, LogQ ) );

	Loan_Run run;
	run.Run( Policy, &clientMgr, Thr_Client, nullptr, -1 );

	bThreadRun = false;
	for(auto &thr : vecThr)
	{
		if(true == thr.joinable())
			thr.join();
	}

    std::cout << "[FINISH] << loan_gether stop" << std::endl;
	run.Stop_All(&clientMgr, nullptr, Thr_Client, LogQ);

	google::protobuf::ShutdownProtobufLibrary();

    return 0;
}