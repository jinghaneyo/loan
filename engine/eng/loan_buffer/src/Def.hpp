#ifndef LOAN_DEF_H_
#define LOAN_DEF_H_

#include "Rigi_ClientTCP.hpp"

// typedef std::string 	STR_IP_PORT;
// typedef std::vector<Rigitaeda::Rigi_ClientTCP *>	VEC_RIGI_SESSION;
typedef std::chrono::system_clock::time_point 		STD_TIME;

#ifndef SESSION_ACTIVE
#define SESSION_ACTIVE		0
#endif
#ifndef SESSION_STANDBY	
#define SESSION_STANDBY 	1	
#endif

#endif