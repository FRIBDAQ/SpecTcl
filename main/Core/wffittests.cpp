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
CPPUNIT_TEST(find_1);
CPPUNIT_TEST(find_2);
CPPUNIT_TEST(find_3);
CPPUNIT_TEST(get_1);
CPPUNIT_TEST(get_2);
CPPUNIT_TEST(get_3);
CPPUNIT_TEST(get_4);
CPPUNIT_TEST_SUITE_END();

private: 
    CWaveform* m_pTestwf;

protected:
    void nofits_1();

    void add_1();
    void add_2();
    void add_3();
    void add_4();

    void find_1();
    void find_2();
    void find_3();

    void get_1();              // By number.
    void get_2();

    void get_3();             // By name.
    void get_4();

    // utilities (not tests)

    void add();     // Add fit named "afit"
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

    add();
    EQ(size_t(1), m_pTestwf->m_fitDictionary.size());
    EQ(size_t(1), m_pTestwf->m_fits.size());
}
void
WFFitTests::add_2() {
    // adding a fit puts it in the dictionary.

    add();
    ASSERT(m_pTestwf->m_fitDictionary.find("afit") != m_pTestwf->m_fitDictionary.end());
}
void
WFFitTests::add_3() {
     // Adding a fit puts it in the vector with the correct id and size:

     add();
     size_t n = m_pTestwf->m_fitDictionary["afit"];
     EQ(size_t(0), n);
     EQ(size_t(100), m_pTestwf->m_fits[0].size());
}

void
WFFitTests::add_4() {
    // Adding a duplicate fit throws CDuplicateSingleton

    add();
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

// Tests for the find method:

void
WFFitTests::find_1() {
    // Can't find what's not there.

    add();
    CPPUNIT_ASSERT_THROW(
        m_pTestwf->findFit("nosuch"),
        CNoSuchObjectException

    );
}
void
WFFitTests::find_2() {
    // Can find one it it's there:

    add();
    size_t  id;
    CPPUNIT_ASSERT_NO_THROW(
        id = m_pTestwf->findFit("afit")
    );
    EQ(id, m_pTestwf->m_fitDictionary["afit"]);
}
void
WFFitTests::find_3() {
    // We can find the correct one when there are several present.

    add();                   //"afit"
    m_pTestwf->addFit("another");

    size_t id1, id2;
    CPPUNIT_ASSERT_NO_THROW(
        id1 = m_pTestwf->findFit("afit")
    );
    CPPUNIT_ASSERT_NO_THROW(
        id2 = m_pTestwf->findFit("another")
    );

    // Correct indices "afit" is 0, "another ' is 1.. but
    // let's get it from the map:

    ASSERT(id1 != id2);   // They are distinct.
    EQ(id1, m_pTestwf->m_fitDictionary["afit"]);
    EQ(id2, m_pTestwf->m_fitDictionary["another"]);
}

// Tests for both getFit methods. 

void
WFFitTests::get_1() {
    // Get existing fit by number:
    add();

    CPPUNIT_ASSERT_NO_THROW(
        auto& fit = m_pTestwf->getFit(m_pTestwf->m_fitDictionary["afit"])
    );
    // I don't think fit is in scope any more but we want to check that this is the 
    // correct fit:

    auto& fit = m_pTestwf->getFit(m_pTestwf->m_fitDictionary["afit"]);
    const CWaveform::Fit_t* actual = &(m_pTestwf->m_fits[m_pTestwf->m_fitDictionary["afit"]]);
    EQ(&fit, actual);
}

void 
WFFitTests::get_2() {
    // Get nonexistinug fit by number throws:
    add();
    CPPUNIT_ASSERT_THROW(
        m_pTestwf->getFit(1234),
        CNoSuchObjectException
    );
}

void 
WFFitTests::get_3() {
    // Get by name existing:
    add();
    CPPUNIT_ASSERT_NO_THROW(
        auto& fit = m_pTestwf->getFit("afit")
    );
    auto& fit = m_pTestwf->getFit("afit");
    const CWaveform::Fit_t* actual = &(m_pTestwf->m_fits[m_pTestwf->m_fitDictionary["afit"]]);
    EQ(&fit, actual);

}
void
WFFitTests::get_4() {
    // nonexistent by name throws.
    add();
    CPPUNIT_ASSERT_THROW(
        m_pTestwf->getFit("nosuchfit"),
        CNoSuchObjectException
    );
}


////////////////////// Utility methods:

void
WFFitTests::add() {
    // Add a fit named "afit"

    m_pTestwf->addFit("afit");
}
