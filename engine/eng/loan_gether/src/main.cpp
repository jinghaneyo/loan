#include <stdio.h>
#include <thread>
#include "Conf_Yaml.hpp"
#include "protobuf/loan.pb.h"
#include "Run.hpp"
#include <glog/logging.h>
#include <chrono>

void Read_Tail( __in std::ifstream &_fsLog, 
				__in bool &_bThreadRun,
				__inout MsgLog_Q &_LogQ,
				__inout long &_Pos_seek )
{
	try
	{
		int nEof_try_count = 0;
		int nEof_Limit_try_count = 10;

		std::string strline("");
		while(true == _bThreadRun)
		{
			strline = "";
			if(getline(_fsLog, strline))
			{
				if( true == strline.empty() )
					continue;

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

				_Pos_seek += strline.length() + 1;	// \n 도 더한다

				_LogQ.Push_back(pstrLine);
			}
			else
			{
				if ( true == _fsLog.eof() ) 
				{
					if( nEof_try_count < nEof_Limit_try_count )
					{
						nEof_try_count++;
						sleep(1);
					}
					else
						break; 
				}
			}
		}

		_fsLog.clear();
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
	std::cout << "file close" << std::endl;
	sleep(1);
      
	return ;
}

std::thread Run_Tail( 	__in const char *_pszFilePath, 
						__in bool &_bThreadRun,
						__inout MsgLog_Q &_LogQ )
{
	std::thread thr = std::thread( [_pszFilePath, &_bThreadRun, &_LogQ]() 
	{
		long nPos_seek = 0;

		while(true == _bThreadRun)
		{
			std::ifstream fsLog;
			fsLog.open( _pszFilePath, std::ios::in | std::ios::ate);
			//fsLog.open( _pszFilePath, std::ios::in );

			if (false == fsLog.is_open()) 
			{
				nPos_seek = 0;
				sleep(1);
				continue;
			}

			std::cout << "[SUCC] file open => path = " << _pszFilePath << " | seek = " << std::to_string(nPos_seek) << std::endl;
			fsLog.seekg(nPos_seek);
			Read_Tail(fsLog, _bThreadRun, _LogQ, nPos_seek);
		}

		std::cout << "[FINISH] thread " << _pszFilePath << std::endl;
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
	for( auto &mapLog: Policy.m_mapLogService )
	{
		for( auto &config : mapLog.second )
		{
			vecThr.emplace_back( Run_Tail( config["path"].c_str(), bThreadRun, LogQ ) );
		}
	}

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