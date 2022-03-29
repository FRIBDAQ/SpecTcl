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

class helper10test : public CppUnit::TestFixture {
    CPPUNIT_TEST_SUITE(helper10test);
    CPPUNIT_TEST(bodyhdr);
    CPPUNIT_TEST(bodyptr);
    CPPUNIT_TEST(exbodyhdr);
    
    CPPUNIT_TEST(title);
    CPPUNIT_TEST(run);
    CPPUNIT_TEST(badstate_1);
    CPPUNIT_TEST(badstate_2);
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
};

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