#include "Rigi_TCPSession.hpp"
#include "Rigi_TCPMgr.hpp"
#include "Rigi_SessionPool.hpp"

using namespace Rigitaeda;

Rigi_TCPSession::Rigi_TCPSession()
{
	m_pSocket = nullptr;
	m_pTCPMgr = nullptr;
}

Rigi_TCPSession::~Rigi_TCPSession()
{
	Close(boost::asio::error::eof);

	m_pSocket = nullptr;
}

void Rigi_TCPSession::OnEvent_Receive(	__in char *_pData,
										__in size_t _nData_len )
{
	// 재정의하여 데이터 처리 부분을 추가한다.
}

void Rigi_TCPSession::Handler_Receive( 	__in const boost::system::error_code& _error, 
										__in size_t _bytes_transferred)
{
	if (nullptr == m_pSocket)
	{
		assert(0 && "[Rigi_TCPSession::Handler_Receive] m_pSocket is not nullptr!!!");
		return;
	}

	if (_error)
	{	
		OnEvent_Close();

		Close(_error);
	}
	else
	{
		OnEvent_Receive( m_ReceiveBuffer.data(), _bytes_transferred );

		Async_Receive();
	}
}

void Rigi_TCPSession::Handler_Send( __in const boost::system::error_code& _error, 
									__in size_t _bytes_transferred)
{
}

void Rigi_TCPSession::BufferClear()
{
	m_ReceiveBuffer.empty();
}

void Rigi_TCPSession::Async_Receive()
{
	m_ReceiveBuffer.empty();

	if (nullptr == m_pSocket)
	{
		assert(0 && "[Rigi_TCPSession::Async_Receive] m_pSocket is not nullptr!!!");
		// todo 에러값 저장 
		return;
	}

	m_pSocket->async_read_some( boost::asio::buffer(m_ReceiveBuffer),
								boost::bind( &Rigi_TCPSession::Handler_Receive,
											this, 
											boost::asio::placeholders::error,
											boost::asio::placeholders::bytes_transferred) );
}

void Rigi_TCPSession::Send( __in const char* _pData, 
							__in size_t _nSize)
{
	if (nullptr == m_pSocket)
	{
		assert(0 && "[Rigi_TCPSession::Send] m_pSocket is not nullptr!!!");
		return;
	}

	m_pSocket->send(boost::asio::buffer(_pData, _nSize));
}

void Rigi_TCPSession::Async_Send( 	__in const char* _pData, 
							 		__in size_t _nSize)
{
	if (nullptr == m_pSocket)
	{
		assert(0 && "[Rigi_TCPSession::Async_Send] m_pSocket is not nullptr!!!");
		return;
	}

	boost::asio::async_write( 	*m_pSocket, 
								boost::asio::buffer(_pData, _nSize),
								boost::bind( &Rigi_TCPSession::Handler_Send, 
											this,
										 	boost::asio::placeholders::error,
										 	boost::asio::placeholders::bytes_transferred )
	);
}

void Rigi_TCPSession::Close( __in const boost::system::error_code& _error )
{
	if (nullptr == m_pSocket)
		return;

	if (true == m_pSocket->is_open())
	{
		std::string strClientIP = GetIP_Remote();
		LOG(INFO) << "[CLOSE] " << strClientIP;

		m_pSocket->close();
		m_pSocket = nullptr;
	}

	if(nullptr != m_pSessionPool)
		m_pSessionPool->Close_Session(this);
	m_pSessionPool = nullptr;
}

void Rigi_TCPSession::Close()
{
	boost::system::error_code error;
	Close(error);
}

const char *Rigi_TCPSession::GetIP_Remote()
{
	try
	{
		if( true == m_strIP_Client.empty())
		{
			if(nullptr != m_pSocket)
			{
				boost::asio::ip::tcp::endpoint remote_ep = m_pSocket->remote_endpoint();
				boost::asio::ip::address remote_ad = remote_ep.address();
				m_strIP_Client = remote_ad.to_string();

				return m_strIP_Client.c_str();
			}
			else
				return "0.0.0.0";
		}
		else
			return m_strIP_Client.c_str();
	}
	catch(const std::exception& e)
	{
		std::cerr << "[Rigi_TCPSession::GetIP_Remote] ERR = " << e.what() << '\n';
		return "0.0.0.0";
	}
}

void Rigi_TCPSession::SetSessionPool( __in Rigi_SessionPool *_pSessionPool )
{
	m_pSessionPool = _pSessionPool;
}

const Rigi_SessionPool *Rigi_TCPSession::GetSessionPool()
{
	return m_pSessionPool;
}

void Rigi_TCPSession::Set_TCPMgr( __in void * _pMgr )
{
	m_pTCPMgr = _pMgr;
}

void * Rigi_TCPSession::Get_TCPMgr()
{
	return m_pTCPMgr;
}

_OUTPUT_TYPE Rigi_TCPSession::GetType_Output()
{
	if(nullptr == m_pSessionPool)
		return _OUTPUT_TYPE::LOG;
	else
		return m_pSessionPool->GetType_Output();
}