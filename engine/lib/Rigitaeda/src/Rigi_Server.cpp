#include "Rigi_Server.hpp"

using namespace Rigitaeda;

Rigi_Server::Rigi_Server() : m_acceptor(m_io_service)
{
}

Rigi_Server::~Rigi_Server()
{
    Stop();
}

bool Rigi_Server::Run( __in int _nPort, __in int _nMaxClient)
{
    m_nPort = _nPort;
    m_nMaxClient = _nMaxClient;

    m_io_service.reset();

    boost::asio::ip::tcp::endpoint endpoint(boost::asio::ip::tcp::v4(), m_nPort);

    m_acceptor.open(endpoint.protocol());
    m_acceptor.bind(endpoint);
    m_acceptor.listen(_nMaxClient);

    StartAccept();

    boost::system::error_code ec;
    m_io_service.run(ec);

    return true;
}

void Rigi_Server::Stop()
{
    m_acceptor.cancel();
    m_acceptor.close();

    m_io_service.stop();
}

void Rigi_Server::StartAccept()
{
    AsyncAccept();
}

void Rigi_Server::AsyncAccept()
{
    boost::asio::ip::tcp::socket* pSocket = new boost::asio::ip::tcp::socket(m_io_service);

    Rigi_Session *pSession = new Rigi_Session(m_io_service, pSocket);
    m_vecSession.emplace_back(pSession);

    m_acceptor.async_accept(*(pSession->GetSocket()),
                                boost::bind(&Rigi_Server::Handle_accept,
                                this,
                                pSession,
                                boost::asio::placeholders::error));
}

void Rigi_Server::Handle_accept(    __in Rigi_Session* _pSession, 
                                    __in const boost::system::error_code& _error)
{
    if (!_error)
    {
        if (nullptr != _pSession)
        {
            //if (m_nMaxClient < *AfxGetPtr::Current_Session_Count())
            if (m_nMaxClient < (int)m_vecSession.size())
            {
                char szClose[] = "Connection Full !!";
                _pSession->PostSend(szClose, sizeof(szClose));

                _pSession->Close();
            }
            else
            {
                std::string strClientIP = _pSession->GetIP_Remote();
                LOG(INFO) << "[ACCEPT] " << strClientIP;

                _pSession->PostReceive();
            }
        }

        AsyncAccept();
    }
    else
    {
        //AfxGetPtr::SetLastError(error.message());
    }
}