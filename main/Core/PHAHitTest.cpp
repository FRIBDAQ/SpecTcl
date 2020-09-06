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

/** @file:  PHAHitTest.cpp
 *  @brief: Test the CAENPHAHit class's unpacking and selectors.
 */
#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/Asserter.h>
#include "Asserts.h"
#include "CAENHit.h"

class CAENPHAHitTest : public CppUnit::TestFixture {
    CPPUNIT_TEST_SUITE(CAENPHAHitTest);
    CPPUNIT_TEST(nowf);
    CPPUNIT_TEST(singlewf);
    CPPUNIT_TEST(doublewf);
    CPPUNIT_TEST_SUITE_END();
    
private:
    CAENPHAHit* m_pHit;
public:
    void setUp() {
        m_pHit = new CAENPHAHit;
    }
    void tearDown() {
        delete m_pHit;
    }
protected:
    void nowf();
    void singlewf();
    void doublewf();
};

CPPUNIT_TEST_SUITE_REGISTRATION(CAENPHAHitTest);
//
// Note all these tests are dependent on me being able to create
// accurate synthetic data!!!
//

void CAENPHAHitTest::nowf()
{
    // Data for the hit without a waveform:
    
    uint16_t data[0x11] = {
        0x0011, 0x0000,                 // Word count.
        0x001e, 0x0000,                 // Byte count.
        0x1,    0x0000,                 // Channel #
        0x1234, 0x5678, 0x9abc, 0xef,   // timestamp.
        100,                            // energy.
        200,                            // extras 1
        300,                            // extras 2,
        0, 0                            // no waveforms 0 sample count.
    };
    m_pHit->unpack(data);
    EQ(CAENHit::PHA, m_pHit->getType());
    EQ(uint64_t(0xef9abc56781234), m_pHit->getTimeTag());
    EQ(uint32_t(1), m_pHit->getChannel());
    EQ(size_t(0), m_pHit->trace1().size());
    EQ(size_t(0), m_pHit->trace2().size());
    EQ(uint16_t(100), m_pHit->getEnergy());
    EQ(uint16_t(200), m_pHit->getExtra1());
    EQ(uint16_t(300), m_pHit->getExtra2());
    
}
void CAENPHAHitTest::singlewf()
{
    // Data with a single wave form.
  uint16_t data[] =  {
       26, 0x0000,                 // Word count.
        0x0048, 0x0000,                 // Byte count.
        0x1,    0x0000,                 // Channel #
        0x1234, 0x5678, 0x9abc, 0xef,   // timestamp.
        100,                            // energy.
        200,                            // extras 1
        300,                            // extras 2,
        10, 0,                            // no waveforms10 samples
        0,                             // one trace.
        0,1,2,3,4,5,6,7,8,9            // the trace
  };
  m_pHit->unpack(data);
  
  // 10 samples in the first trace, none in the second.
  
  EQ(size_t(10), m_pHit->trace1().size());
  EQ(size_t(0),  m_pHit->trace2().size());
  const std::vector<uint16_t>& t(m_pHit->trace1());
  for (int i = 0; i < 10; i++) {
    EQ(uint16_t(i), t[i]);
  }
}
void CAENPHAHitTest::doublewf()
{
    // Hit with two wave forms.

    uint16_t data[] = {    
        36, 0x0000,                 // Word count.
        0x005c, 0x0000,                 // Byte count.
        0x1,    0x0000,                 // Channel #
        0x1234, 0x5678, 0x9abc, 0xef,   // timestamp.
        100,                            // energy.
        200,                            // extras 1
        300,                            // extras 2,
        10, 0,                            // no waveforms10 samples
        1,                             // dual trace.
        0,1,2,3,4,5,6,7,8,9,            // the trace
        9,8,7,6,5,4,3,2,1,0
    };
    m_pHit->unpack(data);
    EQ(size_t(10), m_pHit->trace1().size());
    EQ(size_t(10), m_pHit->trace2().size());
    const std::vector<uint16_t>& t(m_pHit->trace1());
    for (int i = 0; i < 10; i++) {
      EQ(uint16_t(i), t[i]);
    }
    const std::vector<uint16_t>& t2(m_pHit->trace2());
    for (int i = 9; i >= 0; i--) {
        EQ(uint16_t(i), t2[9-i]);
    }
}