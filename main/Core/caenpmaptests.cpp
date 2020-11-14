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
    
    CPPUNIT_TEST(phamap_1);
    CPPUNIT_TEST(phamap_2);
    CPPUNIT_TEST(phamap_3);
    CPPUNIT_TEST_SUITE_END();
protected:
    void phaarray_1();
    void phaarray_2();
    
    void phamap_1();
    void phamap_2();
    void phamap_3();
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
void caenmaptest::phamap_1()
{
    std::vector<std::string> times = {
        "time1", "time", "ttt", "time-trial"
    };
    std::vector<std::string> energies = {
        "e1", "", "e3", "energy", "low-energy"
    };
    std::vector<std::string> extras1;
    std::vector<std::string> extras2 = {
        "extra2", "ext", "eeeeee"
    };
    
    CTreeParameter t1("time1", 16);
    CTreeParameter e1("e1", 16);
    CTreeParameter extra("extra2", 16);
    
    
    CAENPHAParameterMapper testmap(times, energies, extras1, extras2);
    CTreeParameter::BindParameters();
    CTreeParameter::setEvent(*m_pEvent);
    
    CAENModuleHits  fakeHits;
    CAENPHAHit   fakehit1;
    fakehit1.m_energy = 1;
    fakehit1.m_extra1 = 100;
    fakehit1.m_extra2 = 200;
    fakehit1.m_timeTag = 12345;
    fakehit1.m_channel = 0;
    fakeHits.addHit(&fakehit1);
    
    CPPUNIT_ASSERT_NO_THROW(testmap.assignParameters(fakeHits));
    
    ASSERT(t1.isValid());
    ASSERT(e1.isValid());
    ASSERT(extra.isValid());
    double t = t1;
    double e = e1;
    double ex= extra;
    
    EQ(double(1.0), e);
    EQ(double(12345), t);
    EQ(double(200), ex);
    
    
    
}

void caenmaptest::phamap_2()
{
    std::vector<std::string> times = {
        "time1", "time", "ttt", "time-trial"
    };
    std::vector<std::string> energies = {
        "e1", "", "e3", "energy", "low-energy"
    };
    std::vector<std::string> extras1;
    std::vector<std::string> extras2 = {
        "extra2", "ext", "eeeeee"
    };
    
    CTreeParameter en("low-energy");
    
    CAENPHAParameterMapper testmap(times, energies, extras1, extras2);
    CTreeParameter::BindParameters();
    CTreeParameter::setEvent(*m_pEvent);
    
    CAENModuleHits  fakeHits;
    CAENPHAHit   fakehit1;
    fakehit1.m_energy = 1;
    fakehit1.m_extra1 = 100;
    fakehit1.m_extra2 = 200;
    fakehit1.m_timeTag = 12345;
    fakehit1.m_channel = 4;
    fakeHits.addHit(&fakehit1);
    
    CPPUNIT_ASSERT_NO_THROW(testmap.assignParameters(fakeHits));
    
    ASSERT(en.isValid());
    double e = en;
    EQ(double(1), e);
}

void caenmaptest::phamap_3()
{
    // Double hit the second hit hits the hole in energies:
    
    
    std::vector<std::string> times = {
        "time1", "time", "ttt", "time-trial"
    };
    std::vector<std::string> energies = {
        "e1", "", "e3", "energy", "low-energy"
    };
    std::vector<std::string> extras1;
    std::vector<std::string> extras2 = {
        "extra2", "ext", "eeeeee"
    };
    
    // Parameters we can claw out of hit1 in channel 1:
    
    CTreeParameter t1("time");
    CTreeParameter ex1("ext");
    
    // parameters we can claw out of hit 2 in channel 3
    
    CTreeParameter t2("time-trial");
    CTreeParameter e2("energy");
    
    // Parameters that should not be valid
    
    CTreeParameter invt("time1");
    CTreeParameter inve("low-energy");
    CTreeParameter invex("eeeeee");
    
    
    CAENPHAParameterMapper testmap(times, energies, extras1, extras2);
    CTreeParameter::BindParameters();
    CTreeParameter::setEvent(*m_pEvent);
    
    CAENModuleHits  fakeHits;
    
    // Hit1 (channel1):
    
    CAENPHAHit   fakehit1;
    fakehit1.m_energy = 1;
    fakehit1.m_extra1 = 100;
    fakehit1.m_extra2 = 200;
    fakehit1.m_timeTag = 12345;
    fakehit1.m_channel = 1;
    fakeHits.addHit(&fakehit1);
    
    // Hit 2 (channel 3)
    
    CAENPHAHit fakehit2;
    fakehit2.m_energy = 2;
    fakehit2.m_extra1 = 200;
    fakehit2.m_extra2 = 300;
    fakehit2.m_timeTag = 12366;
    fakehit2.m_channel = 3;
    fakeHits.addHit(&fakehit2);
    
    testmap.assignParameters(fakeHits);
    
    // Ensure all our parameters are valid -- that should be.
    
    ASSERT(t1.isValid());
    ASSERT(ex1.isValid());
    ASSERT(t2.isValid());
    ASSERT(e2.isValid());
    ASSERT(!(invt.isValid()));
    ASSERT(!(inve.isValid()));
    ASSERT(!(invex.isValid()));
    
    // Check the values of valid hits:
    
    double t1v  = t1;
    double ex1v = ex1;
    EQ(double(12345), t1v);
    EQ(double(200), ex1v);
    
    double t2v = t2;
    double e2v = e2;
    EQ(double(12366), t2v);
    EQ(double(2), e2v);
    
}