// regex 헤더 안에 이미 __in, __out 정의 되어있다
// 해서 Rigi_Def.hpp 보다 먼저 선언을 해야 한다
#include <regex>
#include "TCP_ClientMgr.hpp"

TCP_ClientMgr::TCP_ClientMgr() 
{ 
}

TCP_ClientMgr::~TCP_ClientMgr() 
{ 
}

void TCP_ClientMgr::Async_Run( __in char *_pszServerIP, __in int _nPort )
{
	Rigitaeda::Rigi_ClientTCP *pSession = Connect_Session(_pszServerIP, _nPort);
}

void TCP_ClientMgr::Await_Run( __in char *_pszServerIP, __in int _nPort )
{
	Rigitaeda::Rigi_ClientTCP *pSession = Connect_Session(_pszServerIP, _nPort);
}

Rigitaeda::Rigi_ClientTCP * TCP_ClientMgr::Connect_Session( __in char *_pszServerIP, __in int _nPort )
{
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
