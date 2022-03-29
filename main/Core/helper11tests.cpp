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
private:
    void fillStateChangeItem(pStateChangeItem pItem, unsigned run, const char* title);
};


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