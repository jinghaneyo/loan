#ifndef MSGLOG_Q_H_
#define MSGLOG_Q_H_

#include <mutex>
#include <deque>
#include "../protobuf/loan.pb.h"

struct _MsgLog_Data
{
	bool bProtobuf;
	std::string *pstrLog;

	_MsgLog_Data()
	{
		bProtobuf = false;
		pstrLog = nullptr;
	}
	virtual ~_MsgLog_Data()
	{
		// 자동 소멸이아닌 직접 삭제를 해주도록 주석 처리
		// if(nullptr != pstrLog)
		// 	delete pstrLog;
	}

	void Clear()
	{
		if(nullptr != pstrLog)
			delete pstrLog;
	}
};

typedef std::deque<_MsgLog_Data *>	DEQUE_MSG_LOG_PTR;

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
	DEQUE_MSG_LOG_PTR	m_DeQueue;
	int			m_nStart_Q;	
	int			m_nLimit_Q;
	int			m_nFull_Q;
public:
	// 일반 함수
	void Clear_Q()
	{
		const std::lock_guard<std::mutex> lock(m_LockQueue);

		while ( false == m_DeQueue.empty()  )
		{
			auto *ptr = m_DeQueue.front();
			delete ptr;

			m_DeQueue.pop_front();
		}
	}
	
	void Push_back( __in std::string *_pData, __in bool _bProtobuf )
	{
		const std::lock_guard<std::mutex> lock(m_LockQueue);
		_MsgLog_Data *pLog = new _MsgLog_Data();
		pLog->pstrLog = _pData;
		if(true == _bProtobuf)
			pLog->bProtobuf = true;
		else
			pLog->bProtobuf = false;

		m_DeQueue.push_back(pLog);
	}

	void Push_front( __in std::string *_pData, __in bool _bProtobuf )
	{
		const std::lock_guard<std::mutex> lock(m_LockQueue);
		_MsgLog_Data *pLog = new _MsgLog_Data();
		pLog->pstrLog = _pData;
		if(true == _bProtobuf)
			pLog->bProtobuf = true;
		else
			pLog->bProtobuf = false;
		m_DeQueue.push_front(pLog);
	}

	_MsgLog_Data * Pop_front()
	{
		_MsgLog_Data *pRet = nullptr;

		const std::lock_guard<std::mutex> lock(m_LockQueue);
		if( true == m_DeQueue.empty()  )
			return nullptr;

		pRet = m_DeQueue.front();
		m_DeQueue.pop_front();

		return pRet;
	}

	size_t GetQ_Size()
	{
		const std::lock_guard<std::mutex> lock(m_LockQueue);
		return (int)m_DeQueue.size();
	}

	bool QEmpty()
	{
		const std::lock_guard<std::mutex> lock(m_LockQueue);
		return m_DeQueue.empty();
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