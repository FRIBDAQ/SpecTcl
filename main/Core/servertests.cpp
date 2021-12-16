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

/** @file:  servertest.cpp
 *  @brief: Test server framework.
 */
#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/Asserter.h>
#include "Asserts.h"
#include <stdint.h>

#define private public
#include "AbstractThreadedServer.h"
#undef private
#include "CSocket.h"
#include <TCLInterpreter.h>
#include <memory>
#include <unistd.h>
#include <CTCLMutex.h>
#include <CTCPConnectionLost.h>
#include <stdint.h>
#include <string.h>

////////////////////////////////////////////////////////////////////////////////
// Define an echo server:

class Echo : public AbstractClientServer
{
    bool connected;
    bool closed;
public:
    Echo(ServerListener* pListener, CSocket* pClient);
    ~Echo();
    void onConnection(CSocket* pSocket, ClientData p);
    void onReadable(CSocket* pSocket, ClientData p);
    void onClose(CSocket* pSocket, ClientData p);
};
Echo::Echo(ServerListener* pListener, CSocket* pClient) :
    AbstractClientServer(pListener, pClient),
    connected(false), closed(false) {}
Echo::~Echo() {}

void Echo::onConnection(CSocket* pSocket, ClientData p) {
    connected = true;
}
void Echo::onReadable(CSocket* pSocket, ClientData p) {
    // This try/empty catch is because if the client disconnects,
    // The read throws a CTCPConnectionLost exception.
    try {
        uint32_t size;
        int n = pSocket->Read(&size, sizeof(uint32_t));
        if (n != sizeof(uint32_t) ) {
            return;
        }
        std::unique_ptr<char> buffer(new char[size]);
    
        n = pSocket->Read(buffer.get(), size);
        if (n != size) {
            return;
        }
        // Echo:
        
        pSocket->Write(&size, sizeof(uint32_t));
        pSocket->Write(buffer.get(), size);
        pSocket->Flush();
    } catch(...) {
    
    }
    
}
void Echo::onClose(CSocket* pSocket, ClientData cd)
{
    closed = true;
}
///////////////////////////////////////////////////////////////////////////////
// EchoFactory:

class EchoFactory : public AbstractServerFactory
{
public:
    EchoFactory() {}
    ~EchoFactory() {}
    virtual AbstractClientServer* create(
        CSocket* pSocket, ServerListener* pListener, ClientData cd);
    
};

AbstractClientServer*
EchoFactory::create(CSocket* pSocket, ServerListener* pListener, ClientData cd)
{
    return new Echo(pListener, pSocket);
}

EchoFactory fact;
////////////////////////

static const std::string port="32123";

class servertest : public CppUnit::TestFixture {
    CPPUNIT_TEST_SUITE(servertest);
    CPPUNIT_TEST(noconn);
    CPPUNIT_TEST(connect_1);
    CPPUNIT_TEST(echo_1);
    CPPUNIT_TEST_SUITE_END();
    
private:
    CTCLInterpreter* m_pInterp;
    ServerListener* m_pListener;
public:
    void setUp() {
        m_pInterp = new CTCLInterpreter();
        m_pListener = new ServerListener(port, &fact);
        m_pListener->start();
    }
    void tearDown() {
        if (m_pListener) {
            m_pListener->requestExit();
            m_pListener->join();
        }
        delete m_pListener;
        delete m_pInterp;
    }
protected:
    void noconn();
    void connect_1();
    void echo_1();
};

CPPUNIT_TEST_SUITE_REGISTRATION(servertest);

// Initially there are no connections.

void servertest::noconn()
{
    ASSERT(m_pListener->m_activeClients.empty());
}
// COnnections make a server thread
void servertest::connect_1()
{
    CSocket client;
    CPPUNIT_ASSERT_NO_THROW(client.Connect(std::string("localhost"), port));
    sleep(1);
    {
        CriticalSection g(m_pListener->m_Monitor);
        EQ(size_t(1), m_pListener->m_activeClients.size());
    }
    
    client.Shutdown();              // Should force the thread to exit.
    sleep(1);
    {
        CriticalSection g(m_pListener->m_Monitor);
        ASSERT(m_pListener->m_activeClients.empty());
    }
    
    
}
// Server thread can echo my  data:
void servertest::echo_1()
{
    CSocket client;
    client.Connect(std::string("localhost"), port);
    
    std::string msg("Hello world");
    uint32_t size = msg.size();
    
    char     reply[size+10];
    memset(reply, 0, sizeof(reply));
    
    client.Write(&size, sizeof(size));
    client.Write(msg.c_str(), size);
    client.Flush();
    int s = client.Read(&size, sizeof(uint32_t));
    EQ(sizeof(uint32_t), size_t(s));
    EQ(size_t(size), msg.size());
    
    s = client.Read(reply, size);
    EQ(size_t(s), msg.size());
    std::string strReply(reply);
    EQ(std::string("Hello world"), strReply);
    
    try {
        client.Shutdown();
    } catch (...) {}
    sleep(1);                         // Give it time to get lost:
    {
        CriticalSection g(m_pListener->m_Monitor);
        ASSERT(m_pListener->m_activeClients.empty());
    }
    
}
