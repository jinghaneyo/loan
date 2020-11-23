#ifndef MAX_MESSAGE_LEN
#define MAX_MESSAGE_LEN		1024	
#endif

#ifndef VER_MAJOR
#define VER_MAJOR			"1.0.0"
#endif

#ifndef VER_MINOR
#define VER_MINOR			"20.11.19"
#endif

#include <glog/logging.h>

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

#ifndef DEF_PTOTOCOL
#define DEF_PTOTOCOL
enum class PROTOCOL
{
	TCP,
	UDP,
	MAX
};
#endif