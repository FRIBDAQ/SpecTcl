/*
    This software is Copyright by the Board of Trustees of Michigan
    State University (c) Copyright 2017.

    You may use this software under the terms of the GNU public license
    (GPL).  The terms of this license are described at:

     http://www.gnu.org/licenses/gpl.txt

     Authors:
             Ron Fox
             Giordano Cerriza
	     NSCL
	     Michigan State University
	     East Lansing, MI 48824-1321
*/

/** @file:   MirrorServer.h
 *  @brief:  Defines an abstract client server thread for mirroring.
 */
#ifndef MIRRORSERVER_H
#define MIRRORSERVER_H
#include "AbstractThreadedServer.h"
#include <CTCLMutex.h>
#include <xamineDataTypes.h>
#include <tcl.h>



/**
 * @class MirrorServer
 *    Mirror servers serve shared memory data to mirror clients.
 *    This is a somewhat stateful server/client relationship so that SpecTcl
 *    can maintain a MirrorDirectory that shows which clients are maintaining
 *    memory mirrors:
 *       Upon connection a client is expected (though not required) to
 *       send a Mirror::MSG_TYPE_SHMINFO message which provides the memory key
 *       associated with this client.  THat key and the host of the client
 *       are entered into the mirror directory singleton.
 *
 *       Once that's all done, the client sends Mirror::MSG_TYPE_REQUEST_UPDATE
 *       messages which are responded to by either Mirror::MSG_TYPE_FULL_UPDATE
 *       messages that provide the entier used part of the shared memory or
 *       Mirror::MSG_TYPE_PARTIAL_UPDATE messages which provide only the used
 *       spectrum contents part of the shared memory.
 *
 *       When we are constructed, we have a one-shot flag set to send a full
 *       update after that's been sent, we subsequently send partial updates.
 *
 *       An external 'force' (bound to sbind/unbind traces) can set that flag
 *       again which, in turn will cause the next update to be full.
 *
 *       The idea is that unless sbind or unbind have modified the spectrum
 *       description header part of the shared memory, there's no need to
 *       send it over and over again.  Unly when the set  of spectra in shared
 *       memory change do we need to do that.
 */
class MirrorServer : public AbstractClientServer
{
private:
    std::string m_peer;                       // Host name of peer.
    bool        m_sendAll;                    // send full flag.
    CTCLMutex   m_monitor;                    // For synchronization.
    Xamine_shared* m_pMemory;                 // Shared Memory region.
    
public:
    // Canonicals:
    
    MirrorServer(
        ServerListener* pListener, CSocket* pSocket, Xamine_shared* pMemory
    );
private:
    MirrorServer(const MirrorServer& rhs);
    MirrorServer& operator=(const MirrorServer& rhs);
    
    // Server methods:
public:
    virtual void onConnection(CSocket* pSocket, ClientData pData);
    virtual void onReadable(CSocket* pSocket, ClientData pData);
    virtual void onClose(CSocket* pSocket, ClientData pData);
    
    // Additional publics
public:
    void requestFullUpdate();
    
    // Utilities
private:
    void* spectrumDataPointer();
    size_t usedSpectrumBytes() const;
    size_t headerSize() const;
    void processMemoryInfo(CSocket* pSocket, uint32_t msgSize);
    void processUpdate(CSocket* pSocket, uint32_t msgSize);
    
};

/**
 * @class MirrorServerFactory
 *    Factory to crate new mirror servers.
 */
class MirrorServerFactory : public AbstractServerFactory
{
private:
    Xamine_shared* m_pMemory;
public:
    MirrorServerFactory(Xamine_shared* pMemory);
    virtual AbstractClientServer* create(
        CSocket* pSocket,
        ServerListener* pListener,
        ClientData cd
    );
};

#endif