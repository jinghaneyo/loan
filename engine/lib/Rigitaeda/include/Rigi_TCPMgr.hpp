#ifndef _RIGI_TCP_SERVER_H_
#define _RIGI_TCP_SERVER_H_

#include <vector>
#include <map>

#include "Rigi_Def.hpp"
#include "Rigi_TCPSession.hpp"
#include "Rigi_SessionPool.hpp"

namespace Rigitaeda
{
    template <typename TCP_TMPL>
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
                try
                {
                    TCP_TMPL *pSession = new TCP_TMPL();
                    pSession->Set_TCPMgr((void *)this);
                    if( false == m_SessionPool.Add_Session(pSession, _pSocket) )
                    {
                        delete pSession;
                    }
                    else
                        OnEvent_Accept_Session( pSession );

                    AsyncAccept();
                }
                catch(const std::exception& e)
                {
                    std::cerr << "[Exception][Handle_accept] >> " << e.what() << '\n';
                }
            }
            else
            {
                //AfxGetPtr::SetLastError(error.message());
            }
        }

    public:
        int GetPort() { return m_nPort; };

        bool Run(   __in int _nPort, 
                    __in int _nMaxClient )
        {
            if(1 > _nPort)
            {
                assert(0 && "[Rigi_TCPMgr::Run] port < 0 !!");
                return false;
            }
            if(1 > _nMaxClient)
            {
                assert(0 && "[Rigi_TCPMgr::Run] _nMaxClient < 0 !!");
                return false;
            }

            m_nPort = _nPort;

            m_SessionPool.Set_MaxClient(_nMaxClient);

            m_io_service.reset();

            boost::asio::ip::tcp::endpoint endpoint(boost::asio::ip::tcp::v4(), m_nPort);

            // Init 이벤트 호출
            if( false == OnEvent_Init() )
            {
                assert(0 && "[Rigi_TCPMgr::Run] OnEvent_Init is false !!");
                return false;
            }

            m_acceptor.open(endpoint.protocol());
            m_acceptor.bind(endpoint);
            m_acceptor.listen(_nMaxClient);

            AsyncAccept();

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

        // 재정의하여 데이터 처리 부분을 추가한다.
        virtual void OnEvent_Accept_Session( __in TCP_TMPL *_pSession ) { };

        // 재정의하여 데이터 처리 부분을 추가한다.
        virtual bool OnEvent_Init() { return true; };
    };

}

#endif