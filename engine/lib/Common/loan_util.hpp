#ifndef LOAN_UTIL_H_
#define LOAN_UTIL_H_

#include <string> 
#include <string.h> 
#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include <boost/system/error_code.hpp>

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

std::string Replace_All(	__in const std::string &_strSource,
							__in const char *_pstrOld,
							__in const char *_pstrNew );

std::string Get_LocalHostIP();

std::string GetCurrentTime( __in const char *_pszFormat = "%Y-%m-%d %H:%M:%S" );

std::string Replace_Macro( __in std::string &_strSource );

#endif