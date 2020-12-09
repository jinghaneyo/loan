#ifndef TCP_MGR_H_
#define TCP_MGR_H_

#include <fstream>
#include <mutex>
#include <deque>
#include "Rigi_TCPMgr.hpp"
#include "Data_Policy.hpp"
#include "Data_Packet.hpp"

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
	virtual ~TCP_Mgr()	
	{
		Clear_Q();
	}

private:
	//MAP_DATA_PTR m_mapData;
	std::map<std::string, DATA_POLICY> m_mapPolicy;

	std::mutex m_LockQueue;
	// key => ip + port
	std::map<std::string, DEQUE_DATA_PACKET_PTR *> m_mapQueue;
public:
	// ------------------------------------------------------------------
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
	// ------------------------------------------------------------------

	// 일반 함수
	void Clear_Q()
	{
		m_LockQueue.lock();

		for(auto &pData : m_mapQueue)
		{
			delete pData;
		}

		m_mapQueue.clear();

		m_LockQueue.unlock();
	}
	
	void Push_Data( __in const char * _szIP_Port, __in DATA_PACKET *_pData ) 
	{
		m_LockQueue.lock();

		auto find = m_mapQueue.find(_szKey);
		if( find == m_mapQueue.end())
		{
			VEC_DATA_PACKET_PTR *pVec = new VEC_DATA_PACKET_PTR();
			pVec->push_back(_pData);
			m_mapQueue.insert( std::make_pair(_szKey, pVec) );
		}
		else
		{
			find->second->push_back(_pData);
		}

		m_LockQueue.unlock();
	}

	DATA_PACKET * Pop_Data( __in const char *_pszIP_Port ) 
	{
		DATA_PACKET *pRet = nullptr;

		m_LockQueue.lock();

		auto find = m_mapQueue.find( _pszIP_Port );
		if(find != m_mapQueue.end())
		{
			if(false == find.second->empty())
				find.second->pop_front(pRet);
		}

		m_LockQueue.unlock();

		return pRet;
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