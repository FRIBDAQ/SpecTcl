/*
    This software is Copyright by the Board of Trustees of Michigan
    State University (c) Copyright 2017.

    You may use this software under the terms of the GNU public license
    (GPL).  The terms of this license are described at:

     http://www.gnu.org/licenses/gpl.txt

     Authors:
             Ron Fox
             Giordano Cerriza
	     FRIB
	     Michigan State University
	     East Lansing, MI 48824-1321
*/

/** @file:  ring12tests.cpp
 *  @brief: Test CRingFormatHelper12
 */
#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/Asserter.h>
#include "Asserts.h"
#include "RingFormatHelper12.h"
#include "BufferTranslator.h"

class aTestSuite : public CppUnit::TestFixture {
    CPPUNIT_TEST_SUITE(aTestSuite);
    CPPUNIT_TEST(test_1);
    CPPUNIT_TEST_SUITE_END();
    
private:
    CRingFormatHelper* m_pHelper;
    BufferTranslator*  m_pTranslator;
public:
    void setUp() {
        m_pHelper  = new CRingFormatHelper12;
        m_pTranslator = new NonSwappingBufferTranslator;
    }
    void tearDown() {
        
    }
protected:
    void test_1();
};

CPPUNIT_TEST_SUITE_REGISTRATION(aTestSuite);

void aTestSuite::test_1()
{
}