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

/** @file:  
 *  @brief: 
 */
#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/Asserter.h>
#include "Asserts.h"
#include <Globals.h>
#include <EventSinkPipeline.h>
#include <EventSink.h>
#include <EventList.h>
#include <Event.h>
#include <AnalysisRingItems.h>
#include <ParameterDecoding.h>
#include <vector>
// Special event sink which just records the parmaeters of the first
// event:

class RecordingSink : public CEventSink {
public:
    std::vector<std::pair<uint32_t, double>> m_event;

        virtual void operator()(CEventList& rEvents) {
            CEvent& event = *(rEvents[0]);
            auto dv = event.getDopeVector();
            for (int i =0; i < dv.size(); i++) {
                auto index = dv[i];
                auto value = event[index];
                
                m_event.push_back({index, value});
            }
        }
};

class aTestSuite : public CppUnit::TestFixture {
    CPPUNIT_TEST_SUITE(aTestSuite);
    CPPUNIT_TEST(test_1);
    CPPUNIT_TEST_SUITE_END();
    
private:
    RecordingSink* m_pSink;
public:
    void setUp() {
        gpEventSinkPipeline = new CEventSinkPipeline;
        m_pSink = new RecordingSink;
        gpEventSinkPipeline->AddEventSink(*m_pSink, "Recording");
    }
    void tearDown() {
        delete gpEventSinkPipeline;
        delete m_pSink;
    }
protected:
    void test_1();
};

CPPUNIT_TEST_SUITE_REGISTRATION(aTestSuite);

void aTestSuite::test_1()
{
}