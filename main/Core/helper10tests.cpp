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

/** @file:  helper10tests.cpp
 *  @brief: Test suite for RingFormatHelper10.
 */
#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/Asserter.h>
#include "Asserts.h"
#include "DataFormatPre11.h"
#include "RingFormatHelper10.h"
#include "BufferTranslator.h"
#include <string.h>
#include <time.h>
#include <vector>

class helper10test : public CppUnit::TestFixture {
    CPPUNIT_TEST_SUITE(helper10test);
    CPPUNIT_TEST(bodyhdr);
    CPPUNIT_TEST(bodyptr);
    CPPUNIT_TEST(exbodyhdr);
    
    CPPUNIT_TEST(title);
    CPPUNIT_TEST(run);
    CPPUNIT_TEST(badstate_1);
    CPPUNIT_TEST(badstate_2);
    
    CPPUNIT_TEST(stringcount);
    CPPUNIT_TEST(strings);
    CPPUNIT_TEST(badstring_1);
    CPPUNIT_TEST(badstring_2);
    
    CPPUNIT_TEST(scalercount);
    CPPUNIT_TEST(scalers);
    CPPUNIT_TEST(badscalers_1);
    CPPUNIT_TEST(badscalers_2);
    
    CPPUNIT_TEST(triggers);
    CPPUNIT_TEST(badtriggers);
    CPPUNIT_TEST_SUITE_END();
    
private:
    CRingFormatHelper10* m_pHelper;
    BufferTranslator*    m_pTranslator;
public:
    void setUp() {
        m_pHelper = new CRingFormatHelper10;
        m_pTranslator = new NonSwappingBufferTranslator;
    }   
    void tearDown() {
        delete m_pHelper;
        delete m_pTranslator;
    }
protected:
    void bodyhdr();
    void bodyptr();
    void exbodyhdr();
    
    void title();
    void run();
    void badstate_1();
    void badstate_2();
    
    void stringcount();
    void strings();
    void badstring_1();
    void badstring_2();
    
    void scalercount();
    void scalers();
    void badscalers_1();
    void badscalers_2();
    
    void triggers();
    void badtriggers();
};
// as with the text item fillers, the caller must ensure p points to an appropriately
// sized chu nk of memory.

static void fillScalers(NSCLDAQ10::pScalerItem p, const std::vector<uint32_t>& scalers)
{
    p->s_header.s_type = NSCLDAQ10::INCREMENTAL_SCALERS;
    p->s_header.s_size = sizeof(NSCLDAQ10::ScalerItem) + (scalers.size() - 1)*sizeof(uint32_t);
    p->s_intervalStartOffset =0;
    p->s_intervalEndOffset = 2;
    p->s_timestamp = time(nullptr);
    p->s_scalerCount = scalers.size();
    memcpy(p->s_scalers, scalers.data(), scalers.size()*sizeof(uint32_t));
}

static size_t stringsizes(const std::vector<std::string>& strings)
{
    size_t result(0);
    for(int i = 0; i < strings.size(); i++) {
        result += strings[i].size() + 1;             // +1 is null terminator.
    }
    return result;
}
// note caller must ensure the text item is extended with sufficient
// storage to store the strings.

static void fillStrings(NSCLDAQ10::pTextItem p, const std::vector<std::string>& strings)
{
    p->s_header.s_size = sizeof(NSCLDAQ10::TextItem) + stringsizes(strings) - 1;
    p->s_header.s_type = NSCLDAQ10::MONITORED_VARIABLES;
    p->s_timeOffset = 10;
    p->s_timestamp = time(nullptr);
    p->s_stringCount = strings.size();
    char* s = p->s_strings;
    for (int i =0; i < strings.size(); i++) {
        strcpy(s, strings[i].c_str());
        s += strlen(s) + 1;
    }
}

static void fillState(NSCLDAQ10::pStateChangeItem p)
{
    p->s_header.s_size = sizeof(NSCLDAQ10::StateChangeItem);
    p->s_header.s_type = NSCLDAQ10::BEGIN_RUN;
    p->s_runNumber     = 10;
    p->s_timeOffset    = 0;
    p->s_Timestamp    = time(nullptr);
    const char* title ="This is a test title";
    strncpy(p->s_title, title, strlen(title)+1 );
}

CPPUNIT_TEST_SUITE_REGISTRATION(helper10test);
// No ring items in v10 have body headers.
void helper10test::bodyhdr()
{
    NSCLDAQ10::RingItemHeader item;
    item.s_size = sizeof(NSCLDAQ10::RingItemHeader);
    item.s_type = NSCLDAQ10::FIRST_USER_ITEM_CODE;
    
    ASSERT(!m_pHelper->hasBodyHeader(&item));
}
// the body of a v10 ring item always follows the ring item header.

void helper10test::bodyptr()
{
    NSCLDAQ10::RingItem item;
    item.s_header.s_size = sizeof(NSCLDAQ10::RingItem);
    item.s_header.s_type = NSCLDAQ10::FIRST_USER_ITEM_CODE;
    
    uint8_t* bp = reinterpret_cast<uint8_t*>(m_pHelper->getBodyPointer(&item));
    EQ(&item.s_body[0], bp);
}
// body header pointers are always null:

void helper10test::exbodyhdr()
{
    NSCLDAQ10::RingItem item;
    item.s_header.s_size = sizeof(NSCLDAQ10::RingItem);
    item.s_header.s_type = NSCLDAQ10::FIRST_USER_ITEM_CODE;
    
    ASSERT(m_pHelper->getBodyHeaderPointer(&item) == nullptr);
}
// Get the title from a state change item.
void helper10test::title()
{
    NSCLDAQ10::StateChangeItem item;
    fillState(&item);
    EQ(std::string("This is a test title"), m_pHelper->getTitle(&item));
}
// get the run number from a state change item
void helper10test::run()
{
    NSCLDAQ10::StateChangeItem item;
    fillState(&item);
    EQ(unsigned(10), m_pHelper->getRunNumber(&item, m_pTranslator));

}
// both getTitle and getRunNumber throw std::string if the item is not
// a state change item type:

void helper10test::badstate_1()
{
    NSCLDAQ10::StateChangeItem item;
    item.s_header.s_size= sizeof(NSCLDAQ10::StateChangeItem);
    item.s_header.s_type = NSCLDAQ10::FIRST_USER_ITEM_CODE;
    
    CPPUNIT_ASSERT_THROW(
        m_pHelper->getTitle(&item),
        std::string
    );
}
void helper10test::badstate_2()
{
    NSCLDAQ10::StateChangeItem item;
    item.s_header.s_size= sizeof(NSCLDAQ10::StateChangeItem);
    item.s_header.s_type = NSCLDAQ10::FIRST_USER_ITEM_CODE;
    
    CPPUNIT_ASSERT_THROW(
        m_pHelper->getRunNumber(&item,  m_pTranslator),
        std::string
    );
}
// Get string count from a text item:

void helper10test::stringcount()
{
    // Apologies to Dr. Suess.
    std::vector<std::string> theStrings = {
        "one string", "two string", "three string", "four",
        "red string", "blue string", "green string", "more"
    }; 
    struct {
        NSCLDAQ10::TextItem item;
        char strings[200];
    } item;

    
    fillStrings(&(item.item), theStrings);
    EQ(unsigned(theStrings.size()), m_pHelper->getStringCount(&item, m_pTranslator));
}
// Get the strings themselves:

void helper10test::strings()
{
    // Apologies to Dr. Suess.
    std::vector<std::string> theStrings = {
        "one string", "two string", "three string", "four",
        "red string", "blue string", "green string", "more"
    };

    struct {
        NSCLDAQ10::TextItem item;
        char strings[200];
    } item;
    
    fillStrings(&(item.item), theStrings);
    auto gotten = m_pHelper->getStrings(&item, m_pTranslator);
    EQ(theStrings.size(), gotten.size());
    for (int i =0; i < theStrings.size(); i++) {
        EQ(theStrings[i], gotten[i]);
    }
}
// Invalid item types lead to string exceptions.
void helper10test::badstring_1()
{
    // Apologies to Dr. Suess.
    std::vector<std::string> theStrings = {
        "one string", "two string", "three string", "four",
        "red string", "blue string", "green string", "more"
    };
#pragma pack(push, 1)    
    struct {
        NSCLDAQ10::TextItem item;
        char strings[200];
    } item;
#pragma pack(pop)
    
    fillStrings(&(item.item), theStrings);
    item.item.s_header.s_type= NSCLDAQ10::FIRST_USER_ITEM_CODE;
    CPPUNIT_ASSERT_THROW(
        m_pHelper->getStringCount(&item, m_pTranslator),
        std::string
    );
}
void helper10test::badstring_2()
{
    // Apologies to Dr. Suess.
    std::vector<std::string> theStrings = {
        "one string", "two string", "three string", "four",
        "red string", "blue string", "green string", "more"
    };
#pragma pack(push, 1)    
    struct {
        NSCLDAQ10::TextItem item;
        char strings[200];
    } item;
#pragma pack(pop)
    
    fillStrings(&(item.item), theStrings);
    item.item.s_header.s_type= NSCLDAQ10::FIRST_USER_ITEM_CODE;
    CPPUNIT_ASSERT_THROW(
        m_pHelper->getStrings(&item, m_pTranslator),
        std::string
    );
}
// Get scaler count from a scaler item:

void helper10test::scalercount()
{
#pragma pack(push, 1)
    struct {
        NSCLDAQ10::ScalerItem base;
        uint32_t  additional_scalers[31];
    } item;
#pragma pack(pop)
    std::vector<uint32_t> scalers;
    for (int i =0; i < 32; i++) {
        scalers.push_back(10*i);
    }
    fillScalers(&item.base, scalers);
    
    EQ(unsigned(scalers.size()), m_pHelper->getScalerCount(&item, m_pTranslator));
}
// get scalers themselves:

void helper10test::scalers()
{
#pragma pack(push, 1)
    struct {
        NSCLDAQ10::ScalerItem base;
        uint32_t  additional_scalers[31];
    } item;
#pragma pack(pop)
    std::vector<uint32_t> scalers;
    for (int i =0; i < 32; i++) {
        scalers.push_back(10*i);
    }
    fillScalers(&item.base, scalers);
    
    auto result = m_pHelper->getScalers(&item, m_pTranslator);
    EQ(scalers.size(), result.size());
    for (int i =0; i < scalers.size(); i++) {
        EQ(scalers[i], result[i]);
    }
}
// Bad item type throws strings.

void helper10test::badscalers_1()
{
#pragma pack(push, 1)
    struct {
        NSCLDAQ10::ScalerItem base;
        uint32_t  additional_scalers[31];
    } item;
#pragma pack(pop)
    std::vector<uint32_t> scalers;
    for (int i =0; i < 32; i++) {
        scalers.push_back(10*i);
    }
    fillScalers(&item.base, scalers);
    item.base.s_header.s_type = NSCLDAQ10::FIRST_USER_ITEM_CODE;
    
    CPPUNIT_ASSERT_THROW(
        m_pHelper->getScalerCount(&item, m_pTranslator),
        std::string
    );
}
void helper10test::badscalers_2()
{
#pragma pack(push, 1)
    struct {
        NSCLDAQ10::ScalerItem base;
        uint32_t  additional_scalers[31];
    } item;
#pragma pack(pop)
    std::vector<uint32_t> scalers;
    for (int i =0; i < 32; i++) {
        scalers.push_back(10*i);
    }
    fillScalers(&item.base, scalers);
    item.base.s_header.s_type = NSCLDAQ10::FIRST_USER_ITEM_CODE;
    
    CPPUNIT_ASSERT_THROW(
        m_pHelper->getScalers(&item, m_pTranslator),
        std::string
    );    
}
// Get the trigger count from a PhysicsEventCountItem using helper:

void helper10test::triggers()
{
    NSCLDAQ10::PhysicsEventCountItem item;
    item.s_header.s_size = sizeof(NSCLDAQ10::PhysicsEventCountItem);
    item.s_header.s_type = NSCLDAQ10::PHYSICS_EVENT_COUNT;
    item.s_timeOffset = 100;
    item.s_timestamp = time(nullptr);
    item.s_eventCount = 0x1234567890;
    
    EQ(uint64_t(0x1234567890), m_pHelper->getTriggerCount(&item, m_pTranslator));
}
// invalid type throws.
void helper10test::badtriggers()
{
    NSCLDAQ10::PhysicsEventCountItem item;
    item.s_header.s_size = sizeof(NSCLDAQ10::PhysicsEventCountItem);
    item.s_header.s_type = NSCLDAQ10::FIRST_USER_ITEM_CODE;
    item.s_timeOffset = 100;
    item.s_timestamp = time(nullptr);
    item.s_eventCount = 0x1234567890;
    
    CPPUNIT_ASSERT_THROW(
        m_pHelper->getTriggerCount(&item, m_pTranslator),
        std::string
    );
}
