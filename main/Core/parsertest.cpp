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

/** @file:  parsertest.cpp
 *  @brief:  Test CAENParser class.
 */
#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/Asserter.h>
#include "Asserts.h"
#define private public
#include "CAENParser.h"
#undef private
#include "CAENHit.h"
#include "CAENModuleHits.h"
#include "DataFormat.h"

#include <fragment.h>
#include <stdint.h>
#include <string.h>

/**
 *  The struct type below is used to compose 'events'.
 */
struct FakeEvent {
    uint8_t*   m_pBuffer;
    uint8_t*   m_pCursor;
    
    FakeEvent(size_t nBytes);
    ~FakeEvent();
    
    void putFragment(size_t nBytes, uint64_t timestamp, uint32_t sid, void* pData);
    void finish();
    
};
// Construction  - note we need to save space for the size long.

FakeEvent::FakeEvent(size_t nBytes) :
    m_pBuffer(nullptr), m_pCursor(nullptr)
{
    m_pBuffer = new uint8_t[nBytes];
    m_pCursor = m_pBuffer + sizeof(uint32_t);
}
FakeEvent::~FakeEvent()
{
    delete []m_pBuffer;
}
void
FakeEvent::putFragment(
    size_t nBytes, uint64_t timestamp, uint32_t sid, void* pData
)
{
    ufmt::EVB::pFragmentHeader p =
        reinterpret_cast<ufmt::EVB::pFragmentHeader>(m_pCursor);
    p->s_timestamp = timestamp;
    p->s_sourceId  = sid;
    p->s_size      = nBytes + sizeof(RingItemHeader) + sizeof(BodyHeader);
    p->s_barrier   = 0;
    pRingItemHeader pRHeader = reinterpret_cast<pRingItemHeader>(p+1);
    pRHeader->s_size = p->s_size;
    pRHeader->s_type = PHYSICS_EVENT;
    pBodyHeader pBHeader = reinterpret_cast<pBodyHeader>(pRHeader+1);
    pBHeader->s_size = sizeof(BodyHeader);
    pBHeader->s_timestamp = timestamp;
    pBHeader->s_sourceId   = sid;
    pBHeader->s_barrier   = 0;
    
    m_pCursor      = reinterpret_cast<uint8_t*>(pBHeader+1);
    memcpy(m_pCursor, pData, nBytes);
    m_pCursor += nBytes;
    
}
void
FakeEvent::finish()
{
    uint32_t nBytes = (m_pCursor - m_pBuffer);
    uint32_t* p     = reinterpret_cast<uint32_t*>(m_pBuffer);
    *p  = nBytes;
}


class parsertest : public CppUnit::TestFixture {
    
private:
    CAENParser* m_pParser;
public:
    void setUp() {
        m_pParser = new CAENParser;
    }
    void tearDown() {
        delete m_pParser;    
    }
private:
    CPPUNIT_TEST_SUITE(parsertest);
    CPPUNIT_TEST(construct_1);
    
    CPPUNIT_TEST(add_1);
    CPPUNIT_TEST(add_2);
    CPPUNIT_TEST(add_3);
    
    CPPUNIT_TEST(parse_1);
    CPPUNIT_TEST(parse_2);
    CPPUNIT_TEST_SUITE_END();

protected:
    void construct_1();
    
    void add_1();
    void add_2();
    void add_3();
    
    void parse_1();
    void parse_2();
};

CPPUNIT_TEST_SUITE_REGISTRATION(parsertest);

void parsertest::construct_1()
{
    // without something to parse there are no hit modules:
    
    EQ(size_t(0), m_pParser->getHitModules().size());
    EQ(size_t(0), m_pParser->m_modules.size());
}
void parsertest::add_1()
{
    // Add PSD and check structures:
    
    m_pParser->addModule(1, CAENHit::PSD);
    EQ(size_t(1), m_pParser->m_modules.size());
    const CAENParser::ModuleInfo& info = m_pParser->m_modules[1];
    
    EQ(CAENHit::PSD, info.s_hitType);
    EQ(0, info.s_multiplier);
    EQ(1, info.s_module.getSourceId());
    EQ(size_t(0), info.s_module.getHits().size());
    
}
void parsertest::add_2()
{
    // ADD PHA and check structures:
    
    m_pParser->addModule(2, CAENHit::PHA);
    
    EQ(size_t(1), m_pParser->m_modules.size());
    const CAENParser::ModuleInfo& info = m_pParser->m_modules[2];
    
    EQ(CAENHit::PHA, info.s_hitType);
    EQ(0, info.s_multiplier);
    EQ(2, info.s_module.getSourceId());
    EQ(size_t(0), info.s_module.getHits().size());
}
void parsertest::add_3()
{
    // Can add more than one and they're kept straight:
    
    m_pParser->addModule(1, CAENHit::PSD);
    m_pParser->addModule(2, CAENHit::PHA);
    
    EQ(size_t(2), m_pParser->m_modules.size());
    // Using blocks like this allows a re-use of the assertions
    //  from add_1, add_3
    
    {
        const CAENParser::ModuleInfo& info = m_pParser->m_modules[1];
        EQ(CAENHit::PSD, info.s_hitType);
        EQ(0, info.s_multiplier);
        EQ(1, info.s_module.getSourceId());
        EQ(size_t(0), info.s_module.getHits().size());
    }
    {
        const CAENParser::ModuleInfo& info = m_pParser->m_modules[2];
        EQ(CAENHit::PHA, info.s_hitType);
        EQ(0, info.s_multiplier);
        EQ(2, info.s_module.getSourceId());
        EQ(size_t(0), info.s_module.getHits().size());
    }
}
void parsertest::parse_1()
{
    // Parse an event with one fragment:
    
    FakeEvent event(1024);            // an event:
    
    // PHA hit with no waveform:
    
     uint16_t data[0x11] = {
        0x0011, 0x0000,                 // Word count.
        0x001e, 0x0000,                 // Byte count.
        0x1,    0x0000,                 // Channel #
        0x1234, 0x5678, 0x9abc, 0xef,   // timestamp.
        100,                            // energy.
        200,                            // extras 1
        300, 0,                           // extras 2,
        0, 0                            // no waveforms 0 sample count.
    };
    event.putFragment(sizeof(data), 0x12345, 1, data);
    event.finish();
    
    CAENParser parser;
    parser.addModule(1, CAENHit::PHA);
    parser.addModule(2, CAENHit::PSD);
    
    // Parse the hit:
    
    parser(event.m_pBuffer);
    auto& modules = parser.getHitModules();
    EQ(size_t(1), modules.size());
    
    auto& module(*modules[0]);
    EQ(1, module.getSourceId());
    
    auto& hits = module.getHits();
    EQ(size_t(1), hits.size());
    auto& hit  = (*hits[0]);
    EQ(CAENHit::PHA, hit.getType());
    EQ(uint64_t(0xef9abc56781234), hit.getTimeTag());
    EQ(uint32_t(1), hit.getChannel());
    EQ(size_t(0), hit.trace1().size());
    EQ(size_t(0), hit.trace2().size());
    
    const CAENPHAHit& phaHit(dynamic_cast<const CAENPHAHit&>(hit));
    EQ(uint16_t(100), phaHit.getEnergy());
    EQ(uint16_t(200), phaHit.getExtra1());
    EQ(uint16_t(300), phaHit.getExtra2());
}
void parsertest::parse_2()
{
    // parse an event with two fragments, one in each sid.
    
    
    FakeEvent event(1024);            // an event:
    
    // PHA hit with no waveform:
    
     uint16_t data[0x11] = {
        0x0011, 0x0000,                 // Word count.
        0x001e, 0x0000,                 // Byte count.
        0x1,    0x0000,                 // Channel #
        0x1234, 0x5678, 0x9abc, 0xef,   // timestamp.
        100,                            // energy.
        200,                            // extras 1
        300,0,                            // extras 2,
        0, 0                            // no waveforms 0 sample count.
    };
    event.putFragment(sizeof(data), 0x12345, 1, data);

    // psd hit with no waveform.
    
    uint16_t frag2[17] = {
        17,0,                         // Size of body (words)
        30, 0,                        // Size of hits (bytes)
        0x2234, 0x5678, 0x9abc, 0xef, // Timestamp
        1,                           // channel number.
        200,                        // short charge
        400,                        // long charge
        50,                         // baseline
        0,                          // PUR flag.
        0x7654, 0x3210,             // Extras
        sizeof(uint32_t), 0         // no waveform data.
    };
    event.putFragment(sizeof(frag2), 0x12347, 2, frag2);
    event.finish();
    
    // Set up the parser, sid 1 is a PHA module, sid2 PSD as per our fragments.
    
    CAENParser parser;
    parser.addModule(1, CAENHit::PHA);
    parser.addModule(2, CAENHit::PSD);
    parser(event.m_pBuffer);              // Parse the event into modules & hits.
    
    // Sholid be two modules first the PHA, then the PSD (order of items in event):
    
    auto& modules = parser.getHitModules();
    EQ(size_t(2), modules.size());
    
    auto& phamod = *(modules[0]);
    auto& psdmod = *(modules[1]);
    
    // double check the source ids:
    
    EQ(1, phamod.getSourceId());
    EQ(2, psdmod.getSourceId());
    
    // Based on the previous test, we believe in the  parsing of the PHA hit
    // let's test the PSD hit:
    
    // First the type independent stuff:
    
    auto& hits = psdmod.getHits();
    EQ(size_t(1), hits.size());
    auto& hit  = (*hits[0]);
    EQ(CAENHit::PSD, hit.getType());
    EQ(uint64_t(0xef9abc56782234), hit.getTimeTag());
    EQ((uint32_t)1, hit.getChannel());
    EQ(size_t(0), hit.trace1().size());
    EQ(size_t(0), hit.trace2().size());
    
    // Now the stuff we can get because it's a psd hit:
    
    CAENPSDHit& psdhit(dynamic_cast<CAENPSDHit&>(hit));
    EQ(uint16_t(200), psdhit.getShortCharge());
    EQ(uint16_t(400), psdhit.getLongCharge());
    EQ(uint16_t(50),  psdhit.getBaseline());
    EQ(uint16_t(0),   psdhit.getPURFlag());
    
    // Assuming we've got it right so far, there's no need to
    // The CFD time and getTime values have already been verified
    // by other tests.
}