#ifndef _RIGI_SERVER_H_
#define _RIGI_SERVER_H_

#include <iostream>
#include <algorithm>
#include <string>
#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include <boost/system/error_code.hpp>
#include <vector>

#include "Rigi_Def.hpp"
#include "Rigi_Session.hpp"

// TCP 서버 
namespace Rigitaeda
{
    class Rigi_TServer
    {
    public:
        Rigi_TServer();
        ~Rigi_TServer();

    private:
        int m_nPort = 3333;
        int m_nMaxClient = 1000;

        boost::asio::io_service m_io_service;
        boost::asio::ip::tcp::acceptor m_acceptor;

        std::vector<Rigi_Session *> m_vecSession;
    public:
        bool Run( __in int _nPort, __in int _nMaxClient);

        void Stop();

        void StartAccept();

        void AsyncAccept();

        void Handle_accept( __in Rigi_Session* _pSession, 
                            __in const boost::system::error_code& _error);
    };
}

#endif