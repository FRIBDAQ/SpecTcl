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
#include <unistd.h>

#include <errno.h>
#include <string.h>

class mctest : public CppUnit::TestFixture {
    CPPUNIT_TEST_SUITE(mctest);
    CPPUNIT_TEST(connect_1);
    CPPUNIT_TEST(connect_2);
    
    CPPUNIT_TEST(update_1);
    CPPUNIT_TEST(update_2);
    CPPUNIT_TEST(update_3);
    CPPUNIT_TEST(update_4);
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
        delete m_pClient;
        if (m_listener) {
            m_listener->requestExit();
            m_listener->join();
        }
        delete m_listener;
        delete m_factory;
        delete m_pInterp;        
    }
protected:
    void connect_1();
    void connect_2();
    
    void update_1();
    void update_2();
    void update_3();
    void update_4();
};

CPPUNIT_TEST_SUITE_REGISTRATION(mctest);

// Connect but without memory key:
void mctest::connect_1()
{
    m_pClient->connect("localhost", 5555);
    auto p = MirrorDirectorySingleton::getInstance();
    EQ(size_t(0), p->list().size());
}
// Connect with memory key
void mctest::connect_2()
{
    m_pClient->connect("localhost", 5555, "DEFG");
    sleep(1);            // Let the server start.
    auto p = MirrorDirectorySingleton::getInstance();
    auto l = p->list();
    EQ(size_t(1), l.size());
    EQ(std::string("DEFG"), l[0].second);
    EQ(std::string("localhost"), l[0].first);
}
// Get an update... should be full and it should work.
//

void mctest::update_1()
{
    m_pClient->connect("localhost", 5555, "DEFG");
    bool how = m_pClient->update(&m_clientMemory);
    ASSERT(how);               // Full update.
    EQ(0, memcmp(&m_memory, &m_clientMemory, offsetof(Xamine_shared, dsp_spectra.XAMINE_b)));
}
// Second update is not full:
void mctest::update_2()
{
    m_pClient->connect("localhost", 5555, "DEFG");
    m_pClient->update(&m_clientMemory);
    bool how = m_pClient->update(&m_clientMemory);
    ASSERT(!how);
}
//  Define some spectra ... update makes the def and transfers spectra.
void mctest::update_3()
{
    m_memory.dsp_xy[0].xchans = 256;
    m_memory.dsp_xy[0].ychans = 0;
    m_memory.dsp_offsets[0] = 0;
    m_memory.dsp_types[0] = onedlong;
    
    for (int i = 0; i < 256; i++) {
        m_memory.dsp_spectra.XAMINE_l[i] = i;
    }
    
    m_pClient->connect("localhost", 5555);
    m_pClient->update(&m_clientMemory);
    EQ(
       0,
       memcmp(
            &m_memory, &m_clientMemory,
            256*sizeof(uint32_t) + offsetof(Xamine_shared, dsp_spectra.XAMINE_b)
        )
    );
}
// Partial update won't update the definitions but will update the
// data.
void mctest::update_4()
{
    m_memory.dsp_xy[0].xchans = 256;
    m_memory.dsp_xy[0].ychans = 0;
    m_memory.dsp_offsets[0] = 0;
    m_memory.dsp_types[0] = onedlong;
    
    for (int i = 0; i < 256; i++) {
        m_memory.dsp_spectra.XAMINE_l[i] = i;
    }
    
    m_pClient->connect("localhost", 5555);
    m_pClient->update(&m_clientMemory); // full update.
    
    m_memory.dsp_xy[0].xchans = 512;     // Won't update.
    
    for (int i = 0; i < 256; i++) {
        m_memory.dsp_spectra.XAMINE_l[i] += 100; 
    }
    
    m_pClient->update(&m_clientMemory);     // partial update.
    EQ(uint32_t(256), m_clientMemory.dsp_xy[0].xchans);   // WOn't udpate
    
    EQ(
        0,
        memcmp(
            m_memory.dsp_spectra.XAMINE_b, m_clientMemory.dsp_spectra.XAMINE_b,
            256*sizeof(uint32_t)
        )
    );
}
