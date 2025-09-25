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

/** @file:  wfdicttests.cpp
 *  @brief: Test the waveform dictionary singleton.
 */
#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/Asserter.h>
#include "Asserts.h"

#define private public
#include "CWaveFormDictionary.h"
#undef private
#include "CWaveForm.h"

#include <CDuplicateSingleton.h>
#include <CNoSuchObjectException.h>



class WFDictTests : public CppUnit::TestFixture {
    CPPUNIT_TEST_SUITE(WFDictTests);
    CPPUNIT_TEST(instance_1);
    CPPUNIT_TEST(instance_2);

    CPPUNIT_TEST(add_1);
    CPPUNIT_TEST(add_2);
    CPPUNIT_TEST(add_2);
    CPPUNIT_TEST_SUITE_END();

protected:
    void instance_1();
    void instance_2();

    void add_1();
    void add_2();
    void add_3();
public:
    void setUp() {
        // destroy the dictionary:
        delete CWaveFormDictionary::m_pInstance;
        CWaveFormDictionary::m_pInstance = nullptr;

    }
};

CPPUNIT_TEST_SUITE_REGISTRATION(WFDictTests);

// Get instance tests:

void
WFDictTests::instance_1() {
    // A non null ref is retunred.
    auto& d1 = CWaveFormDictionary::getInstance();
    ASSERT(&d1);
}
void
WFDictTests::instance_2() {
    // The same instance ref is returned both times:

    auto& d1 = CWaveFormDictionary::getInstance();
    auto& d2 = CWaveFormDictionary::getInstance();

    EQ(&d1, &d2);
}

// Add tests.

void
WFDictTests::add_1() {
    // Adding an item is ok and adds it to the underlying dict:

    auto& d = CWaveFormDictionary::getInstance();
    CPPUNIT_ASSERT_NO_THROW(d.add(CWaveform("test", 100)));

    EQ(size_t(1), d.m_dict.size());
    ASSERT(d.m_dict.find("test") != d.m_dict.end());
    const CWaveform& wf = d.m_dict.find("test")->second;
    //EQ(size_t(100), wf.size());
}
void
WFDictTests::add_2() {
    // Adding a second item with a distinct name is ok.

    auto& d= CWaveFormDictionary::getInstance();
    CPPUNIT_ASSERT_NO_THROW(d.add(CWaveform("test1", 100)));
    CPPUNIT_ASSERT_NO_THROW(d.add(CWaveform("test2", 100)));

    EQ(size_t(2), d.m_dict.size());
    ASSERT(d.m_dict.find("test1") != d.m_dict.end());
    ASSERT(d.m_dict.find("test2") != d.m_dict.end());
}
void
WFDictTests::add_3() {
    // It's a duplicate singleton to put two identically named waveforms in the cit.

    auto& d = CWaveFormDictionary::getInstance();

    d.add(CWaveform("test", 100));
    CPPUNIT_ASSERT_THROW(
        d.add(CWaveform("test", 150)),
        CDuplicateSingleton
    );
}