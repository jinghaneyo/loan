#include <stdio.h>
#include "TCP_Mgr.hpp"
#include "TCP_Session.hpp"
#include "TCP_ClientMgr.hpp"
#include "MsgLog_Q.hpp"
#include <glog/logging.h>
#include <thread>
#include <yaml-cpp/parser.h>

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
	TCP_Mgr<TCP_Session> mgr(&logQ);

	// -----------------------------------------------------
	// 분석 엔진 등록 
	clientMgr.Add_Eng("172.17.0.2", 4444);
	clientMgr.Add_Eng("172.17.0.3", 5555);
	std::thread thr_client( [&]()
	{
		clientMgr.Run();
 	});
	thr_client.detach();
	// -----------------------------------------------------

	// -----------------------------------------------------
	// 서버 기동
    std::cout << "[START] << server run" << std::endl;
	Rigitaeda::Rigi_Server server(10240);
	server.Run( 3333, 100, &mgr);
	// -----------------------------------------------------

	// -----------------------------------------------------
	// 종료-> 리소스 해제 
	clientMgr.Stop();
	thr_client.join();
	logQ.Clear_Q();
	// -----------------------------------------------------

	google::protobuf::ShutdownProtobufLibrary();

    return 0;
}