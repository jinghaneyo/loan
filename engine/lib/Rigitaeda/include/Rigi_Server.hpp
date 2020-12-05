/*
Licensed under the MIT License <http://opensource.org/licenses/MIT>.
SPDX-License-Identifier: MIT
Copyright (c) 2013-2019 lee,gil-jae
*/

#ifndef _RIGI_SERVER_H_
#define _RIGI_SERVER_H_

#include "Rigi_Def.hpp"
#include "Rigi_TCPMgr.hpp"
#include "Rigi_TCPSession.hpp"
#include "Rigi_EventHandler.hpp"

namespace Rigitaeda
{
    class Rigi_UDPServer;

    class TCPSession final : public Rigi_TCPSession 
    {
    public:
        TCPSession() { }
        ~TCPSession(){ }
    };

    class Rigi_Server
    {
    public:
        Rigi_Server();
        virtual ~Rigi_Server();
    private:
        Rigi_TCPMgr<Rigi_TCPSession> *m_pTCP_Mgr;
        Rigi_UDPServer *m_pUDP_Server;
    public:
        bool Run(   __in int _nPort, 
                    __in int _nMaxClient )
        {
            // 기본 클래스로 인스턴스 생성하며, EventHandler로 콜백함수로 Receive를 받도록 한다
            m_pTCP_Mgr = new Rigi_TCPMgr<Rigi_TCPSession>();
            return m_pTCP_Mgr->Run( _nPort, _nMaxClient );
        }

        template <typename TCP_TMPL>
        bool Run(   __in int _nPort, 
                    __in int _nMaxClient,
                    __in Rigi_TCPMgr<TCP_TMPL> *_pMgr )
        {
            if(nullptr == _pMgr)
            {
                assert(0 && "Rigi_TCPMgr is not nullptr!!!");
                return false;
            }

            return _pMgr->Run( _nPort, _nMaxClient );
        }

        void Load_Conf( __in const char *_pszPath_conf );

        void Stop();
    };
}

#endif