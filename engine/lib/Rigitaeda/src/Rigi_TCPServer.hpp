#ifndef _RIGI_TCP_SERVER_H_
#define _RIGI_TCP_SERVER_H_

#include <vector>

#include "Rigi_Def.hpp"
#include "Rigi_TCPSession.hpp"

// TCP 서버 
namespace Rigitaeda
{
    class Rigi_TCPServer
    {
    public:
        Rigi_TCPServer();
        ~Rigi_TCPServer();

    private:
        int m_nPort;
        int m_nMaxClient;

        boost::asio::io_service m_io_service;
        boost::asio::ip::tcp::acceptor m_acceptor;

        Event_Received_TCP m_Event_Receive;

        std::vector<Rigi_TCPSession *> m_vecSession;
    public:
        bool Run(   __in Event_Received_TCP &&_Callback,
                    __in int _nPort, 
                    __in int _nMaxClient);

        void Stop();

        void StartAccept();

        void AsyncAccept();

        void Handle_accept( __in Rigi_TCPSession* _pSession, 
                            __in const boost::system::error_code& _error);
    };
}

#endif