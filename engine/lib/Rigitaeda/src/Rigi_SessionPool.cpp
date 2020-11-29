#include "Rigi_SessionPool.hpp"

using namespace Rigitaeda;

Rigi_SessionPool::Rigi_SessionPool()
{
}

Rigi_SessionPool::~Rigi_SessionPool()
{
	Clear();
}

void Rigi_SessionPool::Set_MaxClient( __in int _nMaxClient )
{
	m_nMaxClient = _nMaxClient;
}

bool Rigi_SessionPool::Add_Session( __in Rigi_TCPSession *_pSession )
{
	auto find = m_mapTCP.find(_pSession);
	if(find == m_mapTCP.end())
	{
		_pSession->SetSessionPool(this);

		m_mapTCP.insert( std::make_pair(_pSession, _pSession) );

		if (m_nMaxClient < (int)m_mapTCP.size())
		{
			char szClose[] = "Connection Full !!";
			std::cout << "[ACCEPT] >> " << szClose << std::endl;
			_pSession->Send(szClose, sizeof(szClose));
			_pSession->Close();
		}
		else
		{
			std::string strClientIP = _pSession->GetIP_Remote();
			LOG(INFO) << "[ACCEPT] " << strClientIP;

			_pSession->Async_Receive();
		}

		return true;
	}

	return false;
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