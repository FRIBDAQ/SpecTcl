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

/** @file:  wfapitests.cpp
 *  @brief: Test the waveform api in the SpecTcl singleton.
 */
#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/Asserter.h>
#include "Asserts.h"

#define private public
#include "CWaveFormDictionary.h"
#undef private
#include "SpecTcl.h"
#include "CWaveForm.h"


// Note that the api is just a wrapper to the dictionary
// since that's been thoroughly tested, in many cases
// mimimal testing is sufficent.
class WFAPITests : public CppUnit::TestFixture {
    CPPUNIT_TEST_SUITE(WFAPITests);
    CPPUNIT_TEST(add_1);
    CPPUNIT_TEST(remove_1);
    CPPUNIT_TEST_SUITE_END();

protected:
    void add_1();
    void remove_1();
public:
    void setUp() {
        // new dictionary for each test:

        delete CWaveFormDictionary::m_pInstance;
        CWaveFormDictionary::m_pInstance = nullptr;
    }
};

CPPUNIT_TEST_SUITE_REGISTRATION(WFAPITests);


// Test add:

void
WFAPITests::add_1() {
    auto pApi = SpecTcl::getInstance();
    pApi->addWaveform(CWaveform("test", 100));

    CPPUNIT_ASSERT_NO_THROW(CWaveFormDictionary::getInstance().find("test"));
}

// remove

void
WFAPITests::remove_1() {
    auto pApi = SpecTcl::getInstance();
    pApi->addWaveform(CWaveform("test", 100));

    pApi->removeWaveform("test");
    EQ(size_t(0), CWaveFormDictionary::getInstance().size());
}