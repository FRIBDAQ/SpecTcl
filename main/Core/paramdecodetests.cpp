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
// Need to reset the tree parameter map between tests;

#include <CTreeParameter.h>
#include <CTreeParameterArray.h>
#include "TreeTestSupport.h"
#include <sstream>
#include <iomanip>


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
    CPPUNIT_TEST(obs_2);
    CPPUNIT_TEST(obs_3);
    CPPUNIT_TEST(pdef_1);
    CPPUNIT_TEST(pdef_2);
    CPPUNIT_TEST(pdef_3);
    CPPUNIT_TEST(getvar_1);
    CPPUNIT_TEST(getvar_2);
    CPPUNIT_TEST(event_1);
    CPPUNIT_TEST(event_2);
    CPPUNIT_TEST_SUITE_END();
protected:
    void construct_1();
    void var_1();
    void var_2();
    void vlookup_1();
    void obs_1();
    void obs_2();
    void obs_3();
    void pdef_1();
    void pdef_2();
    void pdef_3();
    void getvar_1();
    void getvar_2();
    void event_1();
    void event_2();
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
        
        TreeTestSupport::ClearMap();
    }

private:
    void initEvent(pParameterItem pItem);
    pParameterValue addPvalue(
        pParameterItem pItem, pParameterValue pNext,
        std::uint32_t id, double value
    );
    
    void initVar(pVariableItem pItem);
    pVariable addVarDef(
        pVariableItem pItem, pVariable pVar,
        const char* name, const char* units, double value
    );
    void initPdef(pParameterDefinitions pItem);
    pParameterDefinition addPdef(
        pParameterDefinitions pItem, pParameterDefinition pNext,
        const char* pName, std::uint32_t id
    );
};
// Initialize an ring item that contains parameter values:

void
PDecodeTest::initEvent(pParameterItem pItem) {
    pItem->s_header.s_size = sizeof(RingItemHeader) +
        sizeof(std::uint64_t) + sizeof(std::uint32_t);
    pItem->s_header.s_type = PARAMETER_DATA;
    pItem->s_header.s_unused = sizeof(std::uint32_t);
    
    pItem->s_triggerCount = 1;
    pItem->s_parameterCount = 0;
}
// Add a parameter value to a parmeter item:
// pItem - points to the item.
// pNext - points to where to put the parameter id/value pair.
// id  -  parameter id
// value - parameter value.
// Returns - pointer to where to put the next item.
//
pParameterValue
PDecodeTest::addPvalue(
    pParameterItem pItem, pParameterValue pNext,
    std::uint32_t id, double value
) {
    // Add the item:
    
    pNext->s_number = id;
    pNext->s_value = value;
    pNext++;
    
    // Adjust the book keeping:
    
    pItem->s_header.s_size += sizeof(ParameterValue);
    pItem->s_parameterCount++;
    
    return pNext;
}

// Initialize a parameter definition item:

void
PDecodeTest::initPdef(pParameterDefinitions pItem) {
    pItem->s_header.s_size = sizeof(RingItemHeader) + sizeof(std::uint32_t);
    pItem->s_header.s_type = PARAMETER_DEFINITIONS;
    pItem->s_header.s_unused = sizeof(std::uint32_t);
    pItem->s_numParameters = 0;
}
// Add a new parameter definition to ta pdef ring item:
// pItem - points to item.
// pNext - points to the next byte of available storage for the item.
// pName - the parameter name.
// id    - The parameter id.
// Returns a pointer to the next unused byte.

pParameterDefinition
PDecodeTest::addPdef(
     pParameterDefinitions pItem, pParameterDefinition pNext,
    const char* pName, std::uint32_t id
) {
    pNext->s_parameterNumber = id;
    auto returnValue = strcpy(pNext->s_parameterName, pName) + strlen(pName) + 1;
    
    // Adjust the book keeping:
    
    pItem->s_header.s_size += sizeof(ParameterDefinition) + strlen(pName);
    pItem->s_numParameters++;
    
    return reinterpret_cast<pParameterDefinition>(returnValue);
}



// Initialize a variable item:

void
PDecodeTest::initVar(pVariableItem pItem) {
    pItem->s_header.s_type = VARIABLE_VALUES;
    pItem->s_header.s_size = sizeof(RingItemHeader) + sizeof(std::uint32_t);
    pItem->s_header.s_unused = sizeof(std::uint32_t);
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
// Test that observer is invoked if unrecognized item:

void PDecodeTest::obs_2() {
    class Obs : public spectcl::ParameterDecoder::Observer {
    public:
        bool called;
        Obs() : called(false){}
        virtual bool operator()(spectcl::ParameterDecoder& d, const void* p) {
            called = true;
            return true;
        }
    };
    
    Obs o;
    m_pDecoder->setObserver(&o);
    RingItemHeader hdr = {
        sizeof(RingItemHeader),
        0x12345678,
        sizeof(std::uint32_t)
    };
    ASSERT((*m_pDecoder)(&hdr));
    ASSERT(o.called);
}
// Really be sure that operator() returns what the observer returned:

void PDecodeTest::obs_3() {
    class Obs : public spectcl::ParameterDecoder::Observer {
    public:
        bool called;
        Obs() : called(false){}
        virtual bool operator()(spectcl::ParameterDecoder& d, const void* p) {
            called = true;
            return false;
        }
    };
    
    Obs o;
    m_pDecoder->setObserver(&o);
    RingItemHeader hdr = {
        sizeof(RingItemHeader),
        0x12345678,
        sizeof(std::uint32_t)
    };
    ASSERT(!(*m_pDecoder)(&hdr));
}
// Can define a single parameter and the mapping is made:
//
void PDecodeTest::pdef_1() {
    union {
        ParameterDefinitions item;
        std::uint8_t storage[1000];
    } data;
    
    initPdef(&data.item);
    auto p = data.item.s_parameters;
    addPdef(&data.item, p, "parameter", 10);
    
    ASSERT((*m_pDecoder)(&data));
    
    // Map should contain 10:
    
    EQ(size_t(11), m_pDecoder->m_parameterMap.size());
    auto param = m_pDecoder->m_parameterMap[10];
    ASSERT(param != nullptr);
    EQ(std::string("parameter"), param->getName());
}
// Map to an existing parameter works correctly:

void PDecodeTest::pdef_2() {
    CTreeParameter existing("existing.parameter");
    CTreeParameter::BindParameters();   // gives the existing an id.
    
    union {
        ParameterDefinitions item;
        std::uint8_t storage[1000];
    } data;
    
    initPdef(&data.item);
    auto p = data.item.s_parameters;
    addPdef(&data.item, p, "existing.parameter", 10);
    
    ASSERT((*m_pDecoder)(&data));
    EQ(size_t(11), m_pDecoder->m_parameterMap.size());
    auto param = m_pDecoder->m_parameterMap[10];
    ASSERT(param != nullptr);
    
    // The parameters match in id and name:
    
    EQ(existing.getId(), param->getId());
    EQ(existing.getName(), param->getName());
}
// For grins and giggles a tree parameter array one item at a time:
//
void PDecodeTest::pdef_3() {
    CTreeParameterArray existing("base", 10, 0);  // 00, 01, ...09 I hope.
    
    union {
        ParameterDefinitions item;
        std::uint8_t storage[1000];
    } data;
    initPdef(&data.item);
    auto p = data.item.s_parameters;
    
    for (int i =0; i < 10; i++) {
        std::stringstream sname;
        sname << "base." << std::setw(2) << std::setfill('0') << i;
        std::string name = sname.str();
        p = addPdef(&data.item, p, name.c_str(), i);
    }
    
    (*m_pDecoder)(&data);
    EQ(size_t(10), m_pDecoder->m_parameterMap.size());
    for (int i =0; i < 10; i++) {
        auto param = m_pDecoder->m_parameterMap[i];
        ASSERT(param != nullptr);
        EQ(existing[i].getName(), param->getName());
        EQ(existing[i].getId(), param->getId());
    }
}
//getVariableDefinition for nonexistent returns nullptr.

void PDecodeTest::getvar_1() {
    // Make some variables:
    
    union {
        std::uint8_t raw[1000];
        VariableItem item;
    } data;
    initVar(&data.item);
    auto p = data.item.s_variables;
    p = addVarDef(&data.item, p, "var.1", "mm", 1.234);
    p = addVarDef(&data.item, p, "var.2", "degrees", 45.0);
    p = addVarDef(&data.item, p, "var.3", "", 1.414);
    
    ASSERT((*m_pDecoder)(&data));
    
    ASSERT(m_pDecoder->getVariableDefinition("var.4") == nullptr);
    
}
// Can get one that does exist:
void PDecodeTest::getvar_2() {
    // make some vars:
    //
    union {
        std::uint8_t raw[1000];
        VariableItem item;
    } data;
    initVar(&data.item);
    auto p = data.item.s_variables;
    p = addVarDef(&data.item, p, "var.1", "mm", 1.234);
    p = addVarDef(&data.item, p, "var.2", "degrees", 45.0);
    p = addVarDef(&data.item, p, "var.3", "", 1.414);
    
    ASSERT((*m_pDecoder)(&data));
    
    auto pV = m_pDecoder->getVariableDefinition("var.2");
    ASSERT(pV != nullptr);
    EQ(std::string("degrees"), pV->s_units);
    EQ(45.0, pV->s_value);
}
// An event with a single parameter value that maps to a local parameter.

void PDecodeTest::event_1() {
    // Define a local and a data stream parameter named "parameter"
    CTreeParameter param("parameter");
    CTreeParameter::BindParameters();
    union {
        ParameterDefinitions item;
        std::uint8_t storage[1000];
    } data;
    
    initPdef(&data.item);
    auto p = data.item.s_parameters;
    addPdef(&data.item, p, "parameter", 10);
    ASSERT((*m_pDecoder)(&data));
    
    // Define an event that only has a value for that parameter then dispatch it:
    
    union {
        std::uint8_t raw[1000];
        ParameterItem item;
    } event;
    
    initEvent(&event.item);
    auto pd = event.item.s_parameters;
    addPvalue(&event.item, pd, 10, 1.2345);
    
    ASSERT((*m_pDecoder)(&event));   // Process the event.
    
    // The recording sink should have an item with the id of the local variable
    // and the value 1.2345:
    
    EQ(size_t(1), m_pSink->m_event.size());
    auto ep = m_pSink->m_event[0];
    EQ(std::uint32_t(param.getId()), ep.first);
    EQ(1.2345, ep.second);
    
}
// an event with several parameters:

void PDecodeTest::event_2() {
    CTreeParameterArray params("params", 10, 0);  // 00 ..09
    CTreeParameter::BindParameters();
    union {
        ParameterDefinitions item;
        std::uint8_t storage[1000];
    } data;
    initPdef(&data.item);
    auto p = data.item.s_parameters;
    
    for (int i =0; i < 10; i++) {
        std::stringstream sname;
        sname << "params." << std::setw(2) << std::setfill('0') << i;
        std::string name = sname.str();
        p = addPdef(&data.item, p, name.c_str(), i);
    }
    ASSERT((*m_pDecoder)(&data));
    
    union {
        std::uint8_t raw[1000];
        ParameterItem item;
    } event;
    
    initEvent(&event.item);
    auto pd = event.item.s_parameters;
    for (int i =0; i < 10; i++) {
        pd = addPvalue(&event.item, pd, i, 0.5*i);
    }
    ASSERT((*m_pDecoder)(&event));
    
    // Should be 10 parameters in the event with the same indices as
    // the params array with values 0.0 - 4.5 in steps of 0.5.
    
    EQ(size_t(10), m_pSink->m_event.size());
    for (int i =0; i < 10; i++) {
        auto datum = m_pSink->m_event[i];
        EQ(std::uint32_t(params[i].getId()), datum.first);
        EQ(0.5*i, datum.second);
    }
    
}