#include "Rigi_Socket.hpp"

using namespace Rigitaeda;

Rigi_Socket::Rigi_Socket( __in boost::asio::ip::tcp::socket *_pSocket )
	: m_pSockTCP(_pSocket)
{
	assert(m_pSockTCP && "Do not nullptr m_pSockTCP !!!");
	m_eProtocol = PROTOCOL::TCP;
}

Rigi_Socket::Rigi_Socket( __in boost::asio::ip::udp::socket *_pSocket )
	: m_pSockUDP(_pSocket)
{
	assert(m_pSockUDP && "Do not nullptr m_pSockUDP !!!");
	m_eProtocol = PROTOCOL::UDP;
}

Rigi_Socket::~Rigi_Socket()
{
}