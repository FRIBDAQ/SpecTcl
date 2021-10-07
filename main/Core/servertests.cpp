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

static const std::string port="5555";

class servertest : public CppUnit::TestFixture {
    CPPUNIT_TEST_SUITE(servertest);
    CPPUNIT_TEST(test_1);
    CPPUNIT_TEST_SUITE_END();
    
private:
    CTCLInterpreter* pInterp;
    ServerListener* pListener;
public:
    void setUp() {
        pInterp = new CTCLInterpreter();
        pListener = new ServerListener(port, &fact);
        pListener->start();
    }
    void tearDown() {
        pListener->requestExit();
        pListener->join();
        delete pListener;
        delete pInterp;
    }
protected:
    void test_1();
};

CPPUNIT_TEST_SUITE_REGISTRATION(servertest);

void servertest::test_1()
{
}