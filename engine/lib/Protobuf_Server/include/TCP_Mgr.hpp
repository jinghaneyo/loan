#ifndef TCP_MGR_H_
#define TCP_MGR_H_

#include <fstream>
#include <mutex>
#include "../data/Data_Policy.hpp"
#include "Rigi_TCPServerMgr.hpp"
#include "../data/MsgLog_Q.hpp"

template <typename T>
class TCP_Mgr : public Rigitaeda::Rigi_TCPServerMgr<T>
{
public:
	TCP_Mgr( __in MsgLog_Q *_pLogQ, __in DATA_POLICY *_pPolicy ) : m_pLogQ(_pLogQ), m_pPolicy(_pPolicy)
	{
	}
	virtual ~TCP_Mgr()
	{
		m_pLogQ = nullptr;
		m_pPolicy = nullptr;
	}

private:
	MsgLog_Q *m_pLogQ;
	DATA_POLICY *m_pPolicy;

public:
	// ------------------------------------------------------------------
	// 이벤트 함수
	// false로 리턴 시 종료 된다
	bool OnEvent_Init()
	{
		if(nullptr == m_pLogQ)
		{
			//assert(0 && "[TCP_Mgr::OnEvent_Init] m_pLogQ is nullptr");
			return false;
		}

		char szPath[1024] = {0,};
		if (getcwd(szPath, sizeof(szPath)) == NULL) 
		{
			fprintf(stderr, "getcwd error\n");
			exit(1);
		}
		std::string strPath = szPath;
		strPath += "/conf.json";

		std::cout << "[" << __FUNCTION__ << "][CURRENT PATH] >> " << strPath << std::endl;

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

	DATA_POLICY * Get_Policy()	{	return m_pPolicy;	};
};

#endif