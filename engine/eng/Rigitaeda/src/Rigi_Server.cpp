#include "Rigi_Server.hpp"

Rigi_Server::Rigi_Server() : m_acceptor(m_io_service)
{
}

Rigi_Server::~Rigi_Server()
{
}

bool Rigi_Server::Run(int nPort, int nMaxClient)
{
    m_nPort = nPort;
    m_nMaxClient = nMaxClient;

    m_io_service.reset();

    //boost::asio::ip::tcp::endpoint endpoint(boost::asio::ip::tcp::v4(), m_nPort);
    // boost::asio::ip::tcp::endpoint endpoint( boost::asio::ip::address::from_string("127.0.0.1"), m_nPort);

    // m_acceptor.open(endpoint.protocol());
    // m_acceptor.bind(endpoint);
    // m_acceptor.listen(nMaxClient);

    // StartAccept();

    // boost::system::error_code ec;
    // m_io_service.run(ec);

    return true;
}

void Rigi_Server::Stop()
{
    // m_acceptor.cancel();
    // m_acceptor.close();

    // m_io_service.stop();
}

void Rigi_Server::StartAccept()
{
    AsyncAccept();
}

void Rigi_Server::AsyncAccept()
{
    // boost::asio::ip::tcp::socket* pSocket = new boost::asio::ip::tcp::socket(m_io_service);

    // Rigi_Session *pSession = new Rigi_Session(m_io_service, pSocket);
    // m_vecSession.emplace_back(pSession);

    // m_acceptor.async_accept(*(pSession->GetSocket()),
    //                             boost::bind(&Rigi_Server::Handle_accept,
    //                             this,
    //                             pSession,
    //                             boost::asio::placeholders::error));
}

void Rigi_Server::Handle_accept(Rigi_Session* pSession, const boost::system::error_code& error)
{
    // if (!error)
    // {
    //     if (nullptr != pSession)
    //     {
    //         //if (m_nMaxClient < *AfxGetPtr::Current_Session_Count())
    //         if (m_nMaxClient < (int)m_vecSession.size())
    //         {
    //             char szClose[] = "Connection Full !!";
    //             pSession->PostSend(szClose, sizeof(szClose));

    //             pSession->Close();
    //         }
    //         else
    //         {
    //             pSession->PostReceive();
    //         }
    //     }

    //     AsyncAccept();
    // }
    // else
    // {
    //     //AfxGetPtr::SetLastError(error.message());
    // }
}