#ifndef TCP_MGR_H_
#define TCP_MGR_H_

#include <fstream>
#include <mutex>
#include "Data_Policy.hpp"
#include "Rigi_TCPServerMgr.hpp"
#include "Data_Policy.hpp"
#include "Data_Packet.hpp"
#include "MsgLog_Q.hpp"

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
	std::map<std::string, DATA_POLICY> m_mapPolicy;

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

		DATA_POLICY conf;
		if( false == Load_ConfJson( strPath.c_str(), conf ) )
			return false;

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

	const std::map<std::string, DATA_POLICY> * GetPolicy()
	{
		return &m_mapPolicy;
	}

	bool Load_ConfJson( __in const char *_pszPath, __out DATA_POLICY &conf )
	{
		return true;

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