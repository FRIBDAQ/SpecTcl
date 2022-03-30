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
#include "DataFormat12.h"
#include <time.h>
#include <string.h>


/**
 * a bit about the strategy.  12 has the following differences from 11:
 *  - some items have an original source id field.  We must test the ability
 *    to get stuff in those items past that field.
 *  - The field that is mbz in items with no body header is set to sizeof(uint32_t).
 *     we need to check out the ability to get the body and body header pointers
 *     for items withand without body headers again.  Otherwise, everything
 *     in the v11 tests should hold.
 */


class helper12test : public CppUnit::TestFixture {
    CPPUNIT_TEST_SUITE(helper12test);
    CPPUNIT_TEST(hashdr_1);
    CPPUNIT_TEST(hashdr_2);
    
    CPPUNIT_TEST(bodyptr_1);
    CPPUNIT_TEST(bodyptr_2);
    
    CPPUNIT_TEST(bodyhdrptr_1);
    CPPUNIT_TEST(bodyhdrptr_2);
    
    // Specific v11 overrides:
    
    CPPUNIT_TEST(title_1);
    CPPUNIT_TEST(title_2);
    CPPUNIT_TEST(title_3);
    
    CPPUNIT_TEST(strings_1);
    CPPUNIT_TEST(strings_2);
    CPPUNIT_TEST(strings_3);
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
    void hashdr_1();
    void hashdr_2();
    
    void bodyptr_1();
    void bodyptr_2();
    
    void bodyhdrptr_1();
    void bodyhdrptr_2();
    
    void title_1();
    void title_2();
    void title_3();
    
    void strings_1();
    void strings_2();
    void strings_3();
};

static const char* runTitle="This is the common run title";
static uint32_t    runNumber = 12;

CPPUNIT_TEST_SUITE_REGISTRATION(helper12test);

static void fillBodyHeader(NSCLDAQ12::BodyHeader& hdr)
{
    hdr.s_size = sizeof(NSCLDAQ12::BodyHeader);
    hdr.s_timestamp = 0x1234567890;
    hdr.s_sourceId  = 2;
    hdr.s_barrier   = 0;
}

static void fillStateChangeBody(NSCLDAQ12::pStateChangeItemBody pBody)
{
    pBody->s_runNumber = runNumber;
    pBody->s_timeOffset = 10;
    pBody->s_Timestamp = time(nullptr);
    pBody->s_offsetDivisor = 1;
    pBody->s_originalSid = 1;      // 12.0 original source id.
    strncpy(pBody->s_title, runTitle, TITLE_MAXSIZE+1);

}

static ptrdiff_t fillTextBody(NSCLDAQ12::TextItemBody& body, const std::vector<std::string>& strings)
{
    body.s_timeOffset = 10;;
    body.s_timestamp = time(nullptr);
    body.s_stringCount = strings.size();
    body.s_offsetDivisor = 1;
    body.s_originalSid = 1;
    char* p =        body.s_strings;
    
    for (int i =0; i < strings.size(); i++) {
        strcpy(p, strings[i].c_str());
        p += strlen(p) + 1;
    }
    
    uint8_t* pEnd = reinterpret_cast<uint8_t*>(p);
    uint8_t* pBeg = reinterpret_cast<uint8_t*>(&body);
    
    return pEnd - pBeg;
    
}

// Can detect that there's no body header in a 12 item:
void helper12test::hashdr_1()
{
    NSCLDAQ12::RingItem item;
    item.s_header.s_size = sizeof(NSCLDAQ12::RingItemHeader) + sizeof(uint32_t);
    item.s_header.s_type = NSCLDAQ12::FIRST_USER_ITEM_CODE;
    item.s_body.u_noBodyHeader.s_empty = sizeof(uint32_t);   // how 12 sets this.
    
    ASSERT(! m_pHelper->hasBodyHeader(&item));
}
// can detect there is a body header in a 12 item:

void helper12test::hashdr_2()
{
    NSCLDAQ12::RingItem item;
    item.s_header.s_size = sizeof(NSCLDAQ12::RingItemHeader) + sizeof(NSCLDAQ12::BodyHeader);
    item.s_header.s_type = NSCLDAQ12::FIRST_USER_ITEM_CODE;
    fillBodyHeader(item.s_body.u_hasBodyHeader.s_bodyHeader);
    ASSERT(m_pHelper->hasBodyHeader(&item));
}
// Get body pointer with no body header:

void helper12test::bodyptr_1()
{
    NSCLDAQ12::RingItem item;
    item.s_header.s_size = sizeof(NSCLDAQ12::RingItemHeader) + sizeof(uint32_t);
    item.s_header.s_type = NSCLDAQ12::FIRST_USER_ITEM_CODE;
    item.s_body.u_noBodyHeader.s_empty = sizeof(uint32_t);   // how 12 sets this.
    
    EQ(
        &item.s_body.u_noBodyHeader.s_body[0],
        reinterpret_cast<uint8_t*>(m_pHelper->getBodyPointer(&item))
    );
}
// Get body pointer with a header:

void helper12test::bodyptr_2()
{
    NSCLDAQ12::RingItem item;
    item.s_header.s_size = sizeof(NSCLDAQ12::RingItemHeader) + sizeof(NSCLDAQ12::BodyHeader);
    item.s_header.s_type = NSCLDAQ12::FIRST_USER_ITEM_CODE;
    fillBodyHeader(item.s_body.u_hasBodyHeader.s_bodyHeader);
    
    EQ(
        &item.s_body.u_hasBodyHeader.s_body[0],
        reinterpret_cast<uint8_t*>(m_pHelper->getBodyPointer(&item))
    );
}

// Non body header item gets null ptr:

void helper12test::bodyhdrptr_1()
{
    NSCLDAQ12::RingItem item;
    item.s_header.s_size = sizeof(NSCLDAQ12::RingItemHeader) + sizeof(uint32_t);
    item.s_header.s_type = NSCLDAQ12::FIRST_USER_ITEM_CODE;
    item.s_body.u_noBodyHeader.s_empty = sizeof(uint32_t);   // how 12 sets this.
    ASSERT(m_pHelper->getBodyHeaderPointer(&item) == nullptr);
}
void helper12test::bodyhdrptr_2()
{
    NSCLDAQ12::RingItem item;
    item.s_header.s_size = sizeof(NSCLDAQ12::RingItemHeader) + sizeof(NSCLDAQ12::BodyHeader);
    item.s_header.s_type = NSCLDAQ12::FIRST_USER_ITEM_CODE;
    fillBodyHeader(item.s_body.u_hasBodyHeader.s_bodyHeader);
    EQ(
        &item.s_body.u_hasBodyHeader.s_bodyHeader,
        reinterpret_cast<NSCLDAQ12::pBodyHeader>(m_pHelper->getBodyHeaderPointer(&item))
    );
}
// non body header state change:

void helper12test::title_1()
{
    NSCLDAQ12::StateChangeItem item;
    item.s_header.s_size =
        sizeof(NSCLDAQ12::RingItemHeader) + sizeof(uint32_t)
        + sizeof(NSCLDAQ12::StateChangeItemBody);
    item.s_header.s_type = NSCLDAQ12::BEGIN_RUN;
    item.s_body.u_noBodyHeader.s_empty=  sizeof(uint32_t);
    fillStateChangeBody(&(item.s_body.u_noBodyHeader.s_body));
    EQ(std::string(runTitle), m_pHelper->getTitle(&item));
}

// body header state change.

void helper12test::title_2()
{
    NSCLDAQ12::StateChangeItem item;
    item.s_header.s_size =
        sizeof(NSCLDAQ12::RingItemHeader) + sizeof(NSCLDAQ12::BodyHeader) +
        sizeof(NSCLDAQ12::StateChangeItemBody);
    item.s_header.s_type = NSCLDAQ12::BEGIN_RUN;
    fillBodyHeader(item.s_body.u_hasBodyHeader.s_bodyHeader);
    fillStateChangeBody(&(item.s_body.u_hasBodyHeader.s_body));
    EQ(std::string(runTitle), m_pHelper->getTitle(&item));
}
// Non state change item throws:

void helper12test::title_3()
{
    NSCLDAQ12::StateChangeItem item;
    item.s_header.s_size =
        sizeof(NSCLDAQ12::RingItemHeader) + sizeof(uint32_t)
        + sizeof(NSCLDAQ12::StateChangeItemBody);
    item.s_header.s_type = NSCLDAQ12::FIRST_USER_ITEM_CODE;
    item.s_body.u_noBodyHeader.s_empty=  sizeof(uint32_t);
    fillStateChangeBody(&(item.s_body.u_noBodyHeader.s_body));
    
    CPPUNIT_ASSERT_THROW(
        m_pHelper->getTitle(&item),
        std::string
    );
}
// get strings from a text item with no body header.

void helper12test::strings_1()
{
#pragma pack(push, 1)
    struct {
        NSCLDAQ12::TextItem  s_item;
        char      s_moreStrings[500];
    } item;
#pragma pack(pop)
     // Apologies to Dr. Suess.
    std::vector<std::string> strings = {
        "one string", "two string", "three string", "four",
        "red string", "blue string", "green string", "more"
    };
    
    item.s_item.s_header.s_type = NSCLDAQ12::MONITORED_VARIABLES;
    item.s_item.s_body.u_noBodyHeader.s_empty = sizeof(uint32_t);
    
    uint32_t bodySize = fillTextBody(
        item.s_item.s_body.u_noBodyHeader.s_body, strings
    );
    item.s_item.s_header.s_size =
        sizeof(NSCLDAQ12::RingItemHeader) + sizeof(uint32_t)
        + bodySize;
    std::vector<std::string> result = m_pHelper->getStrings(&item, m_pTranslator);
    
    EQ(strings.size(), result.size());
    for (int i =0; i < strings.size(); i++) {
        EQ(strings[i], result[i]);
    }
}
// get strings from a text item with a body header

void helper12test::strings_2()
{
#pragma pack(push, 1)
    struct {
        NSCLDAQ12::TextItem  s_item;
        char      s_moreStrings[500];
    } item;
#pragma pack(pop)
     // Apologies to Dr. Suess.
    std::vector<std::string> strings = {
        "one string", "two string", "three string", "four",
        "red string", "blue string", "green string", "more"
    };
    
    item.s_item.s_header.s_type = NSCLDAQ12::MONITORED_VARIABLES;
    fillBodyHeader(item.s_item.s_body.u_hasBodyHeader.s_bodyHeader);
    
    uint32_t bodySize = fillTextBody(
        item.s_item.s_body.u_hasBodyHeader.s_body, strings
    );
    item.s_item.s_header.s_size =
        sizeof(NSCLDAQ12::RingItemHeader) + sizeof(NSCLDAQ12::BodyHeader)
        + bodySize;
    std::vector<std::string> result = m_pHelper->getStrings(&item, m_pTranslator);
    
    EQ(strings.size(), result.size());
    for (int i =0; i < strings.size(); i++) {
        EQ(strings[i], result[i]);
    }    
}

// get strings from non text item throws std::string

void helper12test::strings_3()
{
#pragma pack(push, 1)
    struct {
        NSCLDAQ12::TextItem  s_item;
        char      s_moreStrings[500];
    } item;
#pragma pack(pop)
     // Apologies to Dr. Suess.
    std::vector<std::string> strings = {
        "one string", "two string", "three string", "four",
        "red string", "blue string", "green string", "more"
    };
    
    item.s_item.s_header.s_type = NSCLDAQ12::FIRST_USER_ITEM_CODE;
    item.s_item.s_body.u_noBodyHeader.s_empty = sizeof(uint32_t);
    
    uint32_t bodySize =
        fillTextBody(item.s_item.s_body.u_noBodyHeader.s_body, strings);
    item.s_item.s_header.s_size =
        sizeof(NSCLDAQ12::RingItemHeader) + sizeof(uint32_t)
        + bodySize;
    CPPUNIT_ASSERT_THROW(
        m_pHelper->getStrings(&item, m_pTranslator),
        std::string
    );
    
}