#include <iostream>
#include "Conf_Yaml.hpp"

#ifdef _WIN32
   #include <io.h> 
   #define access    _access_s
#else
   #include <unistd.h>
#endif

Conf_Yaml::Conf_Yaml()
{
}

Conf_Yaml::~Conf_Yaml()
{
}

void Conf_Yaml::Parse_Server( __in const YAML::Node &_node, __out DATA_POLICY *_pPolicy )
{
	for( auto itr : _node )
	{
		if ( "protocol" == itr.first.as<std::string>() )
		{
			_pPolicy->m_ServerInfo.strProtocol = itr.second.as<std::string>();
			std::cout << _pPolicy->m_ServerInfo.strProtocol << std::endl;
		}
		else if ( "port" == itr.first.as<std::string>() )
		{
			_pPolicy->m_ServerInfo.strPort = itr.second.as<std::string>();
			std::cout << _pPolicy->m_ServerInfo.strPort << std::endl;
		}
	}
}

void Conf_Yaml::Parse_Group( __in const YAML::Node &_node, __out std::map<std::string, std::string> &_mapOut )
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

void Conf_Yaml::Parse_SendRule( 	__in const YAML::Node &_node, __out DATA_POLICY *_pPolicy )
{
	for( auto &itr : _node )
	{
		if ( "round-robin" == itr.first.as<std::string>() )
		{
			for( auto &data : itr.second )
				_pPolicy->m_vecRoudRobin.emplace_back( data.as<std::string>() );
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
							_pPolicy->m_vecFailOver_Change_Limit[INDEX_ACTIVE] = chg.second.as<int>();
						else if( "stand-by" == chg.first.as<std::string>() )
							_pPolicy->m_vecFailOver_Change_Limit[INDEX_STANDBY] = chg.second.as<int>();
					}
				}
				else if( "active" == data.first.as<std::string>() || "stand-by" == data.first.as<std::string>() )
				{
					for( auto &ip : data.second )
					{
						auto find = _pPolicy->m_mapFailOver_IP_Port.find( data.first.as<std::string>() );
						if(find == _pPolicy->m_mapFailOver_IP_Port.end())
						{
							std::vector<std::string> vecIP;
							vecIP.emplace_back( ip.as<std::string>() );
							_pPolicy->m_mapFailOver_IP_Port.insert( std::make_pair(data.first.as<std::string>(), vecIP) );
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
							_pPolicy->m_vecFailBack_Change_Limit[INDEX_ACTIVE] = chg.second.as<int>();
						else if( "stand-by" == chg.first.as<std::string>() )
							_pPolicy->m_vecFailBack_Change_Limit[INDEX_STANDBY] = chg.second.as<int>();
					}
				}
				else if( "active" == data.first.as<std::string>() || "stand-by" == data.first.as<std::string>() )
				{
					for( auto &ip : data.second )
					{
						auto find = _pPolicy->m_mapFailBack_IP_Port.find( data.first.as<std::string>() );
						if(find == _pPolicy->m_mapFailBack_IP_Port.end())
						{
							std::vector<std::string> vecIP;
							vecIP.emplace_back( ip.as<std::string>() );
							_pPolicy->m_mapFailBack_IP_Port.insert( std::make_pair(data.first.as<std::string>(), vecIP) );
						}
						else
							find->second.emplace_back( ip.as<std::string>() );
					}
				}
			}
		}
		else if ( "save-file" == itr.first.as<std::string>() )
		{
			_pPolicy->m_strSavePath_Pattern = itr.second.as<std::string>();
			std::cout << "[Parse_SendRule] save-file = " << _pPolicy->m_strSavePath_Pattern << std::endl;
		}
	}
}

void Conf_Yaml::Parse_LogService( 	__in const YAML::Node &_node, __out DATA_POLICY *_pPolicy )
{
	for( auto &itr : _node )
	{
		for( auto &service : itr.second )
		{
			//std::cout << "[LOG SERVICE 2][" << itr.first.as<std::string>() << "][" << service.first.as<std::string>() << "][" << std::endl;

			if( true == itr.first.as<std::string>().empty() )
				continue;

			MAP_LOG_CONFIG mapConfig;

			for( auto &config : service.second )
			{
				//std::cout << "[LOG SERVICE 2][" << itr.first.as<std::string>() << "][" << service.first.as<std::string>() << "][" << config.first.as<std::string>() << "][" << config.second.as<std::string>() << std::endl;

				if( false == config.first.as<std::string>().empty() && false == config.second.as<std::string>().empty() )
				{
					auto find = mapConfig.find( config.first.as<std::string>() );
					if( find == mapConfig.end() )
						mapConfig.insert( std::make_pair( config.first.as<std::string>(), config.second.as<std::string>()) );
					else
						find->second = config.second.as<std::string>();
				}
			}

			if(false == mapConfig.empty())
			{
				auto find = _pPolicy->m_mapLogService.find( itr.first.as<std::string>() );
				if( find == _pPolicy->m_mapLogService.end() )
				{
					VEC_LOG_CONFIG vecConfig;
					vecConfig.emplace_back(mapConfig);

					_pPolicy->m_mapLogService.insert( std::make_pair( itr.first.as<std::string>(), vecConfig ) );
				}
				else
				{
					find->second.emplace_back( mapConfig );
				}
			}
		}
	}
}

void Conf_Yaml::Parse_Destination( __in const YAML::Node &_node, __out DATA_POLICY *_pPolicy )
{
	for( auto &itr : _node )
	{
		if ( "period-retry-connect-time" == itr.first.as<std::string>() )
		{
			_pPolicy->m_period_retry_connect_time = itr.second.as<int>();
		}
		else if ( "send-rule" == itr.first.as<std::string>() )
		{
			_pPolicy->m_SendRule = itr.second.as<std::string>();
		}
	}
}

bool Conf_Yaml::Load_yaml( __in const char *_pszPath_Conf, __out DATA_POLICY *_pPolicy )
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
		Parse_Server( node["server"], _pPolicy );

	std::map<std::string, std::string> mapGroup;
	if ( node["group"] )
		Parse_Group( node["group"], mapGroup );

	if ( node["send-rule"] )
		Parse_SendRule( node["send-rule"], _pPolicy );

	if ( node["log-service"] )
		Parse_LogService( node["log-service"], _pPolicy );

	if ( node["destination"] )
		Parse_Destination( node["destination"], _pPolicy );

/*
	for(auto &data : _pPolicy->m_vecRoudRobin)
	{
		std::cout << "[SEND-RULE][ROUND-ROBIN][VALUE = " << data << "]" << std::endl;
	}
	for(auto &data : _pPolicy->m_mapFailOver_IP_Port)
	{
		for(auto &ip : data.second)
			std::cout << "[SEND-RULE][FAIL-OVER][KEY = " << data.first <<  "][VALUE = " << ip << "]" << std::endl;
	}
	std::cout << "[SEND-RULE][FAIL-OVER][KEY =   active][VALUE = " << _pPolicy->m_vecFailOver_Change_Limit[INDEX_ACTIVE] << "]" << std::endl;
	std::cout << "[SEND-RULE][FAIL-OVER][KEY = stand-by][VALUE = " << _pPolicy->m_vecFailOver_Change_Limit[INDEX_STANDBY] << "]" << std::endl;

	for(auto &data : _pPolicy->m_mapFailBack_IP_Port)
	{
		for(auto &ip : data.second)
			std::cout << "[SEND-RULE][FAIL-BACK][KEY = " << data.first <<  "][VALUE = " << ip << "]" << std::endl;
	}
	std::cout << "[SEND-RULE][FAIL-BACK][KEY =   active][VALUE = " << _pPolicy->m_vecFailBack_Change_Limit[INDEX_ACTIVE] << "]" << std::endl;
	std::cout << "[SEND-RULE][FAIL-BACK][KEY = stand-by][VALUE = " << _pPolicy->m_vecFailBack_Change_Limit[INDEX_STANDBY] << "]" << std::endl;

	std::cout << "[SEND-RULE][DESTINATION][CONNECT TIME = " << _pPolicy->m_period_retry_connect_time << "]" << std::endl;

	for(auto &mapLog : _pPolicy->m_mapLogService)
	{
		for(auto &vec : mapLog.second)
		{
			for(auto &config : vec )
			{
				std::cout << "[LOG SERVICE][" << mapLog.first << "][" << mapLog.first 
				<< "][KEY = " << config.first 
				<< "][VALUE = " << config.second 
				<< "]" << std::endl;
			}
		}
	}
	//*/

	std::cout << "[SEND-RULE][DESTINATION][SEND-RULE = " << _pPolicy->m_SendRule << "]" << std::endl;

	return true;
}