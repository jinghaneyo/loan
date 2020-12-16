// regex 헤더 안에 이미 __in, __out 정의 되어있다
// 해서 Rigi_Def.hpp 보다 먼저 선언을 해야 한다
#include <regex>
#include <thread>
#include "TCP_ClientMgr.hpp"

TCP_ClientMgr::TCP_ClientMgr() 
{ 
}

TCP_ClientMgr::~TCP_ClientMgr() 
{ 
}

void TCP_ClientMgr::Async_Run()
{
	if(nullptr == m_pLogQ)
	{
		assert(0 && "[TCP_ClientMgr::Async_Run] m_pLogQ is nullptr");
		return ;
	}

	std::thread thr( [&]()
	{
		// 접속 후 소켓이 close 될때까지 대기 한다
		while(true)
		{
			const std::lock_guard<std::mutex> lock(m_mtxSession);
			for(auto &pSession : m_mapSessionPool)
			{
				//loan::MsgLog *pLog = m_pLogQ->Pop_Data(pSession->first.c_str());
				std::string *pLog = m_pLogQ->Pop_Data(pSession.first.c_str());
				if(nullptr != pLog)
					pSession.second->SendPacket( pLog->c_str(), pLog->length() );
			}
		}
	});
	thr.detach();
}

void TCP_ClientMgr::Await_Run()
{
	if(nullptr == m_pLogQ)
	{
		assert(0 && "[TCP_ClientMgr::Await_Run] m_pLogQ is nullptr");
		return ;
	}
}

Rigitaeda::Rigi_ClientTCP * TCP_ClientMgr::Connect_Session( __in const char *_pszServerIP, __in int _nPort )
{
	const std::lock_guard<std::mutex> lock(m_mtxSession);
	
	std::string strIP_Port = _pszServerIP;
	strIP_Port += ":";
	strIP_Port += std::to_string(_nPort);

	auto find = m_mapSessionPool.find(strIP_Port);
	if(find != m_mapSessionPool.end())
		return find->second;

	Rigitaeda::Rigi_ClientTCP *pClient = new Rigitaeda::Rigi_ClientTCP();
	pClient->Connect( _pszServerIP, _nPort, m_io_service );

	m_mapSessionPool.insert(std::make_pair(strIP_Port, pClient));
	return pClient;
}

Rigitaeda::Rigi_ClientTCP * TCP_ClientMgr::Get_Session( __in const char *_pszServerIP, __in int _nPort )
{
	std::string strIP_Port = _pszServerIP;
	strIP_Port += ":";
	strIP_Port += std::to_string(_nPort);

	auto find = m_mapSessionPool.find(strIP_Port);
	if(find != m_mapSessionPool.end())
		return find->second;

	return nullptr;
}

bool TCP_ClientMgr::Add_Analyzer_Eng( __in const char *_pszServerIP, __in int _nPort )
{
	const std::lock_guard<std::mutex> lock(m_mtxSession);
	
	std::string strIP_Port = _pszServerIP;
	strIP_Port += ":";
	strIP_Port += std::to_string(_nPort);

	auto find = m_mapSessionPool.find(strIP_Port);
	if(find != m_mapSessionPool.end())
		return false;

	Rigitaeda::Rigi_ClientTCP *pClient = new Rigitaeda::Rigi_ClientTCP();
	pClient->Connect( _pszServerIP, _nPort, m_io_service );

	m_mapSessionPool.insert(std::make_pair(strIP_Port, pClient));
	return true;
}

bool TCP_ClientMgr::Del_Analyzer_Eng( __in const char *_pszServerIP, __in int _nPort )
{
	const std::lock_guard<std::mutex> lock(m_mtxSession);
	
	std::string strIP_Port = _pszServerIP;
	strIP_Port += ":";
	strIP_Port += std::to_string(_nPort);

	auto find = m_mapSessionPool.find(strIP_Port);
	if(find == m_mapSessionPool.end())
		return false;

	m_mapSessionPool.erase(find);
	return true;
}

bool TCP_ClientMgr::Chg_Analyzer_Eng( 	__in const char *_pszServerIP_Old, __in int _nPort_Old,
										__in const char *_pszServerIP_New, __in int _nPort_New )
{
	const std::lock_guard<std::mutex> lock(m_mtxSession);
	
	std::string strIP_Port_Old = _pszServerIP_Old;
	strIP_Port_Old += ":";
	strIP_Port_Old += std::to_string(_nPort_Old);

	auto find = m_mapSessionPool.find(strIP_Port_Old);
	if(find == m_mapSessionPool.end())
		return false;

	Rigitaeda::Rigi_ClientTCP *pClient = find->second;

	m_mapSessionPool.erase(find);

	std::string strIP_Port_New = _pszServerIP_New;
	strIP_Port_New += ":";
	strIP_Port_New += std::to_string(_nPort_New);

	m_mapSessionPool.insert( std::make_pair(strIP_Port_New, pClient) );
	return true;
}

void TCP_ClientMgr::Set_LogQ( __in MsgLog_Q *_pLogQ )
{
	m_pLogQ = _pLogQ;
}
