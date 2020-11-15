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
    
    CPPUNIT_TEST(psdarray_1);
    CPPUNIT_TEST(psdarray_2);
    
    CPPUNIT_TEST(psdmap_1);
    CPPUNIT_TEST_SUITE_END();
protected:
    void phaarray_1();
    void phaarray_2();
    
    void phamap_1();
    void phamap_2();
    void phamap_3();
    
    void psdarray_1();
    void psdarray_2();
    
    void psdmap_1();
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

void caenmaptest::psdarray_1()
{
    // Psd array parameter mapper:
    
    CAENPSDArrayMapper map("time", "short", "long", "baseline", "PURflag");
    
    // Now the stuff I can pull stuff out of:
    
    CTreeParameterArray times("time", 16, 0);
    CTreeParameterArray shorts("short", 16, 0);
    CTreeParameterArray longs("long", 16, 0);
    CTreeParameterArray bline("baseline", 16, 0);
    CTreeParameterArray pur("PURflag", 16, 0);
    
    CTreeParameter::BindParameters();
    CTreeParameter::setEvent(*m_pEvent);
    
    // Now a PSD hit for channel 7:
    
    CAENPSDHit fakehit(1);            // CFDMultiplier is 1.
    fakehit.m_timeTag = 1234;
    fakehit.m_channel = 7;
    fakehit.m_shortGateCharge = 100;
    fakehit.m_longGateCharge  = 1000;
    fakehit.m_baseline        = 12;
    fakehit.m_purFlag         = 0;
    fakehit.m_CFDTime         = 0;
    
    CAENModuleHits hits;             // Hit container 'decoded'.
    hits.addHit(&fakehit);
    CPPUNIT_ASSERT_NO_THROW(map.assignParameters(hits));
    
    // The tree parameter values for [7] in the arrays are all valid:
    
    ASSERT(times[7].isValid());
    ASSERT(shorts[7].isValid());
    ASSERT(longs[7].isValid());
    ASSERT(bline[7].isValid());
    ASSERT(pur[7].isValid());
    
    // Now that we established the values are valid we can check
    // the contents.  Since the CFD value is 0, we expect just the timetag
    // for the time (since we put ns in the value).
    
    double time = times[7];
    double shortc = shorts[7];       // short and long are, of course reserved 
    double longc = longs[7];         // words.
    double b     = bline[7];
    double pileup= pur[7];
    
    // Note the pileup flag sets 20 for true and 40 for not true.
           
   EQ(double(1234), time);
   EQ(double(100), shortc);
   EQ(double(1000), longc);
   EQ(double(12), b);
   EQ(double(40), pileup);
}
void caenmaptest::psdarray_2()
{
    // A couple of hits -- make the fine time matter.
    
    // Psd array parameter mapper:
    
    CAENPSDArrayMapper map("time", "short", "long", "baseline", "PURflag");
    
    // Now the stuff I can pull stuff out of:
    
    CTreeParameterArray times("time", 16, 0);
    CTreeParameterArray shorts("short", 16, 0);
    CTreeParameterArray longs("long", 16, 0);
    CTreeParameterArray bline("baseline", 16, 0);
    CTreeParameterArray pur("PURflag", 16, 0);
    
    CTreeParameter::BindParameters();
    CTreeParameter::setEvent(*m_pEvent);
    
    // Now a PSD hit for channel 7:
    
    CAENPSDHit fakehit(1);            // CFDMultiplier is 1.
    fakehit.m_timeTag = 1234;
    fakehit.m_channel = 7;
    fakehit.m_shortGateCharge = 100;
    fakehit.m_longGateCharge  = 1000;
    fakehit.m_baseline        = 12;
    fakehit.m_purFlag         = 0;
    fakehit.m_CFDTime         = 1;         // 1ps.
    
    
    CAENModuleHits hits;             // Hit container 'decoded'.
    hits.addHit(&fakehit);
    
    // Now a PSD hit for channel 9:
    
    CAENPSDHit fakehit2(2);
    fakehit2.m_timeTag = 2000;
    fakehit2.m_channel = 9;
    fakehit2.m_shortGateCharge = 250;
    fakehit2.m_longGateCharge  = 2345;
    fakehit2.m_baseline = 20;
    fakehit2.m_purFlag  = 1;                 // Piled up.
    fakehit2.m_CFDTime  = 1;           // 2ps
    hits.addHit(&fakehit2);
    
    map.assignParameters(hits);
    
    // [7] and [9] of the arrays should be valid.  Note going to check
    // the others to ensure they're not valid.
    
    ASSERT(times[7].isValid());
    ASSERT(times[9].isValid());
    ASSERT(shorts[7].isValid());
    ASSERT(shorts[9].isValid());
    ASSERT(longs[7].isValid());
    ASSERT(longs[9].isValid());
    ASSERT(bline[7].isValid());
    ASSERT(bline[9].isValid());
    ASSERT(pur[7].isValid());
    ASSERT(pur[9].isValid());
    
    // Now check the values...remember the times fold in the CFD time too:
    
    double time, shortc, longc, base, purflag;
    // Channel 7:
    time = times[7];
    shortc = shorts[7];
    longc = longs[7];
    base = bline[7];
    purflag = pur[7];
    
    EQ(double(1234.001), time);
    EQ(double(100), shortc);
    EQ(double(1000), longc);
    EQ(double(12), base);
    EQ(double(40), purflag);
    
    // Channel 9:
    
    time = times[9];
    shortc = shorts[9];
    longc = longs[9];
    base  = bline[9];
    purflag = pur[9];
    
    EQ(double(2000.002), time);
    EQ(double(250), shortc);
    EQ(double(2345), longc);
    EQ(double(20), base);
    EQ(double(20), purflag);
    
}
void caenmaptest::psdmap_1()
{
    // individually specified parameter names:
    
    std::vector<std::string> shortnames = {
        "short1", "short2", "front", "", "lastshort"
    };
    std::vector<std::string> longnames = {
        "long1", "", "back", "long4", "lastlong"
    };
    std::vector<std::string> basenames = {
        "base1", "base2", "", "baselast"
    };
    std::vector<std::string> timenames = {
        "time1", "time2", "time3", "time4", "lasttime"
    };
    std::vector<std::string> pupnames = {
        "pup1", "pup2", "pup3", "pup4", "pup5"
    };
    CAENPSDParameterMapper
        mapper(shortnames, longnames, basenames, timenames, pupnames);

    // We need a  few tree parameters of our own to see the data
    // This test will put data into channel1  so:
    
    CTreeParameter s1("short1");                     // NO data.
    CTreeParameter s2("short2");                     // data.
    
    CTreeParameter l1("long1");                     // no data.
    
}