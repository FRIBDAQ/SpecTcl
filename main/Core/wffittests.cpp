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
CPPUNIT_TEST(add_1);
CPPUNIT_TEST(add_2);
CPPUNIT_TEST(add_3);
CPPUNIT_TEST(add_4);
CPPUNIT_TEST_SUITE_END();

private: 
    CWaveform* m_pTestwf;

protected:
    void nofits_1();

    void add_1();
    void add_2();
    void add_3();
    void add_4();
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

// Tests to add fits:

void 
WFFitTests::add_1() {
    // Adding a fit makes the dictionary and vector size 1:

    m_pTestwf->addFit("afit");
    EQ(size_t(1), m_pTestwf->m_fitDictionary.size());
    EQ(size_t(1), m_pTestwf->m_fits.size());
}
void
WFFitTests::add_2() {
    // adding a fit puts it in the dictionary.

    m_pTestwf->addFit("afit");
    ASSERT(m_pTestwf->m_fitDictionary.find("afit") != m_pTestwf->m_fitDictionary.end());
}
void
WFFitTests::add_3() {
     // Adding a fit puts it in the vector with the correct id and size:

     m_pTestwf->addFit("afit");
     size_t n = m_pTestwf->m_fitDictionary["afit"];
     EQ(size_t(0), n);
     EQ(size_t(100), m_pTestwf->m_fits[0].size());
}

void
WFFitTests::add_4() {
    // Adding a duplicate fit throws CDuplicateSingleton

    m_pTestwf->addFit("afit");
    CPPUNIT_ASSERT_THROW(
        m_pTestwf->addFit("afit"),
        CDuplicateSingleton
    );
    // but adding a different one does not:

    CPPUNIT_ASSERT_NO_THROW(
        m_pTestwf->addFit("anotherfit")
    );
    // and that makes 2 dictionary and vector entries:

    EQ(size_t(2), m_pTestwf->m_fitDictionary.size());
    EQ(size_t(2), m_pTestwf->m_fits.size());
}
