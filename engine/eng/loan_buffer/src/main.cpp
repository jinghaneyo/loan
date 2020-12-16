#include <stdio.h>
#include "TCP_Mgr.hpp"
#include "TCP_Session.hpp"
#include "TCP_ClientMgr.hpp"
#include "MsgLog_Q.hpp"
#include <glog/logging.h>

int main()
{
	GOOGLE_PROTOBUF_VERIFY_VERSION;

    // google::InitGoogleLogging("DUMP");   
 	// google::SetLogDestination( google::GLOG_INFO, "./DUMP." );  
	// google::EnableLogCleaner(3);

	MsgLog_Q logQ;

	TCP_ClientMgr clientMgr;
	clientMgr.Set_LogQ(&logQ);
	clientMgr.Add_Analyzer_Eng("172.17.0.2", 4444);
	clientMgr.Add_Analyzer_Eng("172.17.0.3", 5555);
	clientMgr.Async_Run();

    std::cout << "[START] << server run" << std::endl;

	TCP_Mgr<TCP_Session> mgr;
	mgr.Set_LogQ(&logQ);

	Rigitaeda::Rigi_Server server(1024);
	server.Run( 3333, 100, &mgr);

	logQ.Clear_Q();

	google::protobuf::ShutdownProtobufLibrary();

    return 0;
}