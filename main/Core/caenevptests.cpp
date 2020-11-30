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
#include <stdint.h>

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
    TestProcessor();
};

/**
 * TestProcessor constructor
 *    Creates the three parameter maps and registers them for sid 1-3
 *    with dynamic maps.
 */
TestProcessor::TestProcessor() 
{
    setTestMode();
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

class caenevptest : public CppUnit::TestFixture {
    CPPUNIT_TEST_SUITE(caenevptest);
    CPPUNIT_TEST(parse_1);
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
    void parse_1();
};

CPPUNIT_TEST_SUITE_REGISTRATION(caenevptest);

void caenevptest::parse_1()
{
    // Empty event fills nothing:
    
    TestProcessor p;
    
    // PHA1
    
    CTreeParameterArray pha_t("pha_t", 16, 0);
    CTreeParameterArray pha_e("pha_e", 16, 0);
    CTreeParameterArray pha_ex1("pha_ex1", 16, 0);
    CTreeParameterArray pha_ex2("pha_ex2", 16, 0);
    
    // PSD1
    
    CTreeParameterArray psd1_t("psd1_t", 16, 0);
    CTreeParameterArray psd1_s("psd1_s", 16, 0);
    CTreeParameterArray psd1_l("psd1_l", 16, 0);
    CTreeParameterArray psd1_b("psd1_b", 16, 0);
    CTreeParameterArray psd1_p("psd1_p", 16, 0);
    
    // PSD2
    
    CTreeParameterArray psd2_t("psd2_t", 16, 0);
    CTreeParameterArray psd2_s("psd2_s", 16, 0);
    CTreeParameterArray psd2_l("psd2_l", 16, 0);
    CTreeParameterArray psd2_b("psd2_b", 16, 0);
    CTreeParameterArray psd2_p("psd2_p", 16, 0);
    
    // Bind it all
    
    CTreeParameter::BindParameters();
    CTreeParameter::setEvent(*m_pEvent);
    
    
    // An empty event:
    
    uint32_t event(sizeof(uint32_t));
    p(&event, *m_pEvent, *m_pAnalyzer, *m_pDecoder);
    
    // Nothing gets set:
    
    for (int i = 0; i < 16; i++) {
        ASSERT(!pha_t[i].isValid());
        ASSERT(!pha_e[i].isValid());
        ASSERT(!pha_ex1[i].isValid());
        ASSERT(!pha_ex2[i].isValid());
        
        ASSERT(!psd1_s[i].isValid());
        ASSERT(!psd1_l[i].isValid());
        ASSERT(!psd1_b[i].isValid());
        ASSERT(!psd1_p[i].isValid());
        ASSERT(!psd1_t[i].isValid());
        
        ASSERT(!psd2_t[i].isValid());
        ASSERT(!psd2_s[i].isValid());
        ASSERT(!psd2_l[i].isValid());
        ASSERT(!psd2_b[i].isValid());
        ASSERT(!psd2_p[i].isValid());
        
    }
}