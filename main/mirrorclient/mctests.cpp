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

/** @file:  mctests.cpp
 *  @brief: Test the mirror client against a mirror server.
 */
#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/Asserter.h>
#include "Asserts.h"
#include "xamineDataTypes.h"
#include "MirrorServer.h"
#include "MirrorMessages.h"
#include "MirrorDirectory.h"
#include "AbstractThreadedServer.h"
#include "CSocket.h"
#include "MirrorClient.h"
#include <TCLInterpreter.h>

class mctest : public CppUnit::TestFixture {
    CPPUNIT_TEST_SUITE(mctest);
    CPPUNIT_TEST(test_1);
    CPPUNIT_TEST_SUITE_END();
    
private:
    CTCLInterpreter* m_pInterp;
    Xamine_shared m_memory;
    ServerListener* m_listener;
    MirrorServerFactory* m_factory;
    
    MirrorClient* m_pClient;
    Xamine_shared m_clientMemory;
public:
    void setUp() {
        m_pInterp = new CTCLInterpreter; // needed to init thread structs.
        // No defined spectra.
        
        for (int i =0; i < XAMINE_MAXSPEC; i++) {
            m_memory.dsp_types[i]  = undefined;
        }
        m_factory = new MirrorServerFactory(&m_memory);
        m_listener = new ServerListener("5555", m_factory);
        m_listener->start();
        
        m_pClient = new MirrorClient;
    }
    void tearDown() {
        if (m_listener) {
            m_listener->requestExit();
            m_listener->join();
        }
        delete m_listener;
        delete m_factory;
        delete m_pClient;
        delete m_pInterp;
    }
protected:
    void test_1();
};

CPPUNIT_TEST_SUITE_REGISTRATION(mctest);

void mctest::test_1()
{
}