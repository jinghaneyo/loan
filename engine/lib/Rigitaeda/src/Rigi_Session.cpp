#include "Rigi_Session.hpp"

using namespace Rigitaeda;

Rigi_Session::Rigi_Session( __in boost::asio::io_service& _io_service, 
							__in boost::asio::ip::tcp::socket *_pSocket )
	: m_pSockTCP(_pSocket)
{
	assert(m_pSockTCP && "Do not nullptr m_pSockTCP !!!");
	m_eProtocol = PROTOCOL::TCP;
}

Rigi_Session::Rigi_Session( __in boost::asio::io_service& _io_service, 
							__in boost::asio::ip::udp::socket *_pSocket )
	: m_pSockUDP(_pSocket)
{
	assert(m_pSockUDP && "Do not nullptr m_pSockUDP !!!");
	m_eProtocol = PROTOCOL::UDP;
}

Rigi_Session::~Rigi_Session()
{
	Close(boost::asio::error::eof);

	m_pSockTCP = nullptr;
	m_pSockUDP = nullptr;
}

void Rigi_Session::Handler_Receive( __in const boost::system::error_code& _error, 
									__in size_t _bytes_transferred)
{
	//if (nullptr == m_pSocket)
	//{
	//	assert(0 && "m_pSocket is not nullptr!!!");
	//	return;
	//}

	if (_error)
		Close(_error);
	else
	{
		PostReceive();
	}
}

void Rigi_Session::Handler_Send( __in const boost::system::error_code& _error, 
									__in size_t _bytes_transferred)
{
}

void Rigi_Session::Init()
{
	m_ReceiveBuffer.empty();
}

void Rigi_Session::PostReceive()
{
	m_ReceiveBuffer.empty();

	if(PROTOCOL::TCP == m_eProtocol)
	{
		if (nullptr == m_pSockTCP)
		{
			assert(0 && "m_pSockTCP is not nullptr!!!");
			// todo 에러값 저장 
			return;
		}

		m_pSockTCP->async_read_some(boost::asio::buffer(m_ReceiveBuffer),
									boost::bind( &Rigi_Session::Handler_Receive,
												this, 
												boost::asio::placeholders::error,
												boost::asio::placeholders::bytes_transferred) );
	}
	else
	{
		if (nullptr == m_pSockUDP)
		{
			assert(0 && "m_pSockUDP is not nullptr!!!");
			// todo 에러값 저장 
			return;
		}
		// m_pSockUDP->async_receive_from(boost::asio::buffer(m_ReceiveBuffer),
		// 								boost::bind( &Rigi_Session::Handler_Receive,
		// 										this, 
		// 										boost::asio::placeholders::error,
		// 										boost::asio::placeholders::bytes_transferred) );
	}
}

void Rigi_Session::PostSend( __in const char* _pData, 
							 __in size_t _nSize)
{
	if (nullptr == m_pSockTCP)
	{
		assert(0 && "m_pSocket is not nullptr!!!");
		return;
	}

	boost::asio::async_write( *m_pSockTCP, 
								boost::asio::buffer(_pData, _nSize),
								boost::bind( &Rigi_Session::Handler_Send, 
											this,
										 	boost::asio::placeholders::error,
										 	boost::asio::placeholders::bytes_transferred )
	);
}

void Rigi_Session::Close( __in const boost::system::error_code& _error )
{
	if (nullptr == m_pSockTCP)
	{
		assert(0 && "m_pEventMaker is not nullptr!!!");
		return;
	}

	if (true == m_pSockTCP->is_open())
	{
		std::string strClientIP = GetIP_Remote();
		LOG(INFO) << "[CLOSE] " << strClientIP;

		m_pSockTCP->close();

		//*AfxGetPtr::Current_Session_Count() -= 1;
	}
}

void Rigi_Session::Close()
{
	boost::system::error_code error;
	Close(error);
}
