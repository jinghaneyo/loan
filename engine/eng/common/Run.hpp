#ifndef LOAN_RUN_H_
#define LOAN_RUN_H_

#include <thread>
#include "TCP_ServerMgr.hpp"
#include "TCP_Session.hpp"
#include "TCP_ClientMgr.hpp"
#include "MsgLog_Q.hpp"
#include "Data_Policy.hpp"
#include "Conf_Yaml.hpp"

class Loan_Run
{
public:
	Loan_Run();
	~Loan_Run();

private:
	std::string m_strHostIP;

public:
	bool Run(	__in DATA_POLICY &_Policy,
				__in TCP_ClientMgr *_ClientMgr,
				__out std::thread &_Thr_client,
				__in TCP_ServerMgr<TCP_Session> *_ServerMgr,
				__in int _nBindPort );

	bool Split_IP_Port( __in std::string &_strSource, __out std::string &_strIP, __out std::string &_strPort );

	void Add_Eng( 	__in TCP_ClientMgr &_ClientMgr, __in DATA_POLICY &_Policy );

	void Run_Client( 	__in TCP_ClientMgr &_ClientMgr, 
						__in DATA_POLICY &_Policy,
						__out std::thread &_Thr_Clinet,
						__in bool _bBlock );

	void Run_Server( __in TCP_ServerMgr<TCP_Session> &_ServerMgr, __in int _nPort );

	void Stop_All( 	__in TCP_ClientMgr *_pClientMgr, 
					__in TCP_ServerMgr<TCP_Session> *_pServerMgr, 
					__in std::thread &_Thr_Clinet,
					__in MsgLog_Q &_LogQ );
};

#endif