#include <stdio.h>
#include <thread>
#include "TCP_Mgr.hpp"
#include "TCP_Session.hpp"
#include "TCP_ClientMgr.hpp"
#include "MsgLog_Q.hpp"
#include "Conf_Yaml.hpp"
//#include <glog/logging.h>

bool Split_IP_Port( __in std::string &_strSource, __out std::string &_strIP, __out std::string &_strPort )
{
	int nPos = (int)_strSource.find(":");
	if(-1 != nPos )
	{
		_strIP = _strSource.substr(0, nPos);
		_strPort = _strSource.substr(nPos + 1, _strSource.length()-1-nPos );

		return true;
	}

	return false;
}

void Add_Eng( 	__in TCP_ClientMgr &_ClientMgr, 
				__in DATA_POLICY &_Policy )
{
	if( "round-robin" == _Policy.m_SendRule )
	{
		for(auto &data : _Policy.m_vecRoudRobin)
		{
			std::string strIP, strPort;
			if( true == Split_IP_Port(data, strIP, strPort) )
				_ClientMgr.Add_Eng_RoundRobin( strIP.c_str(), strPort.c_str() );
		}
	}
	else if( "fail-over" == _Policy.m_SendRule )
	{
		for(auto &map_vec : _Policy.m_mapFailOver_IP_Port)
		{
			for(auto &data : map_vec.second )
			{
				if("active" == map_vec.first)
				{
					std::string strIP, strPort;
					if( true == Split_IP_Port(data, strIP, strPort) )
						_ClientMgr.Add_Eng_FailOver_Active( strIP.c_str(), strPort.c_str() );
				}
				else if("stand-by" == map_vec.first)
				{
					std::string strIP, strPort;
					if( true == Split_IP_Port(data, strIP, strPort) )
						_ClientMgr.Add_Eng_FailOver_Standby( strIP.c_str(), strPort.c_str() );
				}
			}
		}
	}
	else if( "fail-back" == _Policy.m_SendRule )
	{
		for(auto &map_vec : _Policy.m_mapFailOver_IP_Port)
		{
			for(auto &data : map_vec.second )
			{
				if("active" == map_vec.first)
				{
					std::string strIP, strPort;
					if( true == Split_IP_Port(data, strIP, strPort) )
						_ClientMgr.Add_Eng_FailBack_Active( strIP.c_str(), strPort.c_str() );
				}
				else if("stand-by" == map_vec.first)
				{
					std::string strIP, strPort;
					if( true == Split_IP_Port(data, strIP, strPort) )
						_ClientMgr.Add_Eng_FailBack_Standby( strIP.c_str(), strPort.c_str() );
				}
			}
		}
	}
}

void Run_Client( 	__in TCP_ClientMgr &_ClientMgr, 
					__in DATA_POLICY &_Policy,
					__out std::thread &_Thr_Clinet )
{
	// 분석 엔진 등록 
	Add_Eng( _ClientMgr, _Policy );

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

	DATA_POLICY Policy;
	if(false == Conf_Yaml::Load_yaml(strPath_Conf.c_str(), Policy) )
		return 1;

	MsgLog_Q logQ;
	TCP_ClientMgr clientMgr(&logQ, &Policy);
	TCP_Mgr<TCP_Session> serverMgr(&logQ, &Policy);

	std::thread thr_client;
	Run_Client( clientMgr, Policy, thr_client );

	Run_Server( serverMgr );

	Stop_All( clientMgr, serverMgr, thr_client, logQ );

	google::protobuf::ShutdownProtobufLibrary();

    return 0;
}