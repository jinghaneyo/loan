#include "Rigi_TCPSession.hpp"

using namespace Rigitaeda;

Rigi_TCPSession::Rigi_TCPSession()
{
	m_Event_Receive = nullptr;
}

Rigi_TCPSession::~Rigi_TCPSession()
{
	Close(boost::asio::error::eof);

	m_Event_Receive = nullptr;
}

void Rigi_TCPSession::OnEvent_Receive(	__in char *_pData,
										__in size_t _nData_len )
{
	std::cout << "[HANDLER RECV][" << m_ReceiveBuffer.data() << std::endl;
	// receive 콜백 함수 호출 
	if(nullptr != m_Event_Receive)
	{
		m_Event_Receive( m_pSocket, _pData, _nData_len );
	}
}

void Rigi_TCPSession::Handler_Receive( 	__in const boost::system::error_code& _error, 
										__in size_t _bytes_transferred)
{
	if (nullptr == m_pSocket)
	{
		assert(0 && "m_pSocket is not nullptr!!!");
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

void Rigi_TCPSession::Init()
{
	m_ReceiveBuffer.empty();
}

void Rigi_TCPSession::Async_Receive()
{
	m_ReceiveBuffer.empty();

	if (nullptr == m_pSocket)
	{
		assert(0 && "m_pSocket is not nullptr!!!");
		// todo 에러값 저장 
		return;
	}

	m_pSocket->async_read_some( boost::asio::buffer(m_ReceiveBuffer),
								boost::bind( &Rigi_TCPSession::Handler_Receive,
											this, 
											boost::asio::placeholders::error,
											boost::asio::placeholders::bytes_transferred) );
}

void Rigi_TCPSession::PostSend( __in const char* _pData, 
							 	__in size_t _nSize)
{
	if (nullptr == m_pSocket)
	{
		assert(0 && "m_pSocket is not nullptr!!!");
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
	{
		assert(0 && "m_pSocket is not nullptr!!!");
		return;
	}

	if (true == m_pSocket->is_open())
	{
		std::string strClientIP = GetIP_Remote();
		LOG(INFO) << "[CLOSE] " << strClientIP;

		m_pSocket->close();
		m_pSocket = nullptr;
	}
}

void Rigi_TCPSession::Close()
{
	boost::system::error_code error;
	Close(error);
}

void Rigi_TCPSession::SetEvent_Receive( __in Event_Received_TCP &&_Event )
{
	m_Event_Receive = std::move(_Event);
}

std::string && Rigi_TCPSession::GetIP_Remote()
{
	if( true == m_strIP_Client.empty())
	{
		boost::asio::ip::tcp::endpoint remote_ep = m_pSocket->remote_endpoint();
		boost::asio::ip::address remote_ad = remote_ep.address();
		m_strIP_Client = remote_ad.to_string();

		return std::move(m_strIP_Client);
	}
	else
	{
		std::string strIP = "0.0.0.0";
		return std::move(strIP);
	}
}