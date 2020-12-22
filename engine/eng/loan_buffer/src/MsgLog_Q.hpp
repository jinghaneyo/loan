#ifndef MSGLOG_Q_H_
#define MSGLOG_Q_H_

#include <mutex>
#include <deque>
#include "loan.pb.h"

typedef std::deque<std::string *>	DEQUE_MSG_LOG_PTR;

class MsgLog_Q
{
public:
	MsgLog_Q() 
	{
		m_nStart_Q = 1;	
		m_nLimit_Q = 9000;
		m_nFull_Q  = 10000;
	}
	~MsgLog_Q() {};
private:
	std::mutex  m_LockQueue;
	// key => ip + port
	std::map<std::string, DEQUE_MSG_LOG_PTR *> m_mapQueue;
	int			m_nStart_Q;	
	int			m_nLimit_Q;
	int			m_nFull_Q;
public:
	// 일반 함수
	void Clear_Q()
	{
		const std::lock_guard<std::mutex> lock(m_LockQueue);

		for(auto &pDeque: m_mapQueue)
		{
			while ( false == pDeque.second->empty()  )
			{
				auto *ptr = pDeque.second->front();
				delete ptr;

				pDeque.second->pop_front();
			}
			delete pDeque.second;
		}
		m_mapQueue.clear();
	}
	
	void Push_Data( __in const char * _szKey, __in std::string *_pData )
	{
		const std::lock_guard<std::mutex> lock(m_LockQueue);

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
	}

	std::string * Pop_Data( __in const char *_pszKey )
	{
		std::string *pRet = nullptr;

		const std::lock_guard<std::mutex> lock(m_LockQueue);

		auto find = m_mapQueue.find( _pszKey );
		if(find != m_mapQueue.end())
		{
			if(false == find->second->empty())
			{
				pRet = find->second->front();
				find->second->pop_front();
			}
		}

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
};

#define MSG_TYPE_GEN 	"MSG_TYPE_GEN"

#endif