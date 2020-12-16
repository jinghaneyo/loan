#ifndef TCP_MGR_H_
#define TCP_MGR_H_

#include <fstream>
#include <mutex>
#include <deque>
#include "Rigi_TCPMgr.hpp"
#include "Data_Policy.hpp"
#include "Data_Packet.hpp"
#include "loan.pb.h"

typedef std::deque<loan::MsgLog *>	DEQUE_MSG_LOG_PTR;

template <typename T>
class TCP_Mgr : public Rigitaeda::Rigi_TCPMgr<T>
{
public:
	TCP_Mgr()
	{
	}
	virtual ~TCP_Mgr()
	{
		Clear_Q();
	}

private:
	std::map<std::string, DATA_POLICY> m_mapPolicy;

	std::mutex  m_LockQueue;
	// key => ip + port
	std::map<std::string, DEQUE_MSG_LOG_PTR *> m_mapQueue;
	int			m_nLimit_Q;
	int			m_nFull_Q;
	int			m_nStart_Q;
public:
	// ------------------------------------------------------------------
	// 이벤트 함수
	// false로 리턴 시 종료 된다
	bool OnEvent_Init()
	{
		char szPath[1024] = {0,};
		if (getcwd(szPath, sizeof(szPath)) == NULL) 
		{
			fprintf(stderr, "getcwd error\n");
			exit(1);
		}
		std::string strPath = szPath;
		strPath += "/conf.json";

		std::cout << "[" << __FUNCTION__ << "][CURRENT PATH] >> " << strPath << std::endl;

		DATA_POLICY conf;
		if( false == Load_ConfJson( strPath.c_str(), conf ) )
			return false;

		return true;
	}
	// ------------------------------------------------------------------

	// 일반 함수
	void Clear_Q()
	{
		m_LockQueue.lock();

		// for(auto &pDeque: m_mapQueue)
		// {
		// 	while ( false == pDeque.second->empty()  )
		// 	{
		// 		auto *ptr = pDeque.second->front();
		// 		delete ptr;

		// 		pDeque.second->pop_front();
		// 	}
		// 	delete pDeque.second;
		// }
		// m_mapQueue.clear();

		m_LockQueue.unlock();
	}
	
	void Push_Data( __in const char * _szKey, __in loan::MsgLog *_pData )
	{
		m_LockQueue.lock();

		auto find = m_mapQueue.find(_szKey);
		if( find == m_mapQueue.end())
		{
			DEQUE_MSG_LOG_PTR *pDque = new DEQUE_MSG_LOG_PTR();
			pDque->push_back(_pData);
			m_mapQueue.insert( std::make_pair(_szKey, pDque) );
		}
		else
		{
			find->second->push_back(_pData);
		}

		m_LockQueue.unlock();
	}

	loan::MsgLog * Pop_Data( __in const char *_pszKey )
	{
		loan::MsgLog *pRet = nullptr;

		m_LockQueue.lock();

		auto find = m_mapQueue.find( _pszKey );
		if(find != m_mapQueue.end())
		{
			if(false == find->second->empty())
			{
				pRet = find->second->front();
				find->second->pop_front();
			}
		}

		m_LockQueue.unlock();

		return pRet;
	}

	int GetQ_Size()
	{
		return m_mapQueue.size();
	}

	bool QEmpty()
	{
		return m_mapQueue.empty();
	}

	int GetQ_LimitSize()
	{
		return m_nLimit_Q;
	}

	int GetQ_FullSize()
	{
		return m_nFull_Q;
	}

	bool Is_Exist_File( __in const char *_szFilePath )
	{
		std::ifstream infile(_szFilePath);
		return infile.good();
	}

	const std::map<std::string, DATA_POLICY> * GetPolicy()
	{
		return &m_mapPolicy;
	}

	bool Load_ConfJson( __in const char *_pszPath, __out DATA_POLICY &conf )
	{
		std::ifstream file;
		file.open(_pszPath);
		if(true == file.is_open())
		{
			std::string strBuffer;
			while ( std::getline(file, strBuffer) )
			{
				std::cout << strBuffer << std::endl;

				strBuffer = "";
			}
			file.close();
			return true;
		}

		return false;
	}
};

#endif