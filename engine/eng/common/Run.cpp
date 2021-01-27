#include "Run.hpp"

Loan_Run::Loan_Run()
{
}

Loan_Run::~Loan_Run()
{
}

bool Loan_Run::Split_IP_Port( __in std::string &_strSource, __out std::string &_strIP, __out std::string &_strPort )
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

void Loan_Run::Add_Eng( __in TCP_ClientMgr &_ClientMgr, __in DATA_POLICY &_Policy )
{
	try
	{
		if( "round-robin" == _Policy.m_SendRule )
		{
			for(auto &data : _Policy.m_vecRoudRobin)
			{
				std::string strIP, strPort;
				if( true == Split_IP_Port(data, strIP, strPort) )
					_ClientMgr.Add_Eng_RoundRobin( strIP.c_str(), strPort.c_str() );
			}
		}
		else if( "fail-over" == _Policy.m_SendRule )
		{
			for(auto &map_vec : _Policy.m_mapFailOver_IP_Port)
			{
				for(auto &data : map_vec.second )
				{
					if("active" == map_vec.first)
					{
						std::string strIP, strPort;
						if( true == Split_IP_Port(data, strIP, strPort) )
							_ClientMgr.Add_Eng_FailOver_Active( strIP.c_str(), strPort.c_str() );
					}
					else if("stand-by" == map_vec.first)
					{
						std::string strIP, strPort;
						if( true == Split_IP_Port(data, strIP, strPort) )
							_ClientMgr.Add_Eng_FailOver_Standby( strIP.c_str(), strPort.c_str() );
					}
				}
			}
		}
		else if( "fail-back" == _Policy.m_SendRule )
		{
			for(auto &map_vec : _Policy.m_mapFailOver_IP_Port)
			{
				for(auto &data : map_vec.second )
				{
					if("active" == map_vec.first)
					{
						std::string strIP, strPort;
						if( true == Split_IP_Port(data, strIP, strPort) )
							_ClientMgr.Add_Eng_FailBack_Active( strIP.c_str(), strPort.c_str() );
					}
					else if("stand-by" == map_vec.first)
					{
						std::string strIP, strPort;
						if( true == Split_IP_Port(data, strIP, strPort) )
							_ClientMgr.Add_Eng_FailBack_Standby( strIP.c_str(), strPort.c_str() );
					}
				}
			}
		}
		else if( "save-file" == _Policy.m_SendRule )
		{
			_ClientMgr.Set_SaveFile(_Policy.m_strSavePath_Pattern.c_str());
		}
	}
	catch(std::exception &e)
	{
		std::cerr << "[Exception][MAIN] Add_Eng | Err = " << e.what() << "\n";
	}
	catch(...)
	{
		std::cout << "[Exception][MAIN] Add_Eng" << std::endl;
	}
}

void Loan_Run::Run_Client( 	__in TCP_ClientMgr &_ClientMgr, 
                            __in DATA_POLICY &_Policy,
                            __out std::thread &_Thr_Clinet,
							__in bool _bBlock )
{
	// 분석 엔진 등록 
	Add_Eng( _ClientMgr, _Policy );

	_Thr_Clinet = std::thread( [&]()
	{
		_ClientMgr.Run();
 	});

    std::cout << "[START] << ClientMgr Run" << std::endl;

	if(true == _bBlock)
		_Thr_Clinet.join();
	else
		_Thr_Clinet.detach();
}

void Loan_Run::Run_Server( 	__in TCP_ServerMgr<TCP_Session> &_ServerMgr, __in int _nPort )
{
	std::cout << "Run_Server Call" << std::endl;

	Rigitaeda::Rigi_Server server(10240);
	server.Run( _nPort, 100, &_ServerMgr );
}

void Loan_Run::Stop_All(__in TCP_ClientMgr *_pClientMgr, 
                        __in TCP_ServerMgr<TCP_Session> *_pServerMgr, 
                        __in std::thread &_Thr_Clinet,
                        __in MsgLog_Q &_LogQ )
{
	// -----------------------------------------------------
	// 종료-> 리소스 해제 
	// -----------------------------------------------------
	if(nullptr !=_pClientMgr)
	{
		_pClientMgr->Stop();
		if(true == _Thr_Clinet.joinable())
			_Thr_Clinet.join();
	}

	if(nullptr != _pServerMgr)
		_pServerMgr->Stop();

	_LogQ.Clear_Q();

    std::cout << "[FINISH] << server stop" << std::endl;
}

bool Loan_Run::Run( __in DATA_POLICY &_Policy,
					__in TCP_ClientMgr *_pClientMgr,
					__out std::thread &_Thr_client,
					__in TCP_ServerMgr<TCP_Session> *_pServerMgr,
					__in int _nBindPort )
{
	bool bBlock = false;
	if(nullptr == _pServerMgr && 1 > _nBindPort)
		bBlock = true;

	if(nullptr !=_pClientMgr )
		Run_Client( *_pClientMgr, _Policy, _Thr_client, bBlock);

	if( 1 < _nBindPort && nullptr != _pServerMgr )
		Run_Server( *_pServerMgr, _nBindPort );

	return true;
}
