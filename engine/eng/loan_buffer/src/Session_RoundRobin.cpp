#include "Session_RoundRobin.hpp"

Session_RoundRobin::Session_RoundRobin( __in DATA_POLICY *_pPolicy, __in boost::asio::io_service *_pio_service ) : m_pio_service(_pio_service), Session_Pool(_pPolicy)
{
}

Session_RoundRobin::~Session_RoundRobin()
{
}

bool Session_RoundRobin::Add_SessionPool_Connected( __in TCP_Client *_pSession, __in int _nOption  )
{
	const std::lock_guard<std::mutex> lock(m_mtxSessionPool_Connect);

	if(true == IsExist_SessionPool_Connected(_pSession, &lock))
	{
		std::cout << "[Add_SessionPool_Connected][FAIL][Already Added] << IP = " << _pSession->Get_ServerIP() << " | Port = " << _pSession->Get_Port() << std::endl;
		return false;
	}

	m_DqSessionPool_Connect.emplace_back( _pSession );

	std::cout << "[Add_SessionPool_Connected][SUCC] << IP = " << _pSession->Get_ServerIP() << " | Port = " << _pSession->Get_Port() << std::endl;

	return true;
}

bool Session_RoundRobin::Add_SessionPool_DisConnected( __in TCP_Client *_pSession, __in int _nOption  )
{
	const std::lock_guard<std::mutex> lock(m_mtxSessionPool_DisConnect);

	if(true == IsExist_SessionPool_DisConnected(_pSession, &lock))
	{
		std::cout << "[Add_SessionPool_DisConnected][FAIL][Already Added] << IP = " << _pSession->Get_ServerIP() << " | Port = " << _pSession->Get_Port() << std::endl;
		return false;
	}

	m_DqSessionPool_DisConnect.emplace_back( _pSession );

	std::cout << "[Add_SessionPool_DisConnected][SUCC] << IP = " << _pSession->Get_ServerIP() << " | Port = " << _pSession->Get_Port() << std::endl;

	return true;
}

bool Session_RoundRobin::Del_SessionPool_Connected( __in TCP_Client *_pSession, __in int _nOption  )
{
	const char *pszIP = _pSession->Get_ServerIP().c_str();
	const char *pszPort = _pSession->Get_Port().c_str();

	const std::lock_guard<std::mutex> lock(m_mtxSessionPool_Connect);

	auto itr = m_DqSessionPool_Connect.begin();
	while(itr != m_DqSessionPool_Connect.end())
	{
		TCP_Client * pSe = *itr;
		if( (pSe->Get_ServerIP() == pszIP) && (pSe->Get_Port() == pszPort) )
		{
			itr = m_DqSessionPool_Connect.erase(itr);
			std::cout << "[Del_SessionPool_Connected][SUCC] << IP = " << pszIP << " | Port = " << pszPort << std::endl;
			return true;
		}

		itr++;
	}

	return false;
}

bool Session_RoundRobin::Del_SessionPool_DisConnected( __in TCP_Client *_pSession, __in int _nOption  )
{
	const char *pszIP = _pSession->Get_ServerIP().c_str();
	const char *pszPort = _pSession->Get_Port().c_str();

	const std::lock_guard<std::mutex> lock(m_mtxSessionPool_DisConnect);

	auto itr = m_DqSessionPool_DisConnect.begin();
	while(itr != m_DqSessionPool_DisConnect.end())
	{
		TCP_Client * pSe = *itr;
		if( (pSe->Get_ServerIP() == pszIP ) && (pSe->Get_Port() == pszPort) )
		{
			itr = m_DqSessionPool_DisConnect.erase(itr);
			std::cout << "[Del_SessionPool_DisConnected][SUCC] << IP = " << pszIP << " | Port = " << pszPort << std::endl;
			return true;
		}

		itr++;
	}

	return false;
}

// 3번째 인자에 mutex 를 받으므로써 호출하는 곳에서 강제로 mutx gaurd를 호출하도록 한다
bool Session_RoundRobin::IsExist_SessionPool_Connected( __in TCP_Client *_pSession,
														__in const std::lock_guard<std::mutex> *_pGarud )
{
	const char *pszIP = _pSession->Get_ServerIP().c_str();
	const char *pszPort = _pSession->Get_Port().c_str();

	// 이미 등록이 되어 있는지 확인
	if( nullptr == _pGarud )
	{
		std::lock_guard<std::mutex> garud(m_mtxSessionPool_Connect);

		auto itr = m_DqSessionPool_Connect.begin();
		while(itr != m_DqSessionPool_Connect.end())
		{
			TCP_Client *pSession = *itr;
				std::cout << "[IsExist_SessionPool_Connected] << IP = " << pSession->Get_ServerIP() << " | Port = " << pSession->Get_Port() << std::endl;
			if( (pSession->Get_ServerIP() == pszIP) && (pSession->Get_Port() == pszPort) )
			{
				return true;
			}
			itr++;
		}
	}
	else
	{
		auto itr = m_DqSessionPool_Connect.begin();
		while(itr != m_DqSessionPool_Connect.end())
		{
			TCP_Client *pSession = *itr;
				std::cout << "[IsExist_SessionPool_Connected] << IP = " << pSession->Get_ServerIP() << " | Port = " << pSession->Get_Port() << std::endl;
			if( (pSession->Get_ServerIP() == pszIP) && (pSession->Get_Port() == pszPort) )
			{
				return true;
			}
			itr++;
		}
	}

	return false;
}

// 3번째 인자에 mutex 를 받으므로써 호출하는 곳에서 강제로 mutx gaurd를 호출하도록 한다
bool Session_RoundRobin::IsExist_SessionPool_DisConnected( 	__in TCP_Client *_pSession,
															__in const std::lock_guard<std::mutex> *_pGarud )
{
	const char *pszIP = _pSession->Get_ServerIP().c_str();
	const char *pszPort = _pSession->Get_Port().c_str();

	// 이미 등록이 되어 있는지 확인
	if( nullptr == _pGarud )
	{
		std::lock_guard<std::mutex> garud(m_mtxSessionPool_DisConnect);

		auto itr = m_DqSessionPool_DisConnect.begin();
		while(itr != m_DqSessionPool_DisConnect.end())
		{
			TCP_Client *pSession = *itr;
			if( (pSession->Get_ServerIP() == pszIP) && (pSession->Get_Port() == pszPort) )
				return true;
			itr++;
		}
	}
	else
	{
		// 이미 등록이 되어 있는지 확인
		auto itr = m_DqSessionPool_DisConnect.begin();
		while(itr != m_DqSessionPool_DisConnect.end())
		{
			TCP_Client *pSession = *itr;
			if( (pSession->Get_ServerIP() == pszIP) && (pSession->Get_Port() == pszPort) )
				return true;
			itr++;
		}
	}

	return false;
}

TCP_Client * Session_RoundRobin::Get_Send_Session()
{
	const std::lock_guard<std::mutex> lock1(m_mtxSessionPool_Connect);
	if( true == m_DqSessionPool_Connect.empty())
		return nullptr;

	auto itr = m_DqSessionPool_Connect.begin();
	TCP_Client * pSession = *itr;

	// 1개만 남을땐 굳이 돌릴 필요 없다
	if( 1 == (int)m_DqSessionPool_Connect.size())
		return pSession;

	// 처음걸 뽑아서 리턴해주고 제일 뒤로 돌린다
	// 순환 구조
	m_DqSessionPool_Connect.erase(itr);
	m_DqSessionPool_Connect.emplace_back(pSession);

	std::cout << "[GetSession_Round_Robin] << IP = " << pSession->Get_SessionIP() << " | Port = " << pSession->Get_Port() << std::endl;

	return pSession;
}

void Session_RoundRobin::Clear()
{
	const std::lock_guard<std::mutex> lock1(m_mtxSessionPool_Connect);
	const std::lock_guard<std::mutex> lock2(m_mtxSessionPool_DisConnect);

	auto itr = m_DqSessionPool_Connect.begin();
	while(itr != m_DqSessionPool_Connect.end())
	{
		TCP_Client * pSession = *itr;
		delete pSession;
		itr++;
	}
	m_DqSessionPool_Connect.clear();

	itr = m_DqSessionPool_DisConnect.begin();
	while(itr != m_DqSessionPool_DisConnect.end())
	{
		TCP_Client * pSession = *itr;
		delete pSession;
		itr++;
	}
	m_DqSessionPool_DisConnect.clear();
}

bool Session_RoundRobin::Reconnect_DisConnect_Pool()
{
	const std::lock_guard<std::mutex> lock(m_mtxSessionPool_DisConnect);

	std::vector< std::deque<TCP_Client *>::iterator > vecDel;

	auto itr = m_DqSessionPool_DisConnect.begin();
	while(itr != m_DqSessionPool_DisConnect.end())
	{
		TCP_Client * pSession = *itr;
		if(false == pSession->IsConnected() )
		{
			std::cout << "[Reconnect] << IP = " << pSession->Get_SessionIP() << " | Port = " << pSession->Get_Port() << std::endl;

			if(true == pSession->Reconnect( *m_pio_service ) )
			{
				Add_SessionPool_Connected( pSession );

				vecDel.emplace_back(itr);
			}
		}

		itr++;
	}

	for(auto &itr : vecDel)
		m_DqSessionPool_DisConnect.erase(itr);

	return m_DqSessionPool_DisConnect.empty();
}