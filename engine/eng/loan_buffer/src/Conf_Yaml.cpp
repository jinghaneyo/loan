#include <iostream>
#include "Conf_Yaml.hpp"

Conf_Yaml::Conf_Yaml()
{
}

Conf_Yaml::~Conf_Yaml()
{
}

void Conf_Yaml::Parse_Server( __in const YAML::Node &_node, __out DATA_POLICY &_Policy )
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

void Conf_Yaml::Parse_SendRule( 	__in const YAML::Node &_node, __out DATA_POLICY &_Policy )
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

void Conf_Yaml::Parse_Destination( __in const YAML::Node &_node, __out DATA_POLICY &_Policy )
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

bool Conf_Yaml::Load_yaml( __in const char *_pszPath_Conf, __out DATA_POLICY &_Policy )
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