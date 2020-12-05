#include "Rigi_SessionPool.hpp"

using namespace Rigitaeda;

Rigi_SessionPool::Rigi_SessionPool()
{
	m_eTypeOutput = _OUTPUT_TYPE::LOG;
}

Rigi_SessionPool::~Rigi_SessionPool()
{
	Clear();
}

void Rigi_SessionPool::Set_MaxClient( __in int _nMaxClient )
{
	m_nMaxClient = _nMaxClient;
}

Rigi_TCPSession * Rigi_SessionPool::Add_Session( __in SOCKET_TCP *_pSocket )
{
	Rigi_TCPSession *pSession = new Rigi_TCPSession();

	auto find = m_mapTCP.find(pSession);
	if(find == m_mapTCP.end())
	{
		pSession->SetSocket(_pSocket);
		pSession->SetSessionPool(this);

		m_mapTCP.insert( std::make_pair(pSession, pSession) );

		if (m_nMaxClient < (int)m_mapTCP.size())
		{
			char szClose[] = "Connection Full !!";
			std::cout << "[ACCEPT] >> " << szClose << std::endl;
			pSession->Send(szClose, sizeof(szClose));
			pSession->Close();
			delete pSession;

			return nullptr;
		}
		else
		{
			std::string strClientIP = pSession->GetIP_Remote();
			LOG(INFO) << "[ACCEPT] " << strClientIP;

			pSession->Async_Receive();
			return pSession;
		}
	}
	else
	{
		delete pSession;
	}

	return nullptr;
}

bool Rigi_SessionPool::Close_Session( __in Rigi_TCPSession *_pSession )
{
	auto find = m_mapTCP.find(_pSession);
	if(find == m_mapTCP.end())
		return false;

	delete find->second;
	m_mapTCP.erase(find);

	return true;
}

// void Rigi_SessionPool::Add_Session( __in Rigi_UDPSession *_pSession )
// {
// 	auto find = m_mapUDP.find(_pSession);
// 	if(find == m_mapUDP.end())
// 	{
// 		m_mapUDP.insert( std::make_pair(_pSession, _pSession) );

// 		return true;
// 	}

// 	return false;
// }

// void Rigi_SessionPool::Close_Session( __in Rigi_UDPSession *_pSession )
// {
// 	auto find = m_mapUDP.find(_pSession);
// 	if(find == m_mapUDP.end())
// 		return false;

// 	delete find.second;
// 	m_mapUDP.erase(find);

// 	return true;
// }

void Rigi_SessionPool::Clear()
{
	for(auto &sock : m_mapTCP)
		delete sock.second;
	m_mapTCP.clear();

	// for(auto &sock : m_mapUDP)
	// 	delete sock.second;
	// m_mapUDP.clear();
}

void Rigi_SessionPool::Load_Conf( __in const char *_pszPath_conf )
{

}
