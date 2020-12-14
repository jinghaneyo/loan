#include <stdio.h>
#include "TCP_Mgr.hpp"
#include "TCP_Session.hpp"
#include "Rigi_ClientTCP.hpp"

int main()
{
	GOOGLE_PROTOBUF_VERIFY_VERSION;

    // google::InitGoogleLogging("DUMP");   
 	// google::SetLogDestination( google::GLOG_INFO, "./DUMP." );  
	//google::EnableLogCleaner(3);

	// boost::asio::io_service io_service_client;
	// Rigitaeda::Rigi_ClientTCP client;
	// client.Connect( "172.17.0.5", 5555, io_service_client );
	// std::string strData = "['cmd':'client tcp test']";
	// client.SendPacket(strData.c_str(), strData.length());
	// sleep(1);

    std::cout << "[START] << server run" << std::endl;

	TCP_Mgr<TCP_Session> mgr;
	Rigitaeda::Rigi_Server server(1024);

	server.Run( 3333, 100, &mgr);

	google::protobuf::ShutdownProtobufLibrary();

    return 0;
}