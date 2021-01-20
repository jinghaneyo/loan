#include "client/TCP_ClientMgr.hpp"
#include "MsgLog_Q.hpp"
#include "Conf_Yaml.hpp"
#include <glog/logging.h>
#include <iostream>
#include <fstream>
#include <string>

bool Event_Init()
{
	return true;
}

void Event_Close()
{
}

void Event_Receive( __in char *_pData, __in size_t _nData_len )
{
	loan::MsgLog msgLog;
	msgLog.ParseFromString(_pData);

	if (false == msgLog.service_name().empty())
		std::cout << "[Event_Receive] << Service Name : " << msgLog.service_name() << " << Log contents : " << msgLog.logcontents() << std::endl;
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

void Add_Eng( 	__in TCP_ClientMgr &_ClientMgr, 
				__in DATA_POLICY &_Policy )
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
}

void Run_Client( 	__in TCP_ClientMgr &_ClientMgr, 
					__in DATA_POLICY &_Policy,
					__out std::thread &_Thr_Clinet )
{
	// 분석 엔진 등록 
	Add_Eng( _ClientMgr, _Policy );

	_Thr_Clinet = std::thread( [&]()
	{
		_ClientMgr.Run();
 	});
	_Thr_Clinet.detach();

    std::cout << "[START] << ClientMgr Run" << std::endl;
}

void Read_Tail( __in std::ifstream &_fsLog, __inout MsgLog_Q &_LogQ )
{
	try
	{
		std::string strline("");
		while (true) 
		{
			while(true)
			{
				if(getline(_fsLog, strline))
				{
					// 마지막에 0x1b 0x1b을 넣어서 라인의 끝이라고 알려주자
					strline += 0x1b;
					strline += 0x1b;

					loan::MsgLog msgLog;
					msgLog.set_msg_type("MSG_TYPE_GEN");
					msgLog.set_msg_cmd(1);
					msgLog.set_service_name("crolling");
					msgLog.set_logcontents(strline);

					std::string *pstrLine = new std::string();
					msgLog.SerializeToString(&(*pstrLine));

					_LogQ.Push_back(pstrLine);
				}
				else
					break;
			}

			if (!_fsLog.eof()) 
				break; 
				
			_fsLog.clear();
		}
	}
	catch(std::exception const& e)
	{
		std::cout << "[Exception][Read_Tail] Err = " << e.what() << std::endl;
	}
	catch(...)
	{
		std::cout << "[Exception][Read_Tail] Err = Unknown " << std::endl;
	}

	_fsLog.close();
      
	return ;
}

std::thread Run_Tail( __in const char *_pszFilePath, __inout MsgLog_Q &_LogQ )
{
	std::thread thr = std::thread( [&]() 
	{
		while(true)
		{
			std::ifstream fsLog;
			//fsLog.open( _pszFilePath, std::ios::in | std::ios::ate);
			fsLog.open( _pszFilePath, std::ios::in );

			if (!fsLog.is_open()) 
			{
				std::cout << _pszFilePath << " is not exist." << std::endl;
				sleep(1000000);
				continue;
			}

			std::cout << "[SUCC] file open (" << _pszFilePath << ")" << std::endl;
			Read_Tail(fsLog, _LogQ);
		}
	});
	thr.join();

	return thr;
}

int main( int argc, char* argv[])
{
    // google::InitGoogleLogging("DUMP");   
 	// google::SetLogDestination( google::GLOG_INFO, "./DUMP." );  
	//google::EnableLogCleaner(3);

	std::string strCurrentPath;
	char strBuffer[1024] = { 0, };
	strCurrentPath = getcwd( strBuffer, sizeof(strBuffer) );
	std::string strPath_Conf = strCurrentPath + "/conf.yaml";

	DATA_POLICY Policy;
	if(false == Conf_Yaml::Load_yaml(strPath_Conf.c_str(), &Policy) )
	{
		std::cout << "[MAIN][FAIL] Conf_Yaml::Load_yaml" << std::endl;
		return 1;
	}

	MsgLog_Q logQ;
	TCP_ClientMgr clientMgr(&logQ, &Policy);
	//Tail tail(&logQ);

	std::thread thr_client;
	Run_Client( clientMgr, Policy, thr_client );

	Run_Tail( "/share/engine/log.txt", logQ );

	clientMgr.Stop();
	thr_client.join();
	logQ.Clear_Q();

    std::cout << "[FINISH] << loan_gether stop" << std::endl;

    return 0;
}