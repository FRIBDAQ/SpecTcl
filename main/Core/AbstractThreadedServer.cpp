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

/** @file:  AbstractThreadedServer.cpp
 *  @brief: Implements the abstract threaded server.
 */

#include "AbstractThreadedServer.h"
#include "CSocket.h"
#include <CTCLThread.h>
#include <poll.h>
#include <errno.h>
#include <iostream>
#include <stdlib.h>

//////////////////////////////////////////////////////////////////////////
// AbstractClientServer implementation.

/**
 * AbstractClientServer - constructor
 *   @param pListener - the listener object that's asked its factory to
 *                      construct this.
 *   @param pClient   - The client socket who's requests we process.
 */
AbstractClientServer::AbstractClientServer(
    ServerListener*         pListener,
    CSocket*                pClient
) :
    m_pListener(pListener), m_pClientSocket(pClient)
{
    
}
/**
 * AbstractClientServer - destructor
 *     null for now
 */
AbstractClientServer::~AbstractClientServer()
{}

/**
 * operator()
 *     The strategy pattern for the server:
 *     - Invoke the onConnection strategy method.
 *     - While not asked to exit.
 *        Poll for input.
 *        If there's input invoke onReadable to process it.
 *     - invoke theonClose strategy pattern.
 *     - Return 0 status.
 *  @note the exitHandler method will request destruction from the server listener.
 *  @param p  - client data we ignore but pass to the strategy methods.
 */
int
AbstractClientServer::operator()(ClientData p)
{
    onConnection(m_pClientSocket, p);
    
    int fd = m_pClientSocket->getSocketFd();
    struct pollfd pollInfo = {
        fd, POLLIN | POLLRDHUP, 0
    };
    
    while(!exitRequested()) {
        pollInfo.revents = 0;
        int n = poll(&pollInfo, 1, 1);  // Wait for input for a second.
        if (n > 0) {
            // If this is a HUP break to exit>
            
            if (pollInfo.revents & POLLHUP) {
                break;    
            } else {
                // Must be readable:
                
                onReadable(m_pClientSocket, p);
            }
        } else if (n < 0) {
            // anything other than EINTR is very bad:
            // EINTR means poll was just interrupted so we keep going.
            
            if (errno != EINTR) {
                std::cerr << "AbstractClientServer::operator() - bad status from poll";
                try {
                    m_pClientSocket->Shutdown();
                }
                catch (...) {}               // Ignore failures.
                delete m_pClientSocket;
                return -1;
            }
        }                            // Timed out so while loop can check exit.

    }
    // We get here either when we've exited or the client has exited.
    
    onClose(m_pClientSocket, p);
    
    // Try to shutdown if we can and kill the socket:
    
    try {
        m_pClientSocket->Shutdown();
    }
    catch(...) {}                    // ignore failures.
    delete m_pClientSocket;

    m_pClientSocket = nullptr;            // SO references will kill us.
    
    return 0;
}
/**
 * exitHandler
 *     Called when we're exiting.
 *     Request the listener to destroy us...as it's not safe to self-destruct
 *     in our thread.
 */
void
AbstractClientServer::exitHandler()
{
    m_pListener->requestDestruction(this);
}
///////////////////////////////////////////////////////////////////////////////
// AbstractServerFactory 'implementation'

/**
 * AbstractServerFactory - constructor NoOP
 */
AbstractServerFactory::AbstractServerFactory() {}

/**
 * AbstractServerFactory -destructor noop.
 */
AbstractServerFactory::~AbstractServerFactory() {}


////////////////////////////////////////////////////////////////////////////////
// ServerListener implementation.
//

/**
 * ServerListener - constructor
 *   @param service - the name/port number of the service to be bound to the
 *                    listener socket.  Note this can be a name in /etc/services
 *                    or the textual version of a number It _cannot_ be a
 *                    DAQPortManager service name.
 *   @param pFactory - A factory to create server threads for each connection.
 *   @param factoryData - Client data to be passed to the factory for each
 *                     creation operation.  This is not passed to threads.
 */
ServerListener::ServerListener(
    std::string service,
    AbstractServerFactory* pFactory,
    ClientData factoryData
) :
    m_pServerFactory(pFactory),
    m_pListener(nullptr),
    m_factoryData(factoryData)
{
    m_pListener = new CSocket;
    try {
        m_pListener->Bind(service);
        m_pListener->Listen();
    }
    catch (...) {
        delete m_pListener;
        throw;
    }
}
/**
 * ServerListener destructor
 */
ServerListener::~ServerListener()
{
    delete m_pListener;
}
/**
 * operator() - thread entry point.
 *   @param cd - Client data that is passed to service threads when started.
 *   @return int - 0 on success, -1 otherwise.
 */
int
ServerListener::operator()(ClientData cd)
{
    int fd = m_pListener->getSocketFd();
    struct pollfd pollInfo = {
        fd, POLLIN , 0
    };
    std::string client;
    while (!exitRequested()) {
        int n = poll(&pollInfo, 1, 1);
        if (n > 0) {
            CSocket* pClient = m_pListener->Accept(client);
            startServer(pClient, cd);
        } else if (n < 0) {
            // If EINTR that's ok just go again.
            
            if (errno != EINTR) {
                // Otherwise error and exit.
                
                std::cerr << "ServerListener - polled failed";
                return -1;
            }
        }
        reapClients();               // destroy clients in the m_exitingClients set.
    }
    killallClients();               // Kill all remaining clients.
    
    delete m_pListener;
    m_pListener = nullptr;
    return 0;
}
/**
 * requestDestrution (threadsafe)
 *    Called in a client server thread to transfer us to the exiting clients
 *    set.  Then reapClients in the listener thread will join/destroy.
 * @param pServer - the server that's exiting.
 */
void
ServerListener::requestDestruction(AbstractClientServer* pServer)
{
    CriticalSection guard(m_Monitor);     // Thread-safe
    m_activeClients.erase(pServer);
    m_exitingClients.insert(pServer);
    
}
///////////////////////////////////////////////////////////////////////////////
// ServerListener - private utilities.

/**
 * startServer
 *    Creates, registers and starts a new server.
 *    - Ask the factory for a server.
 *    - Record it in the running server list.
 *    - Start it.
 * @param pServerSocket - the socket the server interacts with the client over.
 * @param cd            - Arbitrary data given to the thread.
 */
void
ServerListener::startServer(CSocket* pServerSocket, ClientData cd)
{
    AbstractClientServer* pNewServer =
        m_pServerFactory->create(pServerSocket, this, m_factoryData);
    recordServer(pNewServer);
    pNewServer->start(cd);
}
/**
 * recordServer (threadsafe)
 * 
 *    Record a new server in m_activeClients.
 * @param AbstractClientServer* pServer - server to record.
 */
void
ServerListener::recordServer(AbstractClientServer* pServer)
{
    CriticalSection guard(m_Monitor);
    m_activeClients.insert(pServer);
}
/**
 * killAllClients (threadsafe)
 *   - One by one, ask all clients to die.Joining and deleteing with them.
 *   - If an active client gets put in the exiting clients list get rid of it
 *   - reap the clients pending destruction that have already exite.
 *
 *   This is made a bit complex by the race conditions and potential deadlocks.
 *   A simplifying assumption is made that no new clients will get added and,
 *   if this runs in the listener thread that's valid.
 */
void
ServerListener::killallClients()
{
    ServerSet active;
    
    // Only hold until copied to prevent deadlocks.
    {
        CriticalSection gaurd(m_Monitor);
        active = m_activeClients;
        m_activeClients.clear();
    }
    // From now on operate on our copy.
    
    for (AbstractClientServer* p : active) {
        if (p) {
            p->requestExit();                // Ask it to exit.
            p->join();                       // Wait until it does...
            delete p;                        // Delete the object
        }
        CriticalSection guard(m_Monitor);
        m_exitingClients.erase(p);    // It will have been added by the timem of join().
    }
    // At this point, only previously  marked for deletion threads wil
    // be in the exiting list:
    
    reapClients();
}
/**
 * reapClients  (thredsafe)
 *    For all items in the exitingClients list:
 *    -  join
 *    -  remove from set
 *    -  destroy.
 * @note - in order to ensure that iterators don't get invalidated by asyncrhonous
 *         adds, we iterate on a copy of the delete pending list.
 */
void
ServerListener::reapClients()
{
    ServerSet deletePending;
    {
        CriticalSection guard(m_Monitor);
        deletePending = m_exitingClients;
        m_exitingClients.clear();
    }
    // From here we iterate over the copy:
    
    for (AbstractClientServer* p : deletePending) {
        if (p) {
            p->join();                      // Ensure actual exit....
            delete p;
        }
    }
}
