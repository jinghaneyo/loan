#include <stdio.h>
#include <thread>
#ifdef _WIN32
   #include <io.h> 
   #define access    _access_s
#else
   #include <unistd.h>
#endif
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

void Parse_SendRule( 	__in const YAML::Node &_node, __out DATA_POLICY &_Policy )
{
	for( auto &itr : _node )
	{
		if ( "round-robin" == itr.first.as<std::string>() )
		{
			for( auto &data : itr.second )
				_Policy.m_vecRoudRobin.emplace_back( data.as<std::string>() );
		}
		else if ( "fail-over" == itr.first.as<std::string>() )
		{
			for( auto &data : itr.second )
			{
				if( "session-change-count" == data.first.as<std::string>() )
				{
					for( auto &chg : data.second )
					{
						if( "active" == chg.first.as<std::string>() )
							_Policy.m_vecFailOver_Session[INDEX_ACTIVE] = chg.second.as<int>();
						else if( "stand-by" == chg.first.as<std::string>() )
							_Policy.m_vecFailOver_Session[INDEX_STANDBY] = chg.second.as<int>();
					}
				}
				else if( "active" == data.first.as<std::string>() || "stand-by" == data.first.as<std::string>() )
				{
					for( auto &ip : data.second )
					{
						auto find = _Policy.m_mapFailOver_IP_Port.find( data.first.as<std::string>() );
						if(find == _Policy.m_mapFailOver_IP_Port.end())
						{
							std::vector<std::string> vecIP;
							vecIP.emplace_back( ip.as<std::string>() );
							_Policy.m_mapFailOver_IP_Port.insert( std::make_pair(data.first.as<std::string>(), vecIP) );
						}
						else
							find->second.emplace_back( ip.as<std::string>() );
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
						if( "active" == chg.first.as<std::string>() )
							_Policy.m_vecFailBack_Session[INDEX_ACTIVE] = chg.second.as<int>();
						else if( "stand-by" == chg.first.as<std::string>() )
							_Policy.m_vecFailBack_Session[INDEX_STANDBY] = chg.second.as<int>();
					}
				}
				else if( "active" == data.first.as<std::string>() || "stand-by" == data.first.as<std::string>() )
				{
					for( auto &ip : data.second )
					{
						auto find = _Policy.m_mapFailBack_IP_Port.find( data.first.as<std::string>() );
						if(find == _Policy.m_mapFailBack_IP_Port.end())
						{
							std::vector<std::string> vecIP;
							vecIP.emplace_back( ip.as<std::string>() );
							_Policy.m_mapFailBack_IP_Port.insert( std::make_pair(data.first.as<std::string>(), vecIP) );
						}
						else
							find->second.emplace_back( ip.as<std::string>() );
					}
				}
			}
		}
	}
}

void Parse_Destination( __in const YAML::Node &_node, __out DATA_POLICY &_Policy )
{
	for( auto &itr : _node )
	{
		if ( "period-retry-connect-time" == itr.first.as<std::string>() )
		{
			_Policy.m_period_retry_connect_time = itr.second.as<int>();
		}
		else if ( "send-rule" == itr.first.as<std::string>() )
		{
			_Policy.m_SendRule = itr.second.as<std::string>();
		}
	}
}

bool Load_yaml( __in const char *_pszPath_Conf, __out DATA_POLICY &_Policy )
{
	if( 0 != access( _pszPath_Conf, 0 ) )
		return false;

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

	if ( node["send-rule"] )
		Parse_SendRule( node["send-rule"], _Policy );
	else
	{
		std::cout << "not exist section => send-rule" << std::endl;
		return false;
	}

	if ( node["destination"] )
		Parse_Destination( node["destination"], _Policy );

	for(auto &data : _Policy.m_vecRoudRobin)
	{
		std::cout << "[SEND-RULE][ROUND-ROBIN][VALUE = " << data << "]" << std::endl;
	}
	for(auto &data : _Policy.m_mapFailOver_IP_Port)
	{
		for(auto &ip : data.second)
			std::cout << "[SEND-RULE][FAIL-OVER][KEY = " << data.first <<  "][VALUE = " << ip << "]" << std::endl;
	}
	std::cout << "[SEND-RULE][FAIL-OVER][KEY =   active][VALUE = " << _Policy.m_vecFailOver_Session[INDEX_ACTIVE] << "]" << std::endl;
	std::cout << "[SEND-RULE][FAIL-OVER][KEY = stand-by][VALUE = " << _Policy.m_vecFailOver_Session[INDEX_STANDBY] << "]" << std::endl;

	for(auto &data : _Policy.m_mapFailBack_IP_Port)
	{
		for(auto &ip : data.second)
			std::cout << "[SEND-RULE][FAIL-BACK][KEY = " << data.first <<  "][VALUE = " << ip << "]" << std::endl;
	}
	std::cout << "[SEND-RULE][FAIL-BACK][KEY =   active][VALUE = " << _Policy.m_vecFailBack_Session[INDEX_ACTIVE] << "]" << std::endl;
	std::cout << "[SEND-RULE][FAIL-BACK][KEY = stand-by][VALUE = " << _Policy.m_vecFailBack_Session[INDEX_STANDBY] << "]" << std::endl;

	std::cout << "[SEND-RULE][DESTINATION][CONNECT TIME = " << _Policy.m_period_retry_connect_time << "]" << std::endl;
	std::cout << "[SEND-RULE][DESTINATION][SEND-RULE = " << _Policy.m_SendRule << "]" << std::endl;

	return true;
}

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

void Run_Client( 	__in TCP_ClientMgr &_ClientMgr, 
					__in DATA_POLICY &_Policy,
					__out std::thread &_Thr_Clinet )
{
	// 분석 엔진 등록 
	if( "round-robin" == _Policy.m_SendRule )
	{
		for(auto &data : _Policy.m_vecRoudRobin)
		{
			std::string strIP, strPort;
			if( true == Split_IP_Port(data, strIP, strPort) )
				_ClientMgr.Add_Eng_RoundRobin( strIP.c_str(), strPort.c_str() );
				//std::cout << "[Add Eng RoundRobin] IP = " << strIP << " | Port " << strPort << std::endl;
		}
	}
	else if( "fail-over" == _Policy.m_SendRule )
	{
	}
	else if( "fail-back" == _Policy.m_SendRule )
	{
	}

	// // _ClientMgr.Add_Eng("172.17.0.2", 4444);
	// // _ClientMgr.Add_Eng("172.17.0.3", 5555);
	// _ClientMgr.Add_Eng_Failover_Active("172.17.0.2", 4444);
	// _ClientMgr.Add_Eng_Failover_Standby("172.17.0.3", 5555);

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
	TCP_ClientMgr clientMgr(&logQ, &Policy);
	TCP_Mgr<TCP_Session> serverMgr(&logQ, &Policy);

	std::thread thr_client;
	Run_Client( clientMgr, Policy, thr_client );

	Run_Server( serverMgr );

	Stop_All( clientMgr, serverMgr, thr_client, logQ );

	google::protobuf::ShutdownProtobufLibrary();

    return 0;
}