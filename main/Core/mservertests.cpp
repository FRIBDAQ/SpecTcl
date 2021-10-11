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

/** @file:  mservertests.cpp
 *  @brief: Tests for the mirror server.
 *  @note there's absolutely no reason we have to operate with an actual
 *        shared memory region.  Any memory block will do.
 */
#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/Asserter.h>
#include "Asserts.h"
#include "xamineDataTypes.h"
#include "MirrorServer.h"
#include "MirrorMessages.h"
#include "AbstractThreadedServer.h"
#include "CSocket.h"

class mservertest : public CppUnit::TestFixture {
    CPPUNIT_TEST_SUITE(mservertest);
    CPPUNIT_TEST(test_1);
    CPPUNIT_TEST_SUITE_END();
    
private:
    Xamine_shared m_memory;
    ServerListener* m_listener;
    MirrorServerFactory* m_factory;
public:
    void setUp() {
        // No defined spectra.
        
        for (int i =0; i < XAMINE_MAXSPEC; i++) {
            m_memory.dsp_types[i]  = undefined;
        }
        m_factory = new MirrorServerFactory(&m_memory);
        m_listener = new ServerListener("5555", m_factory);
        m_listener->start();
    }
    void tearDown() {
        m_listener->requestExit();
        m_listener->join();
        delete m_listener;
        delete m_factory;
    }
protected:
    void test_1();
};

CPPUNIT_TEST_SUITE_REGISTRATION(mservertest);

void mservertest::test_1()
{
}