#include <stdio.h>
#include <thread>
#include "TCP_Mgr.hpp"
#include "TCP_Session.hpp"
#include "TCP_ClientMgr.hpp"
#include "MsgLog_Q.hpp"
#include <yaml-cpp/yaml.h>
#include "Data_Policy.hpp"
//#include <glog/logging.h>

void Parse_Server( __in const YAML::Node &_node, __out DATA_POLICY &_Policy )
{
	for( auto itr : _node )
	{
		if ( "protocol" == itr.first.as<std::string>() )
		{
			_Policy.m_ServerInfo.strProtocol = itr.second.as<std::string>();
			std::cout << _Policy.m_ServerInfo.strProtocol << std::endl;
		}
		else if ( "port" == itr.first.as<std::string>() )
		{
			_Policy.m_ServerInfo.strPort = itr.second.as<std::string>();
			std::cout << _Policy.m_ServerInfo.strPort << std::endl;
		}
	}
}

void Parse_Group( __in const YAML::Node &_node, __out std::map<std::string, std::string> &_mapOut )
{
	for( auto &itr : _node )
	{
		for( auto &data : itr.second )
		{
			auto find = _mapOut.find( data.as<std::string>() );
			if(find == _mapOut.end())
				_mapOut.insert( std::make_pair( itr.first.as<std::string>(), data.as<std::string>() ) );
			else
				find->second = data.as<std::string>();

			std::cout << "[GROUP][KEY = " << itr.first.as<std::string>() <<  "][VALUE = " << data.as<std::string>() << "]" << std::endl;
		}
	}
}

void Parse_SendRule( 	__in const YAML::Node &_node, 
						__out std::map<std::string, std::string> &_mapRoundRobin,
						__out std::map<std::string, std::string> &_mapFailOver,
						__out std::map<std::string, std::string> &_mapFailBack
					)
{
	for( auto &itr : _node )
	{
		if ( "round-robin" == itr.first.as<std::string>() )
		{
			for( auto &data : itr.second )
			{
				auto find = _mapRoundRobin.find( data.as<std::string>() );
				if(find == _mapRoundRobin.end())
					_mapRoundRobin.insert( std::make_pair( itr.first.as<std::string>(), data.as<std::string>() ) );
				else
					find->second = data.as<std::string>();

				std::cout << "[SEND-RULE][ROUND-ROBIN][KEY = " << itr.first.as<std::string>() <<  "][VALUE = " << data.as<std::string>() << "]" << std::endl;
			}
		}
		else if ( "fail-over" == itr.first.as<std::string>() )
		{
			for( auto &data : itr.second )
			{
				if( "session-change-count" == data.first.as<std::string>() )
				{
					for( auto &chg : data.second )
					{
						std::cout << "[SEND-RULE][FAIL-OVER][KEY = " << chg.first.as<std::string>() <<  "][VALUE = " << chg.second.as<std::string>() << "]" << std::endl;
					}
				}
				else
				{
					for( auto &ip : data.second )
					{
						std::cout << "[SEND-RULE][FAIL-OVER][KEY = " << data.first.as<std::string>() <<  "][VALUE = " << ip.as<std::string>() << "]" << std::endl;
					}
				}
			}
		}
		else if ( "fail-back" == itr.first.as<std::string>() )
		{
			for( auto &data : itr.second )
			{
				if( "session-change-count" == data.first.as<std::string>() )
				{
					for( auto &chg : data.second )
					{
						std::cout << "[SEND-RULE][FAIL-BACK][KEY = " << chg.first.as<std::string>() <<  "][VALUE = " << chg.second.as<std::string>() << "]" << std::endl;
					}
				}
				else
				{
					for( auto &ip : data.second )
					{
						std::cout << "[SEND-RULE][FAIL-BACK][KEY = " << data.first.as<std::string>() <<  "][VALUE = " << ip.as<std::string>() << "]" << std::endl;
					}
				}
			}
		}
	}
}

void Parse_Destination( __in const YAML::Node &_node, __out DATA_POLICY &_Policy )
{
	std::map<std::string, std::string> mapRoundrobin;

	//for(auto itr = _node.begin(); itr != _node.end(); itr++)
	{
		// if ( "period_retry_connect_time" == itr->first.as<std::string>() )
		// {
		// 	//_ServerInfo.strProtocol = itr->second.as<std::string>();
		// 	std::cout << itr->second.as<std::string>() << std::endl;
		// }
		// else if ( "round-robin" == itr->first.as<std::string>() )
		// {
		// 	for(auto it = itr->second.begin(); it != itr->second.end(); it++)
		// 	{
		// 		//_ServerInfo.strPort = itr->second.as<std::string>();
		// 		std::cout << it->as<std::string>() << std::endl;
		// 	}
		// }
		// else if ( "fail-over" == itr->first.as<std::string>() )
		// {
		// 	for(auto it = itr->second.begin(); it != itr->second.end(); it++)
		// 	{
		// 		//_ServerInfo.strPort = itr->second.as<std::string>();
		// 		std::cout << it->as<std::string>() << std::endl;
		// 	}
		// }
		// else if ( "fail-back" == itr->first.as<std::string>() )
		// {
		// 	for(auto it = itr->second.begin(); it != itr->second.end(); it++)
		// 	{
		// 		//_ServerInfo.strPort = itr->second.as<std::string>();
		// 		std::cout << it->as<std::string>() << std::endl;
		// 	}
		// }
	}
}

bool Load_yaml( __in const char *_pszPath_Conf, 
				__out DATA_POLICY &_Policy )
{
	printf("[Load_yaml] Path >> %s\n\n", _pszPath_Conf);

	YAML::Node node = YAML::LoadFile( _pszPath_Conf );
	// if(false == node.IsNull())
	// {
	// 	std::cout << "not exist conf.yaml (" << _pszPath_Conf << ")" << std::endl;
	// 	return false;
	// }

	if ( node["server"] )
		Parse_Server( node["server"], _Policy );
	else
	{
		std::cout << "not exist section => server" << std::endl;
		return false;
	}

	std::map<std::string, std::string> mapGroup;
	if ( node["group"] )
		Parse_Group( node["group"], mapGroup );
	else
	{
		std::cout << "not exist section => group" << std::endl;
		return false;
	}

	std::map<std::string, std::string> mapRoudRobin;
	std::map<std::string, std::string> mapFailOver;
	std::map<std::string, std::string> mapFailBack;
	if ( node["send-rule"] )
		Parse_SendRule( node["send-rule"], mapRoudRobin, mapFailOver, mapFailBack );
	else
	{
		std::cout << "not exist section => send-rule" << std::endl;
		return false;
	}

	// send-rule 에 group 이 있다면 값을 대체해 주자

	// for(auto itr = node.begin(); itr != node.end(); itr++)
	// {
	// 	if ( "server" == itr->first.as<std::string>() )
	// 		Parse_Server( itr->second, _Policy );
	// 	else if ( "group" == itr->first.as<std::string>() )
	// 		Parse_Group( itr->second, _Policy );
	// 	else if ( "destination" == itr->first.as<std::string>() )
	// 		Parse_Destination( itr->second, _Policy );
	// }

	// for(auto itr = node.begin(); itr != node.end(); itr++)
	// {
	// 	if ( "server" == itr->first.as<std::string>() )
	// 		Parse_Server( itr->second, _Policy );
	// 	else if ( "group" == itr->first.as<std::string>() )
	// 		Parse_Group( itr->second, _Policy );
	// 	else if ( "destination" == itr->first.as<std::string>() )
	// 		Parse_Destination( itr->second, _Policy );
	// }

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

	DATA_POLICY Policy;
	if(false == Load_yaml(strPath_Conf.c_str(), Policy) )
		return 1;

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