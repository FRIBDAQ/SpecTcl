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
};

CPPUNIT_TEST_SUITE_REGISTRATION(helper12test);

static void fillBodyHeader(NSCLDAQ12::BodyHeader& hdr)
{
    hdr.s_size = sizeof(NSCLDAQ12::BodyHeader);
    hdr.s_timestamp = 0x1234567890;
    hdr.s_sourceId  = 2;
    hdr.s_barrier   = 0;
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
