#ifndef TCP_MGR_H_
#define TCP_MGR_H_

#include <fstream>
#include <mutex>
#include "Rigi_TCPServerMgr.hpp"
#include "MsgLog_Q.hpp"
#include "Policy.hpp"

template <typename T>
class TCP_ServerMgr : public Rigitaeda::Rigi_TCPServerMgr<T>
{
public:
	TCP_ServerMgr( __in MsgLog_Q *_pLogQ, __in POLICY *_pPolicy ) : m_pLogQ(_pLogQ), m_pPolicy(_pPolicy)
	{
	}
	virtual ~TCP_ServerMgr()
	{
		m_pLogQ = nullptr;
		m_pPolicy = nullptr;
	}

private:
	MsgLog_Q *m_pLogQ;
	POLICY *m_pPolicy;

public:
	// ------------------------------------------------------------------
	// 이벤트 함수
	// false로 리턴 시 종료 된다
	bool OnEvent_Init()
	{
		if(nullptr == m_pLogQ)
		{
			ASSERT(0 && "[TCP_Mgr::OnEvent_Init] m_pLogQ is nullptr");
			return false;
		}

		std::cout << "[START] << Server Run (IP = " << Get_LocalServerIP() << " | PORT = " << std::to_string( Get_Port() ) << ")" << std::endl;

		return true;
	}
	// ------------------------------------------------------------------

	void Set_LogQ( __in MsgLog_Q *_pLogQ )
	{
		m_pLogQ = _pLogQ;
	}
	MsgLog_Q * Get_LogQ()
	{
		return m_pLogQ;
	}

	bool Is_Exist_File( __in const char *_szFilePath )
	{
		std::ifstream infile(_szFilePath);
		return infile.good();
	}

	POLICY * Get_Policy()	{	return m_pPolicy;	};

	int Get_Port()
	{
        return Rigitaeda::Rigi_TCPServerMgr<T>::Get_Port() ; 
    }

	std::string Get_LocalServerIP()
	{
        return Rigitaeda::Rigi_TCPServerMgr<T>::Get_LocalServerIP() ; 
    }
};

#endif