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

/** @file:  PSDHitTest.cpp
 *  @brief: Test PSD hit unpacking.
 */
#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/Asserter.h>
#include "Asserts.h"
#include <CAENHit.h>

class PsdHitTest : public CppUnit::TestFixture {
    CPPUNIT_TEST_SUITE(PsdHitTest);
    CPPUNIT_TEST(nowf_1);
    CPPUNIT_TEST(nowf_2);
    
    CPPUNIT_TEST(wf_1);
    CPPUNIT_TEST(wf_2);
    CPPUNIT_TEST_SUITE_END();
    
private:
    CAENPSDHit* m_pHit;
public:
    void setUp() {
        m_pHit = new CAENPSDHit(1);
    }
    void tearDown() {
        delete m_pHit;
    }
protected:
    void nowf_1();
    void nowf_2();
    
    void wf_1();
    void wf_2();
};

CPPUNIT_TEST_SUITE_REGISTRATION(PsdHitTest);

void PsdHitTest::nowf_1()
{
    //no waveform.

    uint32_t buffer[1024];      // Hit goes here.
    uint32_t* p32 = buffer;
    uint32_t* pWordCount = p32++;  // Save a pointer
    uint32_t* pByteCount = p32++;  //
    *p32++  = 0x12345678;      // Low bits of timestamp
    *p32++  = 0x87654321;      // high bits of timestamp.
    uint16_t* p16 = reinterpret_cast<uint16_t*>(p32);
    *p16++  = 6;               // Channel number.
    *p16++  = 123;             // Short charge.
    *p16++  = 435;             // long charge.
    *p16++  = 10;              // baseline.
    *p16++  = 0;               // pur flag.
     p32 = reinterpret_cast<uint32_t*>(p16);
    *p32++  = 0;               // Extras word.
    *p32++  = sizeof(uint32_t); // no waveform.
    
    // Fill in sizes:
    
    uint16_t* start16 = reinterpret_cast<uint16_t*>(pWordCount);
    *pWordCount       = reinterpret_cast<uint16_t*>(p32) - start16;
    uint8_t* start8   = reinterpret_cast<uint8_t*>(pByteCount);
    *pByteCount       = reinterpret_cast<uint8_t*>(p32) - start8;
    
    // unpack the hit.
    
    m_pHit->unpack(buffer);
    
    // Check the results:
    
    EQ(CAENHit::PSD, m_pHit->getType());
    EQ(uint64_t(0x8765432112345678), m_pHit->getTimeTag());
    EQ(uint32_t(6), m_pHit->getChannel());
    
    EQ(uint16_t(123), m_pHit->getShortCharge());
    EQ(uint16_t(435), m_pHit->getLongCharge());
    EQ(uint16_t(10),  m_pHit->getBaseline());
    EQ(uint16_t(0),  m_pHit->getPURFlag());
    EQ(uint16_t(0),  m_pHit->getCFDTime());
    EQ(double(0x8765432112345678), m_pHit->getTime());
    EQ(size_t(0), m_pHit->trace1().size());
    EQ(size_t(0), m_pHit->trace2().size());
   
    
}
void PsdHitTest::nowf_2()
{
    // nonzero cfd - get the fine time right:
    
    uint32_t buffer[1024];      // Hit goes here.
    uint32_t* p32 = buffer;
    uint32_t* pWordCount = p32++;  // Save a pointer
    uint32_t* pByteCount = p32++;  //
    *p32++  = 0x12345678;      // Low bits of timestamp
    *p32++  = 0x87654321;      // high bits of timestamp.
    uint16_t* p16 = reinterpret_cast<uint16_t*>(p32);
    *p16++  = 6;               // Channel number.
    *p16++  = 123;             // Short charge.
    *p16++  = 435;             // long charge.
    *p16++  = 10;              // baseline.
    *p16++  = 0;               // pur flag.
     p32 = reinterpret_cast<uint32_t*>(p16);
    *p32++  = 123;               // Extras word.
    *p32++  = sizeof(uint32_t); // no waveform.
    
    // Fill in sizes:
    
    uint16_t* start16 = reinterpret_cast<uint16_t*>(pWordCount);
    *pWordCount       = reinterpret_cast<uint16_t*>(p32) - start16;
    uint8_t* start8   = reinterpret_cast<uint8_t*>(pByteCount);
    *pByteCount       = reinterpret_cast<uint8_t*>(p32) - start8;
    m_pHit->unpack(buffer);
    
    // Figure out what to expect:
    
    double coarse = 0x8765432112345678;
    double fine   = 123;
    fine          = fine/1024.0;
    double time   = coarse + (fine/1000.0);
    
    EQ(time, m_pHit->getTime());
}

void PsdHitTest::wf_1()
{
    // Single wave form.
    
    uint32_t buffer[1024];      // Hit goes here.
    uint32_t* p32 = buffer;
    uint32_t* pWordCount = p32++;  // Save a pointer
    uint32_t* pByteCount = p32++;  //
    *p32++  = 0x12345678;      // Low bits of timestamp
    *p32++  = 0x87654321;      // high bits of timestamp.
    uint16_t* p16 = reinterpret_cast<uint16_t*>(p32);
    *p16++  = 6;               // Channel number.
    *p16++  = 123;             // Short charge.
    *p16++  = 435;             // long charge.
    *p16++  = 10;              // baseline.
    *p16++  = 0;               // pur flag.
     p32 = reinterpret_cast<uint32_t*>(p16);
    *p32++  = 123;               // Extras word.
    
    uint32_t* pWfLen = p32++;     // Fill this in later.
    *p32++           = 10;        // # samples
    uint8_t* p8      = reinterpret_cast<uint8_t*>(p32);
    *p8++   = 0;                  // single trace.
    *p8++   = 0;                  // selector is input trace.
    p16     = reinterpret_cast<uint16_t*>(p8);
    // trace samples:
    for (int i =0; i < 10; i++) {
        *p16++ = i;
    }
    p32 = reinterpret_cast<uint32_t*>(p16);  //uint32_t* pointer off end:
    
    // Sizes:
    
    uint16_t* start16 = reinterpret_cast<uint16_t*>(pWordCount);
    *pWordCount       = reinterpret_cast<uint16_t*>(p32) - start16;
    uint8_t* start8   = reinterpret_cast<uint8_t*>(pByteCount);
    *pByteCount       = reinterpret_cast<uint8_t*>(p32) - start8;
    m_pHit->unpack(buffer);
    uint8_t*   bwf    = reinterpret_cast<uint8_t*>(pWfLen);
    *pWfLen           = reinterpret_cast<uint8_t*>(p32) - bwf;
    
    
    // Got it.
    
    m_pHit->unpack(buffer);
    
    auto& t = m_pHit->trace1();
    EQ(size_t(10), t.size());
    EQ(size_t(0), m_pHit->trace2().size());
    
    for (uint16_t i =0; i < 10; i++) {
        EQ(i, t[i]);
    }
    
}

void PsdHitTest::wf_2()
{
    // Single wave form.
    
    uint32_t buffer[1024];      // Hit goes here.
    uint32_t* p32 = buffer;
    uint32_t* pWordCount = p32++;  // Save a pointer
    uint32_t* pByteCount = p32++;  //
    *p32++  = 0x12345678;      // Low bits of timestamp
    *p32++  = 0x87654321;      // high bits of timestamp.
    uint16_t* p16 = reinterpret_cast<uint16_t*>(p32);
    *p16++  = 6;               // Channel number.
    *p16++  = 123;             // Short charge.
    *p16++  = 435;             // long charge.
    *p16++  = 10;              // baseline.
    *p16++  = 0;               // pur flag.
     p32 = reinterpret_cast<uint32_t*>(p16);
    *p32++  = 123;               // Extras word.
    
    uint32_t* pWfLen = p32++;     // Fill this in later.
    *p32++           = 10;        // # samples
    uint8_t* p8      = reinterpret_cast<uint8_t*>(p32);
    *p8++   = 1;                  // dual trace.
    *p8++   = 0;                  // selector is input trace.
    p16     = reinterpret_cast<uint16_t*>(p8);
    // trace samples:
    for (int i =0; i < 10; i++) {
        *p16++ = i;
    }
    for (int i = 10; i < 20; i++) {   // Second trace.
        *p16++ = i;
    }
    p32 = reinterpret_cast<uint32_t*>(p16);  //uint32_t* pointer off end:
    
    // Sizes:
    
    uint16_t* start16 = reinterpret_cast<uint16_t*>(pWordCount);
    *pWordCount       = reinterpret_cast<uint16_t*>(p32) - start16;
    uint8_t* start8   = reinterpret_cast<uint8_t*>(pByteCount);
    *pByteCount       = reinterpret_cast<uint8_t*>(p32) - start8;
    m_pHit->unpack(buffer);
    uint8_t*   bwf    = reinterpret_cast<uint8_t*>(pWfLen);
    *pWfLen           = reinterpret_cast<uint8_t*>(p32) - bwf;
    
    
    // Got it.
    
    m_pHit->unpack(buffer);
    
    auto& t = m_pHit->trace1();
    auto& t2 = m_pHit->trace2();
    EQ(size_t(10), t.size());
    EQ(size_t(10), t2.size());
    
    for (uint16_t i =0; i < 10; i++) {
        EQ(i, t[i]);
    }
    for (uint16_t i =0; i < 10; i++) {
        EQ(uint16_t(i+10), t2[i]);
    }
    
}

