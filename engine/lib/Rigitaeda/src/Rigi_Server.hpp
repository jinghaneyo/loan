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

namespace Rigitaeda
{
    class Rigi_Server
    {
    public:
        Rigi_Server();
        ~Rigi_Server();

    private:
        std::array<char, MAX_MESSAGE_LEN> m_ReceiveBuffer;
        boost::asio::ip::tcp::socket *m_pSocket = nullptr;

        int m_nPort = 3333;
        int m_nMaxClient = 1000;

        boost::asio::io_service m_io_service;
        boost::asio::ip::tcp::acceptor m_acceptor;

        std::vector<Rigi_Session *> m_vecSession;
    public:
        bool Run(int nPort, int nMaxClient);

        void Stop();

        void StartAccept();

        void AsyncAccept();

        void Handle_accept(Rigi_Session* pSession, const boost::system::error_code& error);
    };
}

#endif