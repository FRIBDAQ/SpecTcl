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
#include <CWaveForm.h>
#undef private

class WaveformTests : public CppUnit::TestFixture {
CPPUNIT_TEST_SUITE(WaveformTests);
CPPUNIT_TEST_SUITE_END();

public:
    void setUp() {
        CWaveform::m_idIndex = 0;     // Reset the index.
    }

};

CPPUNIT_TEST_SUITE_REGISTRATION(WaveformTests);