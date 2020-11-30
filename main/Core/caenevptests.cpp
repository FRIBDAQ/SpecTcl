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

/**
 * Here is our test CAENEventProcessor derived class.
 *  - it creates three parameter maps:
 *    * A PHA one.
 *    * A PSD one with multiplier 1 (500MHz module).
 *    * A PSD one with multiplier 2 (250MHz module).
 */
class TestProcessor : public CAENEventProcessor
{
public:
    size_t m_nDisposeCount;              // # times dispose was called.
public:
    TestProcessor();
    void disposeMapEntry(int sid, CAENParameterMap* pMap);
};

/**
 * TestProcessor constructor
 *    Creates the three parameter maps and registers them for sid 1-3
 *    with dynamic maps.
 */
TestProcessor::TestProcessor() :
    m_nDisposeCount(0)
{
    addParameterMap(
        1,
        new CAENPHAArrayMapper("pha_t", "pha_e", "pha_ex1", "pha_ex2")
    );
    addParameterMap(
        2,
        new CAENPSDArrayMapper("psd1_t", "psd1_s", "psd1_l", "psd1_b", "psd1_p"),
        1
    );                      // 500MHz PSD module.
    addParameterMap(
        2,
        new CAENPSDArrayMapper("psd2_t", "psd2_s", "psd2_l", "psd2_b", "psd1_p"),
        2
    );                      // 250MHz PSD module.
}
/**
 *   disposeMapEntry
 *   
 * Provide code to destroy the parameter maps and count the number of
 *  times called:
 *    @param sid - the source id for the event processor.
 *    @param pMap - Pointer to the map to destroy.
 */
void
TestProcessor::disposeMapEntry(int sid, CAENParameterMap* pMap)
{
    delete pMap;
    m_nDisposeCount++;
}

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