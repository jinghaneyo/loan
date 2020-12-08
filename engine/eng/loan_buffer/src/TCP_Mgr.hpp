#ifndef TCP_MGR_H_
#define TCP_MGR_H_

#include <fstream>
#include "Rigi_TCPMgr.hpp"
#include "Data_Policy.hpp"

struct Data_Q
{
public:
	Data_Q() {};
	~Data_Q() {};

	std::string strIP;
	std::string strData;
};
typedef std::string 				STR_IP;
//typedef std::map<STR_IP, Data_Q *>	MAP_DATA_PTR;

template <typename T>
class TCP_Mgr : public Rigitaeda::Rigi_TCPMgr<T>
{
public:
	TCP_Mgr()	{};
	virtual ~TCP_Mgr()	{};

private:
	//MAP_DATA_PTR m_mapData;
	std::map<std::string, DATA_POLICY> m_mapPolicy;

	std::map<std::string, std::string> m_mapQueue;
public:
	// 이벤트 함수
	// false로 리턴 시 종료 된다
	bool OnEvent_Init()
	{
		if(false == Load_Json( "conf.json" ) )
		{

			return false;
		}

		return true;
	}

	// 일반 함수
	void Push_Data( __in const char * _pszIP, __in int _nPort, __in Data_Q &data ) 
	{

	}

	void Pop_Data() 
	{

	}

	bool Is_Exist_File( __in const char *_szFilePath )
	{
		std::ifstream infile(fileName);
		return infile.good();
	}

	bool Load_Json( __in const char *_pszPath_Json )
	{
		if( false == Is_Exist_File( _pszPath_Json ) )
		{
			return false;
		}

		return true;
	}
};

#endif