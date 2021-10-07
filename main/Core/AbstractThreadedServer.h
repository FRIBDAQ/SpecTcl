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

/** @file:  AbstractThreadedServer.h
 *  @brief: Define a framework for TCP/IP servers that handle clients in threads. 
 */

#ifndef ABSTRACTTHREADEDSERVER_H
#define ABSTRACTTHREADEDSERVER_H
#include <CTCLThread.h>
#include <CTCLMutex.h>
#include <set>
#include <string>

class CSocket;
class ServerListener;                   // Forward reference.

/**
 * @class AbstractClientServer
 *      This abstract base class provides the base class for instances of
 *      client servers.   We can handle client registration with the port listener
 *      and deregistration.
 *      The flow of control respects exit requests.
 */
class AbstractClientServer : public CTCLThread
{
private:
    ServerListener*           m_pListener;
    CSocket*                  m_pClientSocket;
public:
    AbstractClientServer(
        ServerListener*         pListener,
        CSocket*                pCLient
    );
    virtual ~AbstractClientServer();
    
    // We handle the main flow of control using a strategy model.
    
    virtual int operator()(ClientData p);
    virtual void exitHandler();
    
    // Here are the strategy methods the concrete class must implement:
    // The only mandatory thing is to handle client requests.
    
    virtual void onConnection(CSocket* pSocket, ClientData p)  {};   //! Called on start
    virtual void onReadable(CSocket* pSocket, ClientData p) = 0;     //! Called on msg from peer.
    virtual void onClose(CSocket* pSocket, ClientData p)    {};     //! Called on peer close.
    
};
/**
 * @class AbstractServerFactory
 *    The port listener needs a mechanism to create concrete instances of
 *    a subclass of AbstractClientServer.  It does so by being handed a factory
 *    object that knows how to create them.  This is the base class for that
 *    factory.  This is not the abstract extensible pattern used elsewhere.
 *    Since, in general, a server offers a single service, each server listener
 *    thread gets exactly one concrete instance of theses to build client threads.
 */
class AbstractServerFactory
{
public:
    AbstractServerFactory();
    virtual ~AbstractServerFactory();
    
    /**
     * create
     *    Creates an instance of an Abstract client that handles client
     *    requests from one client.
     * @param pSocket - Socket the client will use to receive/send data.
     * @param cd      - Abstract data the factory can get/use as needed.
     *
     */                  
    virtual AbstractClientServer* create(
        CSocket* pSocket,
        ServerListener* pListener,
        ClientData cd
    ) = 0;
};

/**
 * @class ServerListener
 *   This class is a thread that, when started:
 *   - accepts connectsion on the service port.
 *   - For each connection uses its factory to create a server thread to handle
 *     requests for that connection.
 *   - Maintains a registry of client threads.
 *   - Processes (in its thread context), destruction requests from client threads.
 *     This is important because it's dangerous to destroy a thread object
 *     until the thread actually exits.
 *   @note the flow of control respects exit requests and uses it to
 *     kill all remaining servers before exiting itself.  This may be useful
 *     in a program exit handler.
 */
class ServerListener : public CTCLThread
{
private:
    typedef std::set<AbstractClientServer*> ServerSet;
    CMutex                          m_Monitor;       // For critical regions.
    AbstractServerFactory*          m_pServerFactory; // Creates server threads.
    CSocket*                        m_pListener;      // Socket bound to listener port.
    ServerSet                       m_activeClients;  // RunningClients.
    ServerSet                       m_exitingClients; // join/destroy list.
    ClientData                      m_factoryData;    // Sent to factory.
    
public:
    ServerListener(
        std::string service,
        AbstractServerFactory* pFactory,
        ClientData factoryData = nullptr
    );
    ~ServerListener();
    
    virtual int operator()(ClientData cd); // This CD is sent to new clients.
    
    
    // Called in the context of a client thread
    
    void requestDestruction(AbstractClientServer* pServer);
    
    // Private utilities
private:
    void startServer(CSocket* pServerSocket, ClientData cd);
    void recordServer(AbstractClientServer* pServer);
    void killallClients();
    void reapClients();
    
    // For testing only:
    
    ServerSet listActive() {
        CriticalSection guard(m_Monitor);
        ServerSet result = m_activeClients;
        return result;
    }
    
};

#endif