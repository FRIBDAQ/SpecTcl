/*
    This software is Copyright by the Board of Trustees of Michigan
    State University (c) Copyright 2017.

    You may use this software under the terms of the GNU public license
    (GPL).  The terms of this license are described at:

     http://www.gnu.org/licenses/gpl.txt

     Authors:
             Ron Fox
             Giordano Cerriza
	     NSCL
	     Michigan State University
	     East Lansing, MI 48824-1321
*/

/** @file:  fragidxtst.cpp
 *  @brief: daqdev/SpecTcl#378 - Tests for FragmentIndex class.
 */
#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/Asserter.h>
#include "Asserts.h"
#include "FragmentIndex.h"

#include <stdlib.h>

#include <fragment.h>
#include <DataFormat.h>
#include <string.h>

class fragidxTest : public CppUnit::TestFixture {
    CPPUNIT_TEST_SUITE(fragidxTest);
    CPPUNIT_TEST(empty);
    CPPUNIT_TEST(frag1_1);
    CPPUNIT_TEST(frag1_2);
    CPPUNIT_TEST(frag2_1);
    CPPUNIT_TEST(frag2_2);
    CPPUNIT_TEST_SUITE_END();
    
private:

public:
    void setUp() {
        
    }
    void tearDown() {
        
    }
protected:
    void empty();
    
    void frag1_1();
    void frag1_2();
    
    void frag2_1();
    void frag2_2();
};

CPPUNIT_TEST_SUITE_REGISTRATION(fragidxTest);

void fragidxTest::empty()
{
    uint32_t event(sizeof(uint32_t));  // empty event built data.
    FragmentIndex frags(reinterpret_cast<uint16_t*>(&event));
    EQ(size_t(0), frags.getNumberFragments());
}

void fragidxTest::frag1_1()
{
    // Single fragment, no body header.
    
    uint16_t event[1024];       // Probably too big.

    
    // The Ring item.

    uint8_t frag[100];
    pRingItem pFrag = reinterpret_cast<pRingItem>(frag);
    pFrag->s_header.s_type = PHYSICS_EVENT;
    pFrag->s_header.s_size = sizeof(RingItemHeader) + 2*sizeof(uint32_t);
    pFrag->s_body.u_noBodyHeader.s_mbz = 0;
    uint32_t *pData =
        reinterpret_cast<uint32_t*>(pFrag->s_body.u_noBodyHeader.s_body);
    *pData = 0xaaaaaaaa;         // Nice signature.
    
    
    // The fragment header.
    
    EVB::FragmentHeader fh;
    fh.s_timestamp   = 0x12345678;
    fh.s_sourceId    = 1;
    fh.s_size        = pFrag->s_header.s_size;
    fh.s_barrier     = 0;
    
    // Fill in event:
    
    uint32_t nBytes = sizeof(uint32_t) + sizeof(fh) + pFrag->s_header.s_size;
    uint8_t* p = reinterpret_cast<uint8_t*>(event);
    memcpy(p, &nBytes, sizeof(uint32_t));
    p += sizeof(uint32_t);
    memcpy(p, &fh, sizeof(fh));
    p += sizeof(fh);
    memcpy(p, pFrag, pFrag->s_header.s_size);
    
    FragmentIndex f(event);
    EQ(size_t(1), f.getNumberFragments());
    FragmentInfo info = f.getFragment(0);
    
    EQ(uint64_t(0x12345678), info.s_timestamp);
    EQ(uint32_t(1), info.s_sourceId);
    EQ(fh.s_size, info.s_size);
    EQ(fh.s_barrier, info.s_barrier);
    
    // The header should  point to ring item header:
    
    pRingItemHeader pH = reinterpret_cast<pRingItemHeader>(info.s_itemhdr);
    EQ(uint32_t(PHYSICS_EVENT), pH->s_type);
    EQ(pFrag->s_header.s_size, pH->s_size);
    
    uint32_t* pMbz = reinterpret_cast<uint32_t*>(pH+1);
    EQ(uint32_t(0), *pMbz);
    
    pMbz++;
    uint32_t* body = reinterpret_cast<uint32_t*>(info.s_itembody);
    EQ(pMbz, body);
    EQ(uint32_t(0xaaaaaaaa), *body);
    
    
    
    
}
void fragidxTest::frag1_2()
{
    // One fragment with a 'standard'  body header.
    
    // Single fragment, with body header.
    
    uint16_t event[1024];       // Probably too big.

    
    // The Ring item.

    uint8_t frag[100];
    pRingItem pFrag = reinterpret_cast<pRingItem>(frag);
    pFrag->s_header.s_type = PHYSICS_EVENT;
    pFrag->s_header.s_size = sizeof(RingItemHeader) + sizeof(uint32_t) + sizeof(BodyHeader);
    pFrag->s_body.u_hasBodyHeader.s_bodyHeader.s_size = sizeof(BodyHeader);
    pFrag->s_body.u_hasBodyHeader.s_bodyHeader.s_timestamp = 0x12345678;
    pFrag->s_body.u_hasBodyHeader.s_bodyHeader.s_sourceId    = 1;
    pFrag->s_body.u_hasBodyHeader.s_bodyHeader.s_barrier     = 0;
    uint32_t *pData =
        reinterpret_cast<uint32_t*>(pFrag->s_body.u_hasBodyHeader.s_body);
    *pData = 0xaaaaaaaa;         // Nice signature.
    
    // Fragment header:
    
    // The fragment header.
    
    EVB::FragmentHeader fh;
    fh.s_timestamp   = 0x12345678;
    fh.s_sourceId    = 1;
    fh.s_size        = pFrag->s_header.s_size;
    fh.s_barrier     = 0;
    
    
    // Fill in event:
    
    uint32_t nBytes = sizeof(uint32_t) + sizeof(fh) + pFrag->s_header.s_size;
    uint8_t* p = reinterpret_cast<uint8_t*>(event);
    memcpy(p, &nBytes, sizeof(uint32_t));
    p += sizeof(uint32_t);
    memcpy(p, &fh, sizeof(fh));
    p += sizeof(fh);
    memcpy(p, pFrag, pFrag->s_header.s_size);
    
    FragmentIndex f(event);
    EQ(size_t(1), f.getNumberFragments());
    
    
     FragmentInfo info = f.getFragment(0);
    
    EQ(uint64_t(0x12345678), info.s_timestamp);
    EQ(uint32_t(1), info.s_sourceId);
    EQ(fh.s_size, info.s_size);
    EQ(fh.s_barrier, info.s_barrier);
    
    // The header should  point to ring item header:
    
    pRingItemHeader pH = reinterpret_cast<pRingItemHeader>(info.s_itemhdr);
    EQ(uint32_t(PHYSICS_EVENT), pH->s_type);
    EQ(pFrag->s_header.s_size, pH->s_size);
    
    // The body should point to the signature word:
    
    uint32_t* pBody = reinterpret_cast<uint32_t*>(info.s_itembody);
    EQ(uint32_t(0xaaaaaaaa), *pBody);
    
}
void fragidxTest::frag2_1()
{
    // Two fragments - first has no body header.
    // We can get to the second one ok.
uint16_t event[1024];       // Probably too big.

    
    // The Ring item.

    uint8_t frag[100];
    pRingItem pFrag = reinterpret_cast<pRingItem>(frag);
    pFrag->s_header.s_type = PHYSICS_EVENT;
    pFrag->s_header.s_size = sizeof(RingItemHeader) + 2*sizeof(uint32_t);
    pFrag->s_body.u_noBodyHeader.s_mbz = 0;
    uint32_t *pData =
        reinterpret_cast<uint32_t*>(pFrag->s_body.u_noBodyHeader.s_body);
    *pData = 0xaaaaaaaa;         // Nice signature.
    
    
    // The fragment header.
    
    EVB::FragmentHeader fh;
    fh.s_timestamp   = 0x12345678;
    fh.s_sourceId    = 1;
    fh.s_size        = pFrag->s_header.s_size;
    fh.s_barrier     = 0;
    
    // Fill in event:
    
    uint32_t nBytes = sizeof(uint32_t) + 2*(sizeof(fh) + pFrag->s_header.s_size);
    uint8_t* p = reinterpret_cast<uint8_t*>(event);
    memcpy(p, &nBytes, sizeof(uint32_t));
    p += sizeof(uint32_t);
    memcpy(p, &fh, sizeof(fh));
    p += sizeof(fh);
    memcpy(p, pFrag, pFrag->s_header.s_size);
    p += pFrag->s_header.s_size;
    
    // Second event we just change the timestamp and pattern:
    
    *pData = 0x55555555;
    fh.s_timestamp = 0x87654321;            // Can't happen in real life but...
    
    memcpy(p, &fh, sizeof(fh));
    p += sizeof(fh);
    memcpy(p, pFrag, pFrag->s_header.s_size);
    
    FragmentIndex f(event);
    EQ(size_t(2), f.getNumberFragments());
    auto info = f.getFragment(1);          // Second frag.
    
    EQ(uint64_t(0x87654321), info.s_timestamp);
    uint32_t* pBody = reinterpret_cast<uint32_t*>(info.s_itembody);
    EQ(uint32_t(0x55555555), *pBody);
    
 
}

void fragidxTest::frag2_2()
{
    // Two fragments first has a body header,
    // we can get to the second one ok.
    
     uint16_t event[1024];       // Probably too big.

    
    // The Ring item.

    uint8_t frag[100];
    pRingItem pFrag = reinterpret_cast<pRingItem>(frag);
    pFrag->s_header.s_type = PHYSICS_EVENT;
    pFrag->s_header.s_size = sizeof(RingItemHeader) + sizeof(uint32_t) + sizeof(BodyHeader);
    pFrag->s_body.u_hasBodyHeader.s_bodyHeader.s_size = sizeof(BodyHeader);
    pFrag->s_body.u_hasBodyHeader.s_bodyHeader.s_timestamp = 0x12345678;
    pFrag->s_body.u_hasBodyHeader.s_bodyHeader.s_sourceId    = 1;
    pFrag->s_body.u_hasBodyHeader.s_bodyHeader.s_barrier     = 0;
    uint32_t *pData =
        reinterpret_cast<uint32_t*>(pFrag->s_body.u_hasBodyHeader.s_body);
    *pData = 0xaaaaaaaa;         // Nice signature.
    
    // Fragment header:
    
    // The fragment header.
    
    EVB::FragmentHeader fh;
    fh.s_timestamp   = 0x12345678;
    fh.s_sourceId    = 1;
    fh.s_size        = pFrag->s_header.s_size;
    fh.s_barrier     = 0;
    
    
    // Fill in event:
    
    uint32_t nBytes = sizeof(uint32_t) + sizeof(fh) + pFrag->s_header.s_size;
    uint8_t* p = reinterpret_cast<uint8_t*>(event);
    memcpy(p, &nBytes, sizeof(uint32_t));
    p += sizeof(uint32_t);
    memcpy(p, &fh, sizeof(fh));
    p += sizeof(fh);
    memcpy(p, pFrag, pFrag->s_header.s_size);
    p += pFrag->s_header.s_size;
    
    // Second event change both timestamps and the pattern:
    
    fh.s_timestamp = 0x87654321;
    pFrag->s_body.u_hasBodyHeader.s_bodyHeader.s_timestamp = 0x87654321;
    *pData = 0x55555555;
    
    // Put the event:
    
    memcpy(p, &fh, sizeof(fh));
    p += sizeof(fh);
    memcpy(p, pFrag, pFrag->s_header.s_size);
    p += pFrag->s_header.s_size;
    
    FragmentIndex f(event);
    EQ(size_t(2), f.getNumberFragments());
    
    auto info = f.getFragment(1);       // Second frag.
    EQ(uint64_t(0x8754321), info.s_timestamp);
    uint32_t* pBody = reinterpret_cast<uint32_t*>(info.s_itembody);
    EQ(*pData, *pBody);
      
}