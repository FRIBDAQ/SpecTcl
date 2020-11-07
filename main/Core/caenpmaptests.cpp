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

/** @file: caenpmaptests.cpp
 *  @brief: Test the various CAEN parameter mapping classes.
 */
#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/Asserter.h>
#include "Asserts.h"
#include "CAENParameterMap.h"
#include "CAENModuleHits.h"
#define private public
#define protected public
#include "CAENHit.h"              // Makes it easier to fake hits.
#undef private
#undef protected
#include <string>
#include <map>
#include <config.h>
#include <Event.h>
#include <TreeParameter.h>
#include <Histogrammer.h>
#include <Globals.h>


class caenmaptest : public CppUnit::TestFixture {
    CPPUNIT_TEST_SUITE(caenmaptest);
    CPPUNIT_TEST(phaarray_1);
    CPPUNIT_TEST(phaarray_2);
    CPPUNIT_TEST_SUITE_END();
    
private:
    CHistogrammer* m_pHistogrammer;
    CEvent*  m_pEvent;                  // Fake event.
public:
    void setUp() {
        m_pHistogrammer = new CHistogrammer;
        gpEventSink = m_pHistogrammer;
        m_pEvent = new CEvent;
    }
    void tearDown() {
        delete m_pEvent;
        delete m_pHistogrammer;
        gpEventSink = nullptr;
    }
protected:
    void phaarray_1();
    void phaarray_2();
};

CPPUNIT_TEST_SUITE_REGISTRATION(caenmaptest);

void caenmaptest::phaarray_1()
{
    CAENPHAArrayMapper testMap("time", "energy", "extra1", "extra2");
    CTreeParameterArray times("time", 16, 0);
    CTreeParameterArray energies("energy", 16, 0);
    CTreeParameterArray extra1("extra1", 16, 0);
    CTreeParameterArray extra2("extra2", 16, 0);
    
    CTreeParameter::BindParameters();
    CTreeParameter::setEvent(*m_pEvent);
    CAENModuleHits  fakeHits;
    CAENPHAHit   fakehit;
    fakehit.m_energy = 1;
    fakehit.m_extra1 = 100;
    fakehit.m_extra2 = 200;
    fakehit.m_timeTag = 12345;
    fakehit.m_channel = 2;
    fakeHits.addHit(&fakehit);
    testMap.assignParameters(fakeHits);
    
    
    
    // Our values should be in channel 2 of our arrays
    
    double energy = energies[2];
    EQ(double(fakehit.m_energy), energy);
    double t = times[2];
    EQ(double(fakehit.m_timeTag), t);
    double e1 = extra1[2];
    EQ(double(fakehit.m_extra1), e1);
    double e2 = extra2[2];
    EQ(double(fakehit.m_extra2), e2);
}

void caenmaptest::phaarray_2()
{
    // Two hits.
    CAENPHAArrayMapper testMap("time", "energy", "extra1", "extra2");
    CTreeParameterArray times("time", 16, 0);
    CTreeParameterArray energies("energy", 16, 0);
    CTreeParameterArray extra1("extra1", 16, 0);
    CTreeParameterArray extra2("extra2", 16, 0);
    
    CTreeParameter::BindParameters();
    CTreeParameter::setEvent(*m_pEvent);
    CAENModuleHits  fakeHits;
    CAENPHAHit   fakehit1;
    fakehit1.m_energy = 1;
    fakehit1.m_extra1 = 100;
    fakehit1.m_extra2 = 200;
    fakehit1.m_timeTag = 12345;
    fakehit1.m_channel = 2;
    fakeHits.addHit(&fakehit1);
    
    CAENPHAHit fakehit2;
    fakehit2.m_energy = 2;
    fakehit2.m_extra1 = 200;
    fakehit2.m_extra2 = 300;
    fakehit2.m_timeTag = 12366;
    fakehit2.m_channel = 1;
    fakeHits.addHit(&fakehit2);
    
    testMap.assignParameters(fakeHits);
    
    // channel 2:
    
    double energy = energies[2];
    EQ(double(fakehit1.m_energy), energy);
    double t = times[2];
    EQ(double(fakehit1.m_timeTag), t);
    double e1 = extra1[2];
    EQ(double(fakehit1.m_extra1), e1);
    double e2 = extra2[2];
    EQ(double(fakehit1.m_extra2), e2);
    
    // CHannel 1:
    
    energy = energies[1];
    t      = times[1];
    e1     = extra1[1];
    e2     = extra2[1];
    EQ(double(fakehit2.m_energy), energy);
    EQ(double(fakehit2.m_timeTag), t);
    EQ(double(fakehit2.m_extra1), e1);
    EQ(double(fakehit2.m_extra2), e2);
    
    
}
