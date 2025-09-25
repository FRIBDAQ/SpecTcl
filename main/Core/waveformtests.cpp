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

class WaveformTests : public CppUnit::TestFixture {
CPPUNIT_TEST_SUITE(WaveformTests);
CPPUNIT_TEST(construct_1);
CPPUNIT_TEST(construct_2);
CPPUNIT_TEST_SUITE_END();


protected:
    void construct_1();
    void construct_2();

public:
    void setUp() {
        CWaveform::m_idIndex = 0;     // Reset the index.
    }

};

CPPUNIT_TEST_SUITE_REGISTRATION(WaveformTests);


/// the tests

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