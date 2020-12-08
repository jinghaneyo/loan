#ifndef TCP_MGR_H_
#define TCP_MGR_H_

#include "Rigi_TCPMgr.hpp"

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
	void Push_Data( __in const char * _pszIP, __in Data_Q &data ) {};
	void Pop_Data() {};

	void Load_Json( __in const char *_pszPath_Json );
};

#endif