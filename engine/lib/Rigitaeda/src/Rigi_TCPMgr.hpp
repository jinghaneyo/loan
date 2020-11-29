#ifndef _RIGI_TCP_SERVER_H_
#define _RIGI_TCP_SERVER_H_

#include <vector>
#include <map>

#include "Rigi_Def.hpp"
#include "Rigi_TCPSession.hpp"
#include "Rigi_SessionPool.hpp"

namespace Rigitaeda
{
    template <typename T>
    class Rigi_TCPMgr
    {
    public:
        Rigi_TCPMgr() : m_acceptor(m_io_service)
        {
            m_nPort = 3333;
        }

        virtual ~Rigi_TCPMgr()
        {
            Stop();
        }

    private:
        int m_nPort;

        boost::asio::io_service         m_io_service;
        boost::asio::ip::tcp::acceptor  m_acceptor;

        Rigi_SessionPool                m_SessionPool;
    public:
        bool Run(   __in int _nPort, 
                    __in int _nMaxClient )
        {
            m_nPort = _nPort;

            m_SessionPool.Set_MaxClient(_nMaxClient);

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

            m_SessionPool.Clear();
        }

        void StartAccept()
        {
            AsyncAccept();
        }

        void AsyncAccept()
        {
            SOCKET_TCP * pSocket = new SOCKET_TCP(m_io_service);

            m_acceptor.async_accept(    *pSocket,
                                        boost::bind(&Rigi_TCPMgr::Handle_accept,
                                                    this,
                                                    pSocket,
                                                    boost::asio::placeholders::error)
            );
        }

        void Handle_accept( __in SOCKET_TCP *_pSocket,
                            __in const boost::system::error_code& _error )
        {
            if (!_error)
            {
                T *pSession = new T();
                pSession->SetSocket(_pSocket);

                if( false == m_SessionPool.Add_Session(pSession) )
                {
                    delete pSession;
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