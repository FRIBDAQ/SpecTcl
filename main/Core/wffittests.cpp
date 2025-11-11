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

/** @file:  wffittests.cpp
 *  @brief: Test the waveform fit extensions for Issue #212
 */
#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/Asserter.h>
#include "Asserts.h"

#define private public
#include "CWaveForm.h"
#undef private


#include <CDuplicateSingleton.h>
#include <CNoSuchObjectException.h>

class WFFitTests : public CppUnit::TestFixture {
CPPUNIT_TEST_SUITE(WFFitTests);
CPPUNIT_TEST(nofits_1);
CPPUNIT_TEST_SUITE_END();

private: 
    CWaveform* m_pTestwf;

protected:
    void nofits_1();
public:

    void setUp() {                  // Probably don't need this but whatever.
        CWaveform::m_idIndex = 0;   
        m_pTestwf = new CWaveform("someWaveform", 100);
    }
    void teardown() {
        delete m_pTestwf;
        m_pTestwf = nullptr;
    }

};
CPPUNIT_TEST_SUITE_REGISTRATION(WFFitTests);


///////////////////////////// The tests //////////////////

// If there are no fits, the dictionary and fit vector are empty:

void
WFFitTests::nofits_1() {
    EQ(size_t(0), m_pTestwf->m_fitDictionary.size());
    EQ(size_t(0), m_pTestwf->m_fits.size());
}