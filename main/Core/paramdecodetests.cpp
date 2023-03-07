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
#include <string.h>
#include <string>

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
    CPPUNIT_TEST(var_1);
    CPPUNIT_TEST(var_2);
    CPPUNIT_TEST(vlookup_1);
    CPPUNIT_TEST(obs_1);
    
    CPPUNIT_TEST_SUITE_END();
protected:
    void construct_1();
    void var_1();
    void var_2();
    void vlookup_1();
    void obs_1();
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

private:
    void initVar(pVariableItem pItem);
    pVariable addVarDef(
        pVariableItem pItem, pVariable pVar,
        const char* name, const char* units, double value
    );
};

// Initialize a variable item:

void
PDecodeTest::initVar(pVariableItem pItem) {
    pItem->s_header.s_type = VARIABLE_VALUES;
    pItem->s_header.s_size = sizeof(RingItemHeader) + sizeof(std::uint32_t);
    pItem->s_numVars = 0;
}

// Add a variable to a variable item.
//   pItem - points to the item so sizes can be adjusted.
//   pVar  - Is the next free storage
//   name, units, value define the variable.
// returns the next free storage:

pVariable
PDecodeTest::addVarDef(
        pVariableItem pItem, pVariable pVar,
        const char* name, const char* units, double value
    ) {
    
    pVar->s_value = value;
    strncpy(pVar->s_variableUnits, units, MAX_UNITS_LENGTH);
    pVariable result = reinterpret_cast<pVariable>(
        strcpy(pVar->s_variableName, name) + strlen(name) + 1
    );
    
    // Adjust sizes:
    
    pItem->s_numVars++;
    pItem->s_header.s_size += sizeof(Variable) + strlen(name);
    
    return result;
    
}

CPPUNIT_TEST_SUITE_REGISTRATION(PDecodeTest);

// Maps and vars are empty:
void PDecodeTest::construct_1()
{
    EQ(size_t(0), m_pDecoder->m_parameterMap.size());
    EQ(size_t(0), m_pDecoder->m_variableDict.size());
    ASSERT(m_pDecoder->m_pObserver == nullptr);
    EQ(UInt_t(1), m_pDecoder->m_el.size());
}
// Can add a variable definition to the parameter decoder.
void PDecodeTest::var_1() {
    // Make a variable item:
    
    union {
        std::uint8_t raw[1000];
        VariableItem item;
    } data;
    initVar(&data.item);
    auto p = data.item.s_variables;
    p = addVarDef(&data.item, p, "var.1", "mm", 1.234);
    
    bool result = (*m_pDecoder)(&data);
    ASSERT(result);
    
    // Should be in the variable dictioanry:
    
    EQ(size_t(1), m_pDecoder->m_variableDict.size());
    ASSERT(
        m_pDecoder->m_variableDict.find(std::string("var.1")) !=
        m_pDecoder->m_variableDict.end()
    );
    
    auto stored = m_pDecoder->m_variableDict[std::string("var.1")];
    EQ(std::string("mm"), stored.s_units);
    EQ(1.234, stored.s_value);
}

// Send a few variables to the decoder:
void PDecodeTest::var_2() {
    union {
        std::uint8_t raw[1000];
        VariableItem item;
    } data;
    initVar(&data.item);
    auto p = data.item.s_variables;
    p = addVarDef(&data.item, p, "var.1", "mm", 1.234);
    p = addVarDef(&data.item, p, "var.2", "degrees", 45.0);
    p = addVarDef(&data.item, p, "var.3", "", 1.414);
    
    (*m_pDecoder)(&data);
    
    // We'll believe that if it's in the dictionary it's probably correct
    // given var_1:
    
    auto& d = m_pDecoder->m_variableDict;
    ASSERT(d.find("var.1") != d.end());
    ASSERT(d.find("var.2") != d.end());
    ASSERT(d.find("var.3") != d.end());
}
// can lookup a variable definitions/value:

void PDecodeTest::vlookup_1() {
    union {
        std::uint8_t raw[1000];
        VariableItem item;
    } data;
    initVar(&data.item);
    auto p = data.item.s_variables;
    p = addVarDef(&data.item, p, "var.1", "mm", 1.234);
    p = addVarDef(&data.item, p, "var.2", "degrees", 45.0);
    p = addVarDef(&data.item, p, "var.3", "", 1.414);
    
    (*m_pDecoder)(&data);
    
    auto v1 = m_pDecoder->getVariableDefinition("var.1");
    ASSERT(v1 != nullptr);
    EQ(std::string("mm"), v1->s_units);
    EQ(1.234, v1->s_value);
    
    auto v2 = m_pDecoder->getVariableDefinition("var.2");
    ASSERT(v2 != nullptr);
    EQ(std::string("degrees"), v2->s_units);
    EQ(45.0, v2->s_value);
    
    auto v3 = m_pDecoder->getVariableDefinition("var.3");
    ASSERT(v3 != nullptr);
    EQ(std::string(""), v3->s_units);
    EQ(1.414, v3->s_value);
    
    // One that does not exist:
    
    auto v4 = m_pDecoder->getVariableDefinition("var.4");
    ASSERT(v4 == nullptr);
}
// Test that we can add an observer and get the initial nullptr back:

void PDecodeTest::obs_1() {
    class Obs  : public spectcl::ParameterDecoder::Observer {
        virtual bool operator()(spectcl::ParameterDecoder& d, const void* p) {
            return true;
        }
    };
    Obs o;
    auto old = m_pDecoder->setObserver(&o);
    ASSERT(old == nullptr);
    EQ((spectcl::ParameterDecoder::Observer*)&o, m_pDecoder->m_pObserver);
    Obs o2;
    old = m_pDecoder->setObserver(&o2);
    EQ((spectcl::ParameterDecoder::Observer*)&o, old);
}