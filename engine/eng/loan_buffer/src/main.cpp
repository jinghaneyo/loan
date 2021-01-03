#include <stdio.h>
#include <thread>
#include "TCP_Mgr.hpp"
#include "TCP_Session.hpp"
#include "TCP_ClientMgr.hpp"
#include "MsgLog_Q.hpp"
//#include <glog/logging.h>
//#include <yaml-cpp/parser.h>

bool Load_yaml( const char *_pszPath_Conf )
{
	printf("[Load_yaml] Path >> %s\n\n", _pszPath_Conf);

	// YAML::Node primes = YAML::LoadFile(_pszPath_Conf);
	// for (std::size_t i=0;i<primes.size();i++) 
	// {
	// 	std::cout << primes[i].as<int>() << "\n";
	// }
	// // or:
	// for (YAML::const_iterator it=primes.begin();it!=primes.end();++it) 
	// {
	// 	std::cout << it->as<int>() << "\n";
	// }

	// primes.push_back(13);
	// assert(primes.size() == 6);

	return true;
}

void Run_Client( __in TCP_ClientMgr &_ClientMgr, __out std::thread &_Thr_Clinet )
{
	// 분석 엔진 등록 
	// _ClientMgr.Add_Eng("172.17.0.2", 4444);
	// _ClientMgr.Add_Eng("172.17.0.3", 5555);
	_ClientMgr.Add_Eng_Failover_Active("172.17.0.2", 4444);
	_ClientMgr.Add_Eng_Failover_Standby("172.17.0.3", 5555);
	_Thr_Clinet = std::thread( [&]()
	{
		_ClientMgr.Run();
 	});
	_Thr_Clinet.detach();

    std::cout << "[START] << ClientMgr Run" << std::endl;
}

void Run_Server( __in TCP_Mgr<TCP_Session> &_ServerMgr )
{
    std::cout << "[START] << server run" << std::endl;

	Rigitaeda::Rigi_Server server(10240);
	server.Run( 3333, 100, &_ServerMgr );
}

void Stop_All( 	__in TCP_ClientMgr &_ClientMgr, 
				__in TCP_Mgr<TCP_Session> &_ServerMgr, 
				__in std::thread &_Thr_Clinet,
				__in MsgLog_Q &_LogQ )
{
	// -----------------------------------------------------
	// 종료-> 리소스 해제 
	// -----------------------------------------------------
	_ClientMgr.Stop();
	_Thr_Clinet.join();
	_ServerMgr.Stop();
	_LogQ.Clear_Q();

    std::cout << "[FINISH] << server stop" << std::endl;
}

int main()
{
	GOOGLE_PROTOBUF_VERIFY_VERSION;

    // google::InitGoogleLogging("DUMP");   
 	// google::SetLogDestination( google::GLOG_INFO, "./DUMP." );  
	// google::EnableLogCleaner(3);

	std::string strCurrentPath;
	char strBuffer[1024] = { 0, };
	strCurrentPath = getcwd( strBuffer, sizeof(strBuffer) );
	std::string strPath_Conf = strCurrentPath + "/conf.yaml";
	Load_yaml(strPath_Conf.c_str());

	MsgLog_Q logQ;
	TCP_ClientMgr clientMgr(&logQ);
	TCP_Mgr<TCP_Session> serverMgr(&logQ);

	std::thread thr_client;
	Run_Client( clientMgr, thr_client );

	Run_Server( serverMgr );

	Stop_All( clientMgr, serverMgr, thr_client, logQ );

	google::protobuf::ShutdownProtobufLibrary();

    return 0;
}