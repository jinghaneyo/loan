#ifndef _RIGI_SERVER_H_
#define _RIGI_SERVER_H_

#include "Rigi_Def.hpp"
#include "Rigi_TCPMgr.hpp"
#include "Rigi_TCPSession.hpp"

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
        Rigi_TCPMgr<TCPSession> *m_pTCP_Mgr;

        Rigi_UDPServer *m_pUDP_Server;
    public:
        template <typename T>
        bool Run(   __in int _nPort, 
                    __in int _nMaxClient,
                    __in Rigi_TCPMgr<T> *_pMgr = nullptr )
        {
            if(nullptr == _pMgr)
            {
                assert(0 && "Rigi_TCPMgr is not nullptr!!!");
                return false;
            }

            _pMgr->Run( nullptr, _nPort, _nMaxClient );
            // if(nullptr == _pMgr)
            // {
            //     m_pTCP_Mgr = new Rigi_TCPMgr<TCPSession>();
            //     m_pTCP_Mgr->Run( nullptr, _nPort, _nMaxClient );
            // }
            // else

            return true;
        }

        void Stop();
    };
}

#endif