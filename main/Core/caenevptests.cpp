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

/** @file:   caenevptests.cpp
 *  @brief:  Test Suite of CAENEventProcessor class.
 */
#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/Asserter.h>
#include "Asserts.h"
#include "CAENEventProcessor.h"
#include "CAENParameterMap.h"
#include "CAENModuleHits.h"
#include "CAENHit.h"              // Makes it easier to fake hits.
#include <string>
#include <map>
#include <config.h>
#include <Event.h>
#include <TreeParameter.h>
#include <Histogrammer.h>
#include <Globals.h>
#include <stdint.h>
#include <DataFormat.h>
#include <fragment.h>
#include <memory.h>
#include <stdint.h>


class CAnalyzer;
class CBufferDecoder;

/**
 * Here is our test CAENEventProcessor derived class.
 *  - it creates three parameter maps:
 *    * A PHA one.
 *    * A PSD one with multiplier 1 (500MHz module).
 *    * A PSD one with multiplier 2 (250MHz module).
 */
class TestProcessor : public CAENEventProcessor
{
public:
    TestProcessor();
};

/**
 * TestProcessor constructor
 *    Creates the three parameter maps and registers them for sid 1-3
 *    with dynamic maps.
 */
TestProcessor::TestProcessor() 
{
    setTestMode();
    addParameterMap(
        1,
        new CAENPHAArrayMapper("pha_t", "pha_e", "pha_ex1", "pha_ex2")
    );
    addParameterMap(
        2,
        new CAENPSDArrayMapper("psd1_t", "psd1_s", "psd1_l", "psd1_b", "psd1_p"),
        1
    );                      // 500MHz PSD module.
    addParameterMap(
        3,
        new CAENPSDArrayMapper("psd2_t", "psd2_s", "psd2_l", "psd2_b", "psd2_p"),
        2
    );                      // 250MHz PSD module.
}

class caenevptest : public CppUnit::TestFixture {
    CPPUNIT_TEST_SUITE(caenevptest);
    CPPUNIT_TEST(parse_1);
    CPPUNIT_TEST(parse_2);
    CPPUNIT_TEST(parse_3);
    CPPUNIT_TEST(parse_4);
    CPPUNIT_TEST_SUITE_END();
protected:
    void parse_1();
    void parse_2();
    void parse_3();
    void parse_4();
    
private:
    CHistogrammer* m_pHistogrammer;
    CEvent*  m_pEvent;                  // Fake event.

    CAnalyzer*      m_pAnalyzer;
    CBufferDecoder* m_pDecoder;
public:
    void setUp() {
        m_pHistogrammer = new CHistogrammer;
        gpEventSink = m_pHistogrammer;
        m_pEvent = new CEvent;
        m_pEvent->clear();
        
        // The analyzer and buffer decoder are unused:
        
        m_pAnalyzer = nullptr;
        m_pDecoder  = nullptr;
    }
    void tearDown() {
        delete m_pEvent;
        delete m_pHistogrammer;
        gpEventSink = nullptr;
    }

private:
    // Data formatting stuff.
    
    size_t makePhaHit(void* p, int chan, int time, int e, int ex1, int ex2);
    size_t makePsdHit(void* p, int chan, int shortQ, int longQ, int bl, int time, int cfd, int pur);
    size_t makeRingItem(void* p, int sid, uint64_t time, size_t payloadSize, void* pPayload);
    size_t makeFragment(void* pDest, void *pItem);
};

CPPUNIT_TEST_SUITE_REGISTRATION(caenevptest);

/**
 *  makeFragment
 *     format a fragment.
 * @param pDest - where the fragment goes.
 * @param pItem - ringitem in the fragment.
 * @return size_t - size of the fragment.
 */
size_t
caenevptest::makeFragment(void* pDest, void* pItem)
{
    pRingItemHeader pSrc = reinterpret_cast<pRingItemHeader>(pItem);
    pBodyHeader pBh = reinterpret_cast<pBodyHeader>(pSrc+1);
    
    EVB::pFragmentHeader pFrag = reinterpret_cast<EVB::pFragmentHeader>(pDest);
    EVB::pFragmentHeader pBody = pFrag+1;
    
    // Fill in the fragment header; we get the info from the body header
    // and ring item header.
    
    pFrag->s_timestamp = pBh->s_timestamp;
    pFrag->s_sourceId    = pBh->s_sourceId;
    pFrag->s_size      = pSrc->s_size;
    pFrag->s_barrier   = pBh->s_barrier;
    memcpy(pBody, pSrc, pSrc->s_size);

    return pSrc->s_size + sizeof(EVB::FragmentHeader);
    
}
/**
 * makeRingItem
 *   @param p - where to put it.
 *   @param sid - Source id (for the body header).
 *   @param time - timestamp (for the body header).
 *   @param payloadSize - size of the data past the body header.
 *   @param pPayload   - The body of the ring item.
 *   @return size of ring item.
 */
size_t
caenevptest::makeRingItem(
    void* p, int sid, uint64_t time, size_t payloadSize, void* pPayload
)
{
    // Build pointers to the chunks of the ring item:
    
    pRingItemHeader pItemHeader = reinterpret_cast<pRingItemHeader>(p);
    pBodyHeader     pBHeader    = reinterpret_cast<pBodyHeader>(pItemHeader+1);
    uint32_t*       pBody       = reinterpret_cast<uint32_t*>(pBHeader+1);
    
    // Fill in the body header:
    
    pBHeader->s_timestamp = time;
    pBHeader->s_sourceId  = sid;
    pBHeader->s_barrier   = 0;
    pBHeader->s_size      = sizeof(BodyHeader);
    
    // Fill in the data:
    
    *pBody = (payloadSize)/sizeof(uint16_t) + 1;   // Readout does this.
    memcpy(pBody+1 , pPayload, payloadSize);
    
    // Fill in the ring item header:
    
    size_t totalSize = sizeof(uint32_t)+payloadSize + sizeof(BodyHeader) + sizeof(RingItemHeader);
    pItemHeader->s_size = totalSize;
    pItemHeader->s_type = PHYSICS_EVENT;
    
    return totalSize;
}

/**
 * makePsdHit
 *   Create a PSD hit without a waveform.
 * @param  p   - Where to put the hit (must be big enough).
 * @param chan - channel the hit came from
 * @param shortQ - short charge.
 * @param longQ - long charge.
 * @param bl    - Baseline
 * @param time  - coarse time.
 * @param cfd   - cfd value.
 * @param pur   - Pile up rejection flag.
 * @return size_t - total number of bytes of data put into p.
 */
size_t
caenevptest::makePsdHit(
    void* p, int chan, int shortQ, int longQ, int bl, int time, int cfd, int pur
)
{
    uint32_t* pBodySize = reinterpret_cast<uint32_t*>(p);  // Hold for size.
    uint64_t* pTs       = reinterpret_cast<uint64_t*>(pBodySize+1);
    *pTs = time;
    uint16_t* pStuff    = reinterpret_cast<uint16_t*>(pTs+1);
    *pStuff++ = chan;
    *pStuff++ = shortQ;
    *pStuff++ = longQ;
    *pStuff++ = bl;
    *pStuff++ = pur;
    uint32_t* pLongStuff = reinterpret_cast<uint32_t*>(pStuff);
    *pLongStuff++ = cfd;   // bottom 10 bits of extras...
    *pLongStuff++ = sizeof(uint32_t);     // No waveform data.
    
    // Ok so how much data is this:
    
    size_t nBytes =
        reinterpret_cast<uint8_t*>(pLongStuff) -
        reinterpret_cast<uint8_t*>(pBodySize);
 
    *pBodySize = nBytes;
    return nBytes;   
}
/**
 * makePhaHit
 *   Fill in a PHA hit without waveform data.
 * @param p - Where the hit data is put - must be big enough.
 * @param chan - Channel the hit comes from.
 * @param time - timestamp
 * @param e    - energy.
 * @param ex1  - extra 1.
 * @param ex2  - Extra2 word.
 * @return size_t - number of bytes of data.
 */
size_t
caenevptest::makePhaHit(void* p, int chan, int time, int e, int ex1, int ex2)
{
    uint32_t* pBodySize = reinterpret_cast<uint32_t*>(p);  // hold place.
    uint32_t* p32       = reinterpret_cast<uint32_t*>(pBodySize+1);
    *p32++              = chan;
    uint64_t* p64       = reinterpret_cast<uint64_t*>(p32);
    *p64++              = time;
    uint16_t* p16       = reinterpret_cast<uint16_t*>(p64);
    *p16++              = e;
    *p16++              = ex1;
    *p16++              = ex2;
    p32                 = reinterpret_cast<uint32_t*>(p16);
    *p32++              = 0;     // no waveform samples.
    
    // figure out the size of the hit:
    
    size_t size = reinterpret_cast<uint8_t*>(p32) -
        reinterpret_cast<uint8_t*>(pBodySize);
    *pBodySize = size;
    
    return size;
    
}

void caenevptest::parse_1()
{
    // Empty event fills nothing:
    
    TestProcessor p;
    
    // PHA1
    
    CTreeParameterArray pha_t("pha_t", 16, 0);
    CTreeParameterArray pha_e("pha_e", 16, 0);
    CTreeParameterArray pha_ex1("pha_ex1", 16, 0);
    CTreeParameterArray pha_ex2("pha_ex2", 16, 0);
    
    // PSD1
    
    CTreeParameterArray psd1_t("psd1_t", 16, 0);
    CTreeParameterArray psd1_s("psd1_s", 16, 0);
    CTreeParameterArray psd1_l("psd1_l", 16, 0);
    CTreeParameterArray psd1_b("psd1_b", 16, 0);
    CTreeParameterArray psd1_p("psd1_p", 16, 0);
    
    // PSD2
    
    CTreeParameterArray psd2_t("psd2_t", 16, 0);
    CTreeParameterArray psd2_s("psd2_s", 16, 0);
    CTreeParameterArray psd2_l("psd2_l", 16, 0);
    CTreeParameterArray psd2_b("psd2_b", 16, 0);
    CTreeParameterArray psd2_p("psd2_p", 16, 0);
    
    // Bind it all
    
    CTreeParameter::BindParameters();
    CTreeParameter::setEvent(*m_pEvent);
    
    
    // An empty event:
    
    uint32_t event(sizeof(uint32_t));
    p(&event, *m_pEvent, *m_pAnalyzer, *m_pDecoder);
    
    // Nothing gets set:
    
    for (int i = 0; i < 16; i++) {
        ASSERT(!pha_t[i].isValid());
        ASSERT(!pha_e[i].isValid());
        ASSERT(!pha_ex1[i].isValid());
        ASSERT(!pha_ex2[i].isValid());
        
        ASSERT(!psd1_s[i].isValid());
        ASSERT(!psd1_l[i].isValid());
        ASSERT(!psd1_b[i].isValid());
        ASSERT(!psd1_p[i].isValid());
        ASSERT(!psd1_t[i].isValid());
        
        ASSERT(!psd2_t[i].isValid());
        ASSERT(!psd2_s[i].isValid());
        ASSERT(!psd2_l[i].isValid());
        ASSERT(!psd2_b[i].isValid());
        ASSERT(!psd2_p[i].isValid());
        
    }
}
void caenevptest::parse_2()
{
    // Make an event with one pha hit and see that the right tree parameter
    // array values get filled in while the wrong one's don't.
    
      
    
    TestProcessor p;
    
    // PHA1
    
    CTreeParameterArray pha_t("pha_t", 16, 0);
    CTreeParameterArray pha_e("pha_e", 16, 0);
    CTreeParameterArray pha_ex1("pha_ex1", 16, 0);
    CTreeParameterArray pha_ex2("pha_ex2", 16, 0);
    
    // PSD1
    
    CTreeParameterArray psd1_t("psd1_t", 16, 0);
    CTreeParameterArray psd1_s("psd1_s", 16, 0);
    CTreeParameterArray psd1_l("psd1_l", 16, 0);
    CTreeParameterArray psd1_b("psd1_b", 16, 0);
    CTreeParameterArray psd1_p("psd1_p", 16, 0);
    
    // PSD2
    
    CTreeParameterArray psd2_t("psd2_t", 16, 0);
    CTreeParameterArray psd2_s("psd2_s", 16, 0);
    CTreeParameterArray psd2_l("psd2_l", 16, 0);
    CTreeParameterArray psd2_b("psd2_b", 16, 0);
    CTreeParameterArray psd2_p("psd2_p", 16, 0);
    
    // Bind it all
    
    CTreeParameter::BindParameters();
    CTreeParameter::setEvent(*m_pEvent);
    
    uint8_t hit[100];               //PHA hit buffer.
    uint8_t item[200];              //Ringitem buffer.
    uint32_t event[200];            // Event buffer.
    
    size_t hitSize = makePhaHit(hit, 1, 1234, 100, 5, 7);
    size_t itemSize = makeRingItem(item, 1, 1234, hitSize, hit);
    size_t fragSize = makeFragment(&(event[1]), item);
    event[0] = fragSize + sizeof(uint32_t);
    
    p(event, *m_pEvent, *m_pAnalyzer, *m_pDecoder);
    
    // None of the PSD's should have been hit and the PHA's should only
    // be hit for index 1:
    
    for (int i = 0; i < 16; i++) {
        if (i != 1) {
            ASSERT(!pha_t[i].isValid());
            ASSERT(!pha_e[i].isValid());
            ASSERT(!pha_ex1[i].isValid());
            ASSERT(!pha_ex2[i].isValid());
        } else {
            ASSERT(pha_t[i].isValid());
            ASSERT(pha_e[i].isValid());
            ASSERT(pha_ex1[i].isValid());
            ASSERT(pha_ex2[i].isValid());
        }
        ASSERT(!psd1_s[i].isValid());
        ASSERT(!psd1_l[i].isValid());
        ASSERT(!psd1_b[i].isValid());
        ASSERT(!psd1_p[i].isValid());
        ASSERT(!psd1_t[i].isValid());
        
        ASSERT(!psd2_t[i].isValid());
        ASSERT(!psd2_s[i].isValid());
        ASSERT(!psd2_l[i].isValid());
        ASSERT(!psd2_b[i].isValid());
        ASSERT(!psd2_p[i].isValid());
        
    }
    // The right values should be in PHA[1] tree variables:
    
    EQ(double(1234), double(pha_t[1]));
    EQ(double(100), double(pha_e[1]));
    EQ(double(5), double(pha_ex1[1]));
    EQ(double(7), double(pha_ex2[1]));
}
void caenevptest::parse_3()
{
    // Single PSD hit in module 2:   This is a 500MHz module
    
    TestProcessor p;
    
    // Local copies of the tree parameter arrays I care about:
    
    // PHA1
    
    CTreeParameterArray pha_t("pha_t", 16, 0);
    CTreeParameterArray pha_e("pha_e", 16, 0);
    CTreeParameterArray pha_ex1("pha_ex1", 16, 0);
    CTreeParameterArray pha_ex2("pha_ex2", 16, 0);
    
    // PSD1 - a channel here will have data.
    
    CTreeParameterArray psd1_t("psd1_t", 16, 0);
    CTreeParameterArray psd1_s("psd1_s", 16, 0);
    CTreeParameterArray psd1_l("psd1_l", 16, 0);  
    CTreeParameterArray psd1_b("psd1_b", 16, 0);
    CTreeParameterArray psd1_p("psd1_p", 16, 0);
    
    // PSD2
    
    CTreeParameterArray psd2_t("psd2_t", 16, 0);
    CTreeParameterArray psd2_s("psd2_s", 16, 0);
    CTreeParameterArray psd2_l("psd2_l", 16, 0);
    CTreeParameterArray psd2_b("psd2_b", 16, 0);
    CTreeParameterArray psd2_p("psd2_p", 16, 0);
    
    // Bind it all
    
    CTreeParameter::BindParameters();
    CTreeParameter::setEvent(*m_pEvent);
    
    // Make a psd hit in channel 2 of psd1:
    
    uint8_t hit[100];               //PSD hit buffer.
    uint8_t item[200];              //Ringitem buffer.
    uint32_t event[200];            // Event buffer.
    
    size_t hitSize = makePsdHit(hit, 2, 100, 200, 50, 123, 3, 0);
    size_t itemSize = makeRingItem(item, 2, 123, hitSize, hit);
    size_t fragSize = makeFragment(&(event[1]), item);
    event[0] = fragSize + sizeof(uint32_t);
    
    p(event, *m_pEvent, *m_pAnalyzer, *m_pDecoder);
    
    // Check that only the right channels got hit
    
    for (int i = 0; i < 16; i++) {
        ASSERT(!pha_t[i].isValid());
        ASSERT(!pha_e[i].isValid());
        ASSERT(!pha_ex1[i].isValid());
        ASSERT(!pha_ex2[i].isValid());
        
        if (i != 2) {
            ASSERT(!psd1_s[i].isValid());
            ASSERT(!psd1_l[i].isValid());
            ASSERT(!psd1_b[i].isValid());
            ASSERT(!psd1_p[i].isValid());
            ASSERT(!psd1_t[i].isValid());
        } else {
            ASSERT(psd1_s[i].isValid());
            ASSERT(psd1_l[i].isValid());
            ASSERT(psd1_b[i].isValid());
            ASSERT(psd1_p[i].isValid());
            ASSERT(psd1_t[i].isValid());
        }
        
        ASSERT(!psd2_t[i].isValid());
        ASSERT(!psd2_s[i].isValid());
        ASSERT(!psd2_l[i].isValid());
        ASSERT(!psd2_b[i].isValid());
        ASSERT(!psd2_p[i].isValid());
        
    }
    
    // check that the correct values are in the correct channels.
    // note the CFD multiplier for this module is 1.
    
    EQ(double(100), double(psd1_s[2]));
    EQ(double(200), double(psd1_l[2]));
    EQ(double(50), double(psd1_b[2]));
    EQ(double(40), double(psd1_p[2]));
    EQ(double(123.003), double(psd1_t[2]));
              
}
void caenevptest::parse_4()
{
    // Module 3 has a single PSD hit - that's a CFD multiplier of 2
    // which makes it a 250Mhz module.
    
    TestProcessor p;
    
    // Local copies of the tree parameter arrays I care about:
    
    // PHA1
    
    CTreeParameterArray pha_t("pha_t", 16, 0);
    CTreeParameterArray pha_e("pha_e", 16, 0);
    CTreeParameterArray pha_ex1("pha_ex1", 16, 0);
    CTreeParameterArray pha_ex2("pha_ex2", 16, 0);
    
    // PSD1 - a channel here will have data.
    
    CTreeParameterArray psd1_t("psd1_t", 16, 0);
    CTreeParameterArray psd1_s("psd1_s", 16, 0);
    CTreeParameterArray psd1_l("psd1_l", 16, 0);  
    CTreeParameterArray psd1_b("psd1_b", 16, 0);
    CTreeParameterArray psd1_p("psd1_p", 16, 0);
    
    // PSD2
    
    CTreeParameterArray psd2_t("psd2_t", 16, 0);
    CTreeParameterArray psd2_s("psd2_s", 16, 0);
    CTreeParameterArray psd2_l("psd2_l", 16, 0);
    CTreeParameterArray psd2_b("psd2_b", 16, 0);
    CTreeParameterArray psd2_p("psd2_p", 16, 0);
    
    // Bind it all
    
    CTreeParameter::BindParameters();
    CTreeParameter::setEvent(*m_pEvent);
    
    // Make a psd hit in channel 2 of psd1:
    
    uint8_t hit[100];               //PSD hit buffer.
    uint8_t item[200];              //Ringitem buffer.
    uint32_t event[200];            // Event buffer.
    
    size_t hitSize = makePsdHit(hit, 5, 100, 200, 50, 123, 3, 0);
    size_t itemSize = makeRingItem(item, 3, 123, hitSize, hit);
    size_t fragSize = makeFragment(&(event[1]), item);
    event[0] = fragSize + sizeof(uint32_t);
    
    p(event, *m_pEvent, *m_pAnalyzer, *m_pDecoder);
    
    // Check that only the right channels got hit
    
    for (int i = 0; i < 16; i++) {
        ASSERT(!pha_t[i].isValid());
        ASSERT(!pha_e[i].isValid());
        ASSERT(!pha_ex1[i].isValid());
        ASSERT(!pha_ex2[i].isValid());
        
        ASSERT(!psd1_s[i].isValid());
        ASSERT(!psd1_l[i].isValid());
        ASSERT(!psd1_b[i].isValid());
        ASSERT(!psd1_p[i].isValid());
        ASSERT(!psd1_t[i].isValid());
    
        if (i != 5) {    
            ASSERT(!psd2_t[i].isValid());
            ASSERT(!psd2_s[i].isValid());
            ASSERT(!psd2_l[i].isValid());
            ASSERT(!psd2_b[i].isValid());
            ASSERT(!psd2_p[i].isValid());
        } else {
            ASSERT(psd2_t[i].isValid());
            ASSERT(psd2_s[i].isValid());
            ASSERT(psd2_l[i].isValid());
            ASSERT(psd2_b[i].isValid());
            ASSERT(psd2_p[i].isValid());
        }
    }
    // Check the correct values got set.
    
    EQ(double(100), (double)psd2_s[5]);
    EQ(double(200), (double)psd2_l[5]);
    EQ(double(50), (double)psd2_b[5]);
    EQ(double(40), (double)psd2_p[5]);
    EQ(double(123.006), (double)psd2_t[5]);
    
}