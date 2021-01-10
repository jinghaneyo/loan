#ifndef CONF_YAML_H_
#define CONF_YAML_H_

#ifdef _WIN32
   #include <io.h> 
   #define access    _access_s
#else
   #include <unistd.h>
#endif
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

class Conf_Yaml
{
public:
	Conf_Yaml();
	~Conf_Yaml();

	static void Parse_Server( __in const YAML::Node &_node, __out DATA_POLICY &_Policy );
	static void Parse_Group( __in const YAML::Node &_node, __out std::map<std::string, std::string> &_mapOut );
	static void Parse_SendRule( __in const YAML::Node &_node, __out DATA_POLICY &_Policy );
	static void Parse_Destination( __in const YAML::Node &_node, __out DATA_POLICY &_Policy );
	static bool Load_yaml( __in const char *_pszPath_Conf, __out DATA_POLICY &_Policy );
};

#endif