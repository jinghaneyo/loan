#include <stdio.h>
#include "loan_Server.hpp"

int main( int argc, char* argv[])
{
    // google::InitGoogleLogging("DUMP");   
 	// google::SetLogDestination( google::GLOG_INFO, "./DUMP." );  
	//google::EnableLogCleaner(3);

    std::cout << "argc = " << argc << std::endl;

	int nPort = 5555;
	if( 2 == argc)
		nPort = atoi(argv[1]);

    std::cout << "[START] << server run (port = " << nPort << ")" << std::endl;

	Rigitaeda::Rigi_TCPMgr<TCP_Session> mgr;
	Rigitaeda::Rigi_Server server;

	//server.Load_Conf();
	server.Run( nPort, 100, &mgr );

    return 0;
}