#include "Rigi_TCPServer.hpp"

using namespace Rigitaeda;

Rigi_TCPServer::Rigi_TCPServer() : m_acceptor(m_io_service)
{
    m_nPort = 3333;
    m_nMaxClient = 1000;
    m_Event_Receive = nullptr;
}

Rigi_TCPServer::~Rigi_TCPServer()
{
    Stop();
}

bool Rigi_TCPServer::Run(   __in Event_Received_TCP &&_Callback,
                            __in int _nPort, 
                            __in int _nMaxClient)
{
    m_nPort = _nPort;
    m_nMaxClient = _nMaxClient;

    m_Event_Receive = std::move(_Callback);

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

void Rigi_TCPServer::Stop()
{
    m_acceptor.cancel();
    m_acceptor.close();

    m_io_service.stop();

    for(auto &pSocket : m_vecSession)
    {
        pSocket->Close();
        delete pSocket;
    }
}

void Rigi_TCPServer::StartAccept()
{
    AsyncAccept();
}

void Rigi_TCPServer::AsyncAccept()
{
    boost::asio::ip::tcp::socket* pSocket = new boost::asio::ip::tcp::socket(m_io_service);

    Rigi_TCPSession *pSession = new Rigi_TCPSession(m_io_service, pSocket);
    pSession->SetEvent_Receive( std::move(m_Event_Receive) );
    m_vecSession.emplace_back(pSession);

    m_acceptor.async_accept(*(pSession->GetSocket()),
                                boost::bind(&Rigi_TCPServer::Handle_accept,
                                            this,
                                            pSession,
                                            boost::asio::placeholders::error)
                            );
}

void Rigi_TCPServer::Handle_accept( __in Rigi_TCPSession* _pSession, 
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
                std::cout << "[ACCEPT] >> " << szClose << std::endl;
                _pSession->PostSend(szClose, sizeof(szClose));

                _pSession->Close();
            }
            else
            {
                std::string strClientIP = _pSession->GetIP_Remote();
                std::cout << "[ACCEPT][IP = " << strClientIP << "]" << std::endl;
                LOG(INFO) << "[ACCEPT] " << strClientIP;

                _pSession->Async_Receive();
            }
        }

        AsyncAccept();
    }
    else
    {
        //AfxGetPtr::SetLastError(error.message());
    }
}