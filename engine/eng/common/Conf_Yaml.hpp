#ifndef CONF_YAML_H_
#define CONF_YAML_H_

#include <yaml-cpp/yaml.h>
#include "Data_Policy.hpp"

#ifndef _WIN32
	#ifndef __in
	#define __in
	#endif

	#ifndef __inout
	#define __inout
	#endif

	#ifndef __out
	#define __out
	#endif
#endif

#ifndef INDEX_ACTIVE
#define INDEX_ACTIVE	0
#endif

#ifndef INDEX_STANDBY
#define INDEX_STANDBY	1	
#endif

class Conf_Yaml
{
public:
	Conf_Yaml();
	~Conf_Yaml();

	static void Parse_Server( __in const YAML::Node &_node, __out DATA_POLICY *_pPolicy );
	static void Parse_Group( __in const YAML::Node &_node, __out std::map<std::string, std::string> &_mapOut );
	static void Parse_SendRule( __in const YAML::Node &_node, __out DATA_POLICY *_pPolicy );
	static void Parse_Destination( __in const YAML::Node &_node, __out DATA_POLICY *_pPolicy );
	static void Parse_LogService( __in const YAML::Node &_node, __out DATA_POLICY *_pPolicy );
	static bool Load_yaml( __in const char *_pszPath_Conf, __out DATA_POLICY *_pPolicy );
};

#endif