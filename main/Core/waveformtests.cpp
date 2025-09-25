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

/** @file:  waveformtests.cpp
 *  @brief: Test CWaveform class.
 */
#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/Asserter.h>
#include "Asserts.h"
#define private public
#include "CWaveForm.h"
#undef private

#include <CNoSuchObjectException.h>
#include <math.h>
#include <vector>

class WaveformTests : public CppUnit::TestFixture {
CPPUNIT_TEST_SUITE(WaveformTests);
CPPUNIT_TEST(construct_1);
CPPUNIT_TEST(construct_2);
CPPUNIT_TEST(setmd_1);
CPPUNIT_TEST(setmd_2);
CPPUNIT_TEST(setmd_3);
CPPUNIT_TEST(getmd_1);
CPPUNIT_TEST(getmd_2);
CPPUNIT_TEST(getmd_3);
CPPUNIT_TEST(getmd_4);
CPPUNIT_TEST(update_1);
CPPUNIT_TEST(update_2);
CPPUNIT_TEST_SUITE_END();


protected:
    void construct_1();
    void construct_2();

    void setmd_1();
    void setmd_2();
    void setmd_3();

    void getmd_1();
    void getmd_2();
    void getmd_3();
    void getmd_4();

    void update_1();
    void update_2();

public:
    void setUp() {
        CWaveform::m_idIndex = 0;     // Reset the index.
    }

};

CPPUNIT_TEST_SUITE_REGISTRATION(WaveformTests);


/// the tests


// construction.

void
WaveformTests::construct_1() {
    // Test simple construction of the first one:

    CWaveform wf("Testing", 100);
    EQ(std::string("Testing"), wf.getName());
    EQ(UInt_t(0), wf.getNumber());
    EQ(size_t(100), wf.size());
}

void
WaveformTests::construct_2() {
    // id auto increments:

    CWaveform wf1("test1", 100);
    CWaveform wf2("test2", 125);
    EQ(UInt_t(1), wf2.getNumber());
}

// Setting metadata:
void
WaveformTests::setmd_1() {
    // Can set a single bit of metadata that's not yet defined:

    CWaveform wf("test", 100);
    wf.setMetadata("testing", "junk");
    EQ(std::string("junk"), wf.m_metadata["testing"]);
}
void
WaveformTests::setmd_2() {
    // can overwrite existing metadata:

    CWaveform wf("test", 100);
    wf.setMetadata("testing", "trash");
    wf.setMetadata("testing", "junk");
    EQ(std::string("junk"), wf.m_metadata["testing"]);
}
void
WaveformTests::setmd_3() {
    // More than one bit of metadata is kept straight:

    CWaveform wf("test", 100);
    wf.setMetadata("test1", "junk");
    wf.setMetadata("test2", "trash");

    EQ(std::string("junk"), wf.m_metadata["test1"]);
    EQ(std::string("trash"), wf.m_metadata["test2"]);
}

// Getting metadata.
void
WaveformTests::getmd_1() {
    // No such metadata thows a no such object exception.

    CWaveform wf ("Test", 100);

    CPPUNIT_ASSERT_THROW(
        wf.getMetadata("test"),
        CNoSuchObjectException
    );
}

void
WaveformTests::getmd_2() {
    // Getting one of one metadata:


    CWaveform wf("test", 100);
    wf.setMetadata("test", "junk");
    std::string value;
    CPPUNIT_ASSERT_NO_THROW(
        value = wf.getMetadata("test")
    );
    EQ(std::string("junk"), value);
}
void
WaveformTests::getmd_3() {
    // Got the right one from 'many'.

    CWaveform wf("test", 100);
    wf.setMetadata("test1", "junk");
    wf.setMetadata("test2", "trash");

    std::string value;
    CPPUNIT_ASSERT_NO_THROW(
        value = wf.getMetadata("test1")
    );
    EQ(std::string("junk"), value);

    CPPUNIT_ASSERT_NO_THROW(
        value = wf.getMetadata("test2")
    );
    EQ(std::string("trash"), value);
}

void
WaveformTests::getmd_4() {
    // Get the whole lot of the metadata:

    CWaveform wf("test", 100);
    wf.setMetadata("stooge1", "larry");
    wf.setMetadata("stooge2", "curly");
    wf.setMetadata("stooge3", "moe");
    wf.setMetadata("stoogeextra", "shemp");  // there reeally were 4 stooges :-)

    auto md = wf.getMetadata();
    EQ(size_t(4), md.size());
    EQ(std::string("larry"), md["stooge1"]);
    EQ(std::string("curly"), md["stooge2"]);
    EQ(std::string("moe"), md["stooge3"]);
    EQ(std::string("shemp"), md["stoogeextra"]);
}

// Update tests:

void
WaveformTests::update_1() {
    CWaveform wf("test", 100);
    std::vector<uint16_t> trace;

    for (int i =0; i < 100; i++) {
        double deg = i;                    // i degrees
        double rad = deg*3.1416/180.0; // converted to radians.
        trace.push_back(int(sin(rad) * 100));  // sine wave trace.
    }
    wf.update(trace.data());

    auto& saved = wf.trace();
    for (int i=0; i < saved.size(); i++) {
        EQ(trace[i], saved[i]);
    }

}
void
WaveformTests::update_2() {
    // unupdated trace is 0's:

    CWaveform wf("test", 100);
    auto& saved = wf.trace();
    for (auto sample : saved) {
        EQ(uint16_t(0), sample);
    }
}