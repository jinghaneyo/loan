#include <stdio.h>
#include "TCP_Mgr.hpp"
#include "TCP_Session.hpp"
#include "TCP_ClientMgr.hpp"
#include "MsgLog_Q.hpp"
#include <glog/logging.h>
#include <thread>

int main()
{
	GOOGLE_PROTOBUF_VERIFY_VERSION;

    // google::InitGoogleLogging("DUMP");   
 	// google::SetLogDestination( google::GLOG_INFO, "./DUMP." );  
	// google::EnableLogCleaner(3);

	MsgLog_Q logQ;

	TCP_ClientMgr clientMgr(&logQ);
	clientMgr.Add_Eng("172.17.0.2", 4444);
	clientMgr.Add_Eng("172.17.0.3", 5555);
	//std::thread thr_client( std::bind(&TCP_ClientMgr::Run, clientMgr) );
	std::thread thr_client( [&]()
	{
		clientMgr.Run();
 	});
	thr_client.detach();

    std::cout << "[START] << server run" << std::endl;

	TCP_Mgr<TCP_Session> mgr(&logQ);

	Rigitaeda::Rigi_Server server(10240);
	server.Run( 3333, 100, &mgr);

	clientMgr.Stop();
	thr_client.join();
	logQ.Clear_Q();

	google::protobuf::ShutdownProtobufLibrary();

    return 0;
}