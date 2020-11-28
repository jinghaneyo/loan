#include "Rigi_Server.hpp"
#include "Rigi_TCPMgr.hpp"

using namespace Rigitaeda;

Rigi_Server::Rigi_Server()
{
    m_pTCP_Mgr = nullptr;
    m_pUDP_Server = nullptr;
}

Rigi_Server::~Rigi_Server()
{
    Stop();
}

void Rigi_Server::Stop()
{
    if(nullptr != m_pTCP_Mgr)
    {
        m_pTCP_Mgr->Stop();

        delete m_pTCP_Mgr;
        m_pTCP_Mgr = nullptr;
    }

    // if(nullptr != m_pServerUDP)
    // {
    //     //m_pServerUDP->Stop();

    //     delete m_pServerUDP;
    //     m_pServerUDP = nullptr;
    // }
}