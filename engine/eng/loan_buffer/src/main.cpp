#include <stdio.h>
#include "loan_Server.hpp"

int main()
{
    google::InitGoogleLogging("DUMP");   
 	google::SetLogDestination( google::GLOG_INFO, "./DUMP." );  
	//google::EnableLogCleaner(3);

    std::cout << "[START] << server run" << std::endl;

	Rigitaeda::Rigi_TCPMgr<TCP_Session> tcp;
	Rigitaeda::Rigi_Server server;
	server.Run( 3333, 100, &tcp );

    return 0;
}