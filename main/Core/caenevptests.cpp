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

/** @file:   caenevptests.cpp
 *  @brief:  Test Suite of CAENEventProcessor class.
 */
#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/Asserter.h>
#include "Asserts.h"
#include "CAENEventProcessor.h"
#include "CAENParameterMap.h"
#include "CAENModuleHits.h"
#include "CAENHit.h"              // Makes it easier to fake hits.
#include <string>
#include <map>
#include <config.h>
#include <Event.h>
#include <TreeParameter.h>
#include <Histogrammer.h>
#include <Globals.h>

class CAnalyzer;
class CBufferDecoder;

class aTestSuite : public CppUnit::TestFixture {
    CPPUNIT_TEST_SUITE(aTestSuite);
    CPPUNIT_TEST(test_1);
    CPPUNIT_TEST_SUITE_END();
    
private:
    CHistogrammer* m_pHistogrammer;
    CEvent*  m_pEvent;                  // Fake event.

    CAnalyzer*      m_pAnalyzer;
    CBufferDecoder* m_pDecoder;
public:
    void setUp() {
        m_pHistogrammer = new CHistogrammer;
        gpEventSink = m_pHistogrammer;
        m_pEvent = new CEvent;
        
        // The analyzer and buffer decoder are unused:
        
        m_pAnalyzer = nullptr;
        m_pDecoder  = nullptr;
    }
    void tearDown() {
        delete m_pEvent;
        delete m_pHistogrammer;
        gpEventSink = nullptr;
    }
protected:
    void test_1();
};

CPPUNIT_TEST_SUITE_REGISTRATION(aTestSuite);

void aTestSuite::test_1()
{
}