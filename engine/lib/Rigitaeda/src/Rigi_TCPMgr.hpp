#ifndef _RIGI_TCP_SERVER_H_
#define _RIGI_TCP_SERVER_H_

#include <vector>

#include "Rigi_Def.hpp"
#include "Rigi_TCPSession.hpp"

namespace Rigitaeda
{
    template <typename T>
    class Rigi_TCPMgr
    {
    public:
        Rigi_TCPMgr() : m_acceptor(m_io_service)
        {
            m_nPort = 3333;
            m_nMaxClient = 1000;
            m_Event_Receive = nullptr;
        }

        virtual ~Rigi_TCPMgr()
        {
            Stop();
        }

    private:
        int m_nPort;
        int m_nMaxClient;

        boost::asio::io_service m_io_service;
        boost::asio::ip::tcp::acceptor m_acceptor;

        Event_Received_TCP m_Event_Receive;

        std::vector<T *> m_vecSession;
    public:
        bool Run(   __in Event_Received_TCP &&_Callback,
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

        void Stop()
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

        void StartAccept()
        {
            AsyncAccept();
        }

        void AsyncAccept()
        {
            SOCKET_TCP * pSocket = new SOCKET_TCP(m_io_service);

            T *pSession = new T();
            pSession->SetSocket(pSocket);
            m_vecSession.emplace_back(pSession);

            m_acceptor.async_accept(*(pSession->GetSocket()),
                                        boost::bind(&Rigi_TCPMgr::Handle_accept,
                                                    this,
                                                    pSession,
                                                    boost::asio::placeholders::error)
            );
        }

        void Handle_accept( __in Rigi_TCPSession* _pSession, 
                            __in const boost::system::error_code& _error)
        {
            if (!_error)
            {
                if (nullptr != _pSession)
                {
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
                        LOG(INFO) << "[ACCEPT] " << strClientIP;

                        _pSession->SetEvent_Receive( std::move(m_Event_Receive) );
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
    };

}

#endif