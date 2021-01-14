#include <stdio.h>
#include "loan_Server.hpp"

bool Event_Init()
{
	return true;
}

void Event_Close()
{
}

void Event_Receive( __in char *_pData, __in size_t _nData_len )
{
	loan::MsgLog msgLog;
	msgLog.ParseFromString(_pData);

	if (false == msgLog.service_name().empty())
		std::cout << "[Event_Receive] << Service Name : " << msgLog.service_name() << " << Log contents : " << msgLog.logcontents() << std::endl;
}

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

	Rigitaeda::Rigi_Server server;
	//server.Load_Conf();

	/*
	server.Add_Event_Handler_Close( Event_Close );
	server.Add_Event_Handler_Init( Event_Init );
	server.Add_Event_Handler_Receive( Event_Receive );
	server.Run( nPort, 100 );
	//*/

	//*
	Rigitaeda::Rigi_TCPServerMgr<TCP_Session> mgr;
	server.Run( nPort, 100, &mgr );
	//*/

    return 0;
}