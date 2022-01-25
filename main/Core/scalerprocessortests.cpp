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

/** @file:  scalerprocessortests.cpp
 *  @brief: 
 */
#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/Asserter.h>
#include "Asserts.h"
#include "CScalerProcessor.h"
#include "TCLInterpreter.h"

class spest : public CppUnit::TestFixture {
    CPPUNIT_TEST_SUITE(spest);
    CPPUNIT_TEST(test_1);
    CPPUNIT_TEST_SUITE_END();
    
private:
    CTCLInterpreter* m_pInterp;
    ScalerProcessor* m_pProcessor;
public:
    void setUp() {
        m_pInterp = new CTCLInterpreter;
        m_pProcessor = new ScalerProcessor(*m_pInterp);
    }
    void tearDown() {
        delete m_pProcessor;
        delete m_pInterp;
    }
protected:
    void test_1();
};

CPPUNIT_TEST_SUITE_REGISTRATION(spest);

void spest::test_1()
{
}