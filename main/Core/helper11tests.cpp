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

/** @file:  heper11tests.cpp
 *  @brief: Tests for CRingFormatHelper11 class
 */
#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/Asserter.h>
#include "Asserts.h"
#include "RingFormatHelper11.h"
#include "DataFormat.h"
#include "BufferTranslator.h"
#include <string>
#include <string.h>
#include <time.h>


class ring11test : public CppUnit::TestFixture {
    CPPUNIT_TEST_SUITE(ring11test);
    CPPUNIT_TEST(hasbodyhdr_1);
    CPPUNIT_TEST(hasbodyhdr_2);
    
    CPPUNIT_TEST(bodyptr_1);
    CPPUNIT_TEST(bodyptr_2);
    
    CPPUNIT_TEST(bodyhdr_1);
    CPPUNIT_TEST(bodyhdr_2);
    
    CPPUNIT_TEST(title_1);
    CPPUNIT_TEST(title_2);
    CPPUNIT_TEST(title_3);
    
    CPPUNIT_TEST(run_1);
    CPPUNIT_TEST(run_2);
    CPPUNIT_TEST(run_3);
    
    CPPUNIT_TEST(strcount_1);
    CPPUNIT_TEST(strcount_2);
    CPPUNIT_TEST(strcount_3);
    
    CPPUNIT_TEST(strings_1);
    CPPUNIT_TEST(strings_2);
    CPPUNIT_TEST(strings_3);
    CPPUNIT_TEST_SUITE_END();
    
private:
    CRingFormatHelper* m_pHelper;
    BufferTranslator * m_pTranslator;
public:
    void setUp() {
        m_pHelper = new CRingFormatHelper11;
        m_pTranslator = new NonSwappingBufferTranslator;
    }
    void tearDown() {
        delete m_pHelper;
        delete m_pTranslator;
    }
protected:
    void hasbodyhdr_1();
    void hasbodyhdr_2();
    
    void bodyptr_1();
    void bodyptr_2();
    
    void bodyhdr_1();
    void bodyhdr_2();
    
    void title_1();
    void title_2();
    void title_3();
    
    void run_1();
    void run_2();
    void run_3();
    
    void strcount_1();
    void strcount_2();
    void strcount_3();
    
    void strings_1();
    void strings_2();
    void strings_3();
private:
    void fillStateChangeItem(pStateChangeItem pItem, unsigned run, const char* title);
};
// Note the caller must ensure the body has enough size for the strings.
// Returns the body size.
ptrdiff_t fillTextItemBody(pTextItemBody pBody, const std::vector<std::string>& strings)
{
    pBody->s_timeOffset = 10;
    pBody->s_timestamp  = time(nullptr);
    pBody->s_stringCount = strings.size();
    pBody->s_offsetDivisor = 1;
    char* p = pBody->s_strings;
    for (int i =0; i < strings.size(); i++) {
        strcpy(p, strings[i].c_str());
        p += strlen(p) + 1;
    }
    uint8_t* pEnd = reinterpret_cast<uint8_t*>(p);
    uint8_t* pBeg = reinterpret_cast<uint8_t*>(pBody);
    return pEnd - pBeg;                  // Body size in bytes.
}

static void fillBodyHeader(RingItem* pItem)
{
    pBodyHeader p = &(pItem->s_body.u_hasBodyHeader.s_bodyHeader);
    p->s_size = sizeof(BodyHeader);
    p->s_timestamp = 0x1234567890;
    p->s_sourceId  = 1;
    p->s_barrier   = 0;
}

CPPUNIT_TEST_SUITE_REGISTRATION(ring11test);

void ring11test::fillStateChangeItem(
    pStateChangeItem pItem, unsigned run, const char* title
)
{
    pStateChangeItemBody pBody =
        reinterpret_cast<pStateChangeItemBody>(m_pHelper->getBodyPointer(pItem));
        pBody->s_runNumber = run;
        pBody->s_timeOffset = 0;
        pBody->s_Timestamp = time(nullptr);
        pBody->s_offsetDivisor = 1;
        strncpy(pBody->s_title, title, TITLE_MAXSIZE+1);
}

// Ring item without a body header.

void ring11test::hasbodyhdr_1()
{
    RingItem item;
    item.s_header.s_size = sizeof(RingItemHeader) + sizeof(uint32_t);
    item.s_header.s_type = PHYSICS_EVENT;
    item.s_body.u_noBodyHeader.s_mbz = 0;
    ASSERT(!m_pHelper->hasBodyHeader(&item));
}
// Ring item with body header.
void ring11test::hasbodyhdr_2()
{
    RingItem item;
    item.s_header.s_size = sizeof(RingItemHeader) + sizeof(BodyHeader);
    item.s_header.s_type = PHYSICS_EVENT;
    fillBodyHeader(&item);
    
    ASSERT(m_pHelper->hasBodyHeader(&item));
}
// Body pointer for non body header item:

void ring11test::bodyptr_1()
{
    RingItem item;
    item.s_header.s_size = sizeof(RingItemHeader) + sizeof(uint32_t);
    item.s_header.s_type = PHYSICS_EVENT;
    item.s_body.u_noBodyHeader.s_mbz = 0;
    
    EQ((void*)item.s_body.u_noBodyHeader.s_body, m_pHelper->getBodyPointer(&item));
}
void ring11test::bodyptr_2()
{
    RingItem item;
    item.s_header.s_size = sizeof(RingItemHeader) + sizeof(BodyHeader);
    item.s_header.s_type = PHYSICS_EVENT;
    fillBodyHeader(&item);
    
    EQ((void*)item.s_body.u_hasBodyHeader.s_body, m_pHelper->getBodyPointer(&item));
}
// body header pointer for non body header items>

void ring11test::bodyhdr_1()
{
    RingItem item;
    item.s_header.s_size = sizeof(RingItemHeader) + sizeof(uint32_t);
    item.s_header.s_type = PHYSICS_EVENT;
    item.s_body.u_noBodyHeader.s_mbz = 0;
    ASSERT(m_pHelper->getBodyHeaderPointer(&item) == nullptr);
}
// body header pointer for body header item:

void ring11test::bodyhdr_2()
{
    RingItem item;
    item.s_header.s_size = sizeof(RingItemHeader) + sizeof(BodyHeader);
    item.s_header.s_type = PHYSICS_EVENT;
    fillBodyHeader(&item);
    
    EQ((void*)(&item.s_body.u_hasBodyHeader.s_bodyHeader),
       m_pHelper->getBodyHeaderPointer(&item));
}
// Get the title when there's no body header:

void ring11test::title_1()
{
    StateChangeItem item;
    item.s_header.s_size =
        sizeof(RingItemHeader) + sizeof(StateChangeItemBody) + sizeof(uint32_t);
    item.s_header.s_type   = BEGIN_RUN;
    item.s_body.u_noBodyHeader.s_mbz = 0;
    fillStateChangeItem(&item, 2, "This is my title");
    
    EQ(std::string("This is my title"), m_pHelper->getTitle(&item));
}
// Get the title when there is a body header:

void ring11test::title_2()
{
    StateChangeItem item;
    item.s_header.s_size =
        sizeof(RingItemHeader) + sizeof(StateChangeItemBody) + sizeof(BodyHeader);
    item.s_header.s_type   = BEGIN_RUN;
    fillBodyHeader(reinterpret_cast<pRingItem>(&item));
    fillStateChangeItem(&item, 2, "This is my title");
    EQ(std::string("This is my title"), m_pHelper->getTitle(&item));
}
// non state change item getting title throws std::string.
void ring11test::title_3()
{
    StateChangeItem item;
    item.s_header.s_size =
        sizeof(RingItemHeader) + sizeof(StateChangeItemBody) + sizeof(BodyHeader);
    item.s_header.s_type   = FIRST_USER_ITEM_CODE;
    fillBodyHeader(reinterpret_cast<pRingItem>(&item));
    fillStateChangeItem(&item, 2, "This is my title");
    CPPUNIT_ASSERT_THROW(
        m_pHelper->getTitle(&item),
        std::string
    );
}

// Get the run when there's no body header:

void ring11test::run_1()
{
    StateChangeItem item;
    item.s_header.s_size =
        sizeof(RingItemHeader) + sizeof(StateChangeItemBody) + sizeof(uint32_t);
    item.s_header.s_type   = BEGIN_RUN;
    item.s_body.u_noBodyHeader.s_mbz = 0;
    fillStateChangeItem(&item, 2, "This is my title");
    
    EQ(unsigned(2), m_pHelper->getRunNumber(&item, m_pTranslator));
}
// Get the run when there is a body header:

void ring11test::run_2()
{
    StateChangeItem item;
    item.s_header.s_size =
        sizeof(RingItemHeader) + sizeof(StateChangeItemBody) + sizeof(BodyHeader);
    item.s_header.s_type   = BEGIN_RUN;
    fillBodyHeader(reinterpret_cast<pRingItem>(&item));
    fillStateChangeItem(&item, 2, "This is my title");
    EQ(unsigned(2), m_pHelper->getRunNumber(&item, m_pTranslator));
}
// non state change item getting title throws std::string.
void ring11test::run_3()
{
    StateChangeItem item;
    item.s_header.s_size =
        sizeof(RingItemHeader) + sizeof(StateChangeItemBody) + sizeof(BodyHeader);
    item.s_header.s_type   = FIRST_USER_ITEM_CODE;
    fillBodyHeader(reinterpret_cast<pRingItem>(&item));
    fillStateChangeItem(&item, 2, "This is my title");
    CPPUNIT_ASSERT_THROW(
        m_pHelper->getRunNumber(&item, m_pTranslator),
        std::string
    );
}
// Get string count from non body header item.
void ring11test::strcount_1()
{
#pragma pack(push, 1)
    struct {
        TextItem  s_item;
        char      s_moreStrings[500];
    } item;
#pragma pack(pop)
    // Apologies to Dr. Suess.
    std::vector<std::string> strings = {
        "one string", "two string", "three string", "four",
        "red string", "blue string", "green string", "more"
    };


    item.s_item.s_body.u_noBodyHeader.s_mbz = 0;
    item.s_item.s_header.s_type = MONITORED_VARIABLES;
    pTextItemBody pBody = &(item.s_item.s_body.u_noBodyHeader.s_body);
    auto bodySize = fillTextItemBody(pBody, strings);
    item.s_item.s_header.s_size = sizeof(RingItemHeader) + sizeof(uint32_t) + bodySize;
    
    EQ(strings.size(), size_t(m_pHelper->getStringCount(&item, m_pTranslator)));
    
}
// Get string count from  body header item.

void ring11test::strcount_2()
{
#pragma pack(push, 1)
    struct {
        TextItem  s_item;
        char      s_moreStrings[500];
    } item;
#pragma pack(pop)
    // Apologies to Dr. Suess.
    std::vector<std::string> strings = {
        "one string", "two string", "three string", "four",
        "red string", "blue string", "green string", "more"
    };
    item.s_item.s_header.s_type = MONITORED_VARIABLES;
    fillBodyHeader(reinterpret_cast<pRingItem>(&item));
    pTextItemBody pBody= &(item.s_item.s_body.u_hasBodyHeader.s_body);
    
    auto bodySize = fillTextItemBody(pBody, strings);
    item.s_item.s_header.s_size =
        sizeof(RingItemHeader) + sizeof(BodyHeader) + bodySize;
    
    EQ(strings.size(), size_t(m_pHelper->getStringCount(&item, m_pTranslator)));
}

// Get string count from non text item throws std::string.

void ring11test::strcount_3()
{
#pragma pack(push, 1)
    struct {
        TextItem  s_item;
        char      s_moreStrings[500];
    } item;
#pragma pack(pop)
    // Apologies to Dr. Suess.
    std::vector<std::string> strings = {
        "one string", "two string", "three string", "four",
        "red string", "blue string", "green string", "more"
    };


    item.s_item.s_body.u_noBodyHeader.s_mbz = 0;
    item.s_item.s_header.s_type = FIRST_USER_ITEM_CODE;
    pTextItemBody pBody = &(item.s_item.s_body.u_noBodyHeader.s_body);
    auto bodySize = fillTextItemBody(pBody, strings);
    item.s_item.s_header.s_size = sizeof(RingItemHeader) + sizeof(uint32_t) + bodySize;
    
    CPPUNIT_ASSERT_THROW(
        m_pHelper->getStringCount(&item, m_pTranslator),
        std::string
    );
}
// Get strings from no body header item.

void ring11test::strings_1()
{
#pragma pack(push, 1)
    struct {
        TextItem  s_item;
        char      s_moreStrings[500];
    } item;
#pragma pack(pop)
    // Apologies to Dr. Suess.
    std::vector<std::string> strings = {
        "one string", "two string", "three string", "four",
        "red string", "blue string", "green string", "more"
    };


    item.s_item.s_body.u_noBodyHeader.s_mbz = 0;
    item.s_item.s_header.s_type = MONITORED_VARIABLES;
    pTextItemBody pBody = &(item.s_item.s_body.u_noBodyHeader.s_body);
    auto bodySize = fillTextItemBody(pBody, strings);
    item.s_item.s_header.s_size = sizeof(RingItemHeader) + sizeof(uint32_t) + bodySize;
    
    std::vector<std::string> gotten = m_pHelper->getStrings(&item, m_pTranslator);
    EQ(strings.size(), gotten.size());
    for (int i =0; i < strings.size(); i++) {
        EQ(strings[i], gotten[i]);
    }
}

// Get strings from item with body header.

void ring11test::strings_2()
{
#pragma pack(push, 1)
    struct {
        TextItem  s_item;
        char      s_moreStrings[500];
    } item;
#pragma pack(pop)
    // Apologies to Dr. Suess.
    std::vector<std::string> strings = {
        "one string", "two string", "three string", "four",
        "red string", "blue string", "green string", "more"
    };
    item.s_item.s_header.s_type = MONITORED_VARIABLES;
    fillBodyHeader(reinterpret_cast<pRingItem>(&item));
    pTextItemBody pBody= &(item.s_item.s_body.u_hasBodyHeader.s_body);
    
    auto bodySize = fillTextItemBody(pBody, strings);
    item.s_item.s_header.s_size =
        sizeof(RingItemHeader) + sizeof(BodyHeader) + bodySize;

    

    std::vector<std::string> gotten = m_pHelper->getStrings(&item, m_pTranslator);
    EQ(strings.size(), gotten.size());
    for (int i =0; i < strings.size(); i++) {
        EQ(strings[i], gotten[i]);
    }
}

// Get strings from non string item throws std::string exception.

void ring11test::strings_3()
{
#pragma pack(push, 1)
    struct {
        TextItem  s_item;
        char      s_moreStrings[500];
    } item;
#pragma pack(pop)
    // Apologies to Dr. Suess.
    std::vector<std::string> strings = {
        "one string", "two string", "three string", "four",
        "red string", "blue string", "green string", "more"
    };


    item.s_item.s_body.u_noBodyHeader.s_mbz = 0;
    item.s_item.s_header.s_type = FIRST_USER_ITEM_CODE;
    pTextItemBody pBody = &(item.s_item.s_body.u_noBodyHeader.s_body);
    auto bodySize = fillTextItemBody(pBody, strings);
    item.s_item.s_header.s_size = sizeof(RingItemHeader) + sizeof(uint32_t) + bodySize;
    
    CPPUNIT_ASSERT_THROW(
        m_pHelper->getStrings(&item, m_pTranslator),
        std::string
    );
}
