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



class ring11test : public CppUnit::TestFixture {
    CPPUNIT_TEST_SUITE(ring11test);
    CPPUNIT_TEST(hasbodyhdr_1);
    CPPUNIT_TEST(hasbodyhdr_2);
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