#include "Rigi_Session.hpp"

Rigi_Session::Rigi_Session( __in boost::asio::io_service& _io_service, 
							__in boost::asio::ip::tcp::socket *_pSocket )
	: m_pSocket(_pSocket)
{
	assert(m_pSocket && "Do not nullptr m_pSocket !!!");
}

Rigi_Session::~Rigi_Session()
{
	Close(boost::asio::error::eof);

	m_pSocket = nullptr;
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
	//m_ReceiveBuffer.empty();
	memset(m_ReceiveBuffer, 0, sizeof(m_ReceiveBuffer));
}

void Rigi_Session::PostReceive()
{
	if (nullptr == m_pSocket)
	{
		assert(0 && "m_pSocket is not nullptr!!!");
		return;
	}

	//m_ReceiveBuffer.empty();
	memset(m_ReceiveBuffer, 0, sizeof(m_ReceiveBuffer));

	m_pSocket->async_read_some( boost::asio::buffer(m_ReceiveBuffer, 1024),
							 	boost::bind( &Rigi_Session::Handler_Receive,
										 	this, 
											boost::asio::placeholders::error,
										 	boost::asio::placeholders::bytes_transferred) );
}

void Rigi_Session::PostSend( __in const char* _pData, 
							 __in size_t _nSize)
{
	if (nullptr == m_pSocket)
	{
		assert(0 && "m_pSocket is not nullptr!!!");
		return;
	}

	boost::asio::async_write( *m_pSocket, 
								boost::asio::buffer(_pData, _nSize),
								boost::bind( &Rigi_Session::Handler_Send, 
											this,
										 	boost::asio::placeholders::error,
										 	boost::asio::placeholders::bytes_transferred )
	);
}

void Rigi_Session::Close( __in const boost::system::error_code& _error )
{
	if (nullptr == m_pSocket)
	{
		assert(0 && "m_pEventMaker is not nullptr!!!");
		return;
	}

	if (true == m_pSocket->is_open())
	{
		m_pSocket->close();

		//*AfxGetPtr::Current_Session_Count() -= 1;

		m_bIsAbleSocket = false;
	}
}

void Rigi_Session::Close()
{
	boost::system::error_code error;
	Close(error);
}
