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

// Want to examine the internal of ParameterDecoding so this trick rather than
// friendness:

#define private public
#include <ParameterDecoding.h>
#undef private

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

using namespace frib::analysis;

class PDecodeTest : public CppUnit::TestFixture {
    CPPUNIT_TEST_SUITE(PDecodeTest);
    CPPUNIT_TEST(construct_1);
    CPPUNIT_TEST_SUITE_END();
    
private:
    RecordingSink* m_pSink;
    spectcl::ParameterDecoder* m_pDecoder;
public:
    void setUp() {
        gpEventSinkPipeline = new CEventSinkPipeline;
        m_pSink = new RecordingSink;
        gpEventSinkPipeline->AddEventSink(*m_pSink, "Recording");
        m_pDecoder = new spectcl::ParameterDecoder;
    }
    void tearDown() {
        delete gpEventSinkPipeline;
        delete m_pSink;
        delete m_pDecoder;
    }
protected:
    void construct_1();
};

CPPUNIT_TEST_SUITE_REGISTRATION(PDecodeTest);

// Maps and vars are empty:
void PDecodeTest::construct_1()
{
    EQ(size_t(0), m_pDecoder->m_parameterMap.size());
    EQ(size_t(0), m_pDecoder->m_variableDict.size());
    ASSERT(m_pDecoder->m_pObserver == nullptr);
    EQ(UInt_t(1), m_pDecoder->m_el.size());
}