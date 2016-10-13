#include <config.h>
#include <buffer.h>
#include <buftypes.h>


#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/Asserter.h>
#include "Asserts.h"
#include "CStandardScalerCallback.h"
#include "SRunContext.h"
#include <string>


#ifdef HAVE_STD_NAMESPACE
using namespace std;
#endif

class scalertests : public CppUnit::TestFixture
{
  CPPUNIT_TEST_SUITE(scalertests);
  CPPUNIT_TEST(singlebuffer);
  CPPUNIT_TEST(multibuffer);
  CPPUNIT_TEST_SUITE_END();

private:
  SRunContext              m_Context;
  CStandardScalerCallback* m_pCallback;
  unsigned short m_Buffer[4096];
public:
  void setUp() {
    m_pCallback = new CStandardScalerCallback(m_Context);
    initContext();
    initBuffer();
  }
  void tearDown() {
    delete m_pCallback;
  }

protected:

  void singlebuffer();
  void multibuffer();
private:
  void initContext();
  void initBuffer();

};

CPPUNIT_TEST_SUITE_REGISTRATION(scalertests);



// Some utilities:


struct Buffer {
  bheader Header __attribute__((aligned(1)));
  sclbody Body __attribute__((aligned(1)));
};


void 
scalertests::initContext()
{
  m_Context.s_fScalersThisRun = false;
  m_Context.s_nScalerCount    = -1;
  m_Context.s_IntervalStartTime  = 0xffffffff;
  m_Context.s_IntervalEndTime    = 0xffffffff;

  if(!m_Context.s_Totals.empty()) {
    m_Context.s_Totals.erase(m_Context.s_Totals.begin(),
			     m_Context.s_Totals.end());
  }
  if(!m_Context.s_Increments.empty()) {
    m_Context.s_Increments.erase(m_Context.s_Increments.begin(),
				 m_Context.s_Increments.end());
  }
}

void scalertests::initBuffer()
{
  Buffer* pBuffer = (Buffer*)m_Buffer;
  
  // The header we set up will be minimal:

  pBuffer->Header.type       = SCALERBF;
  pBuffer->Header.ssignature = 0x0102;
  pBuffer->Header.lsignature = 0x01020304;
}


// The tests:

void
scalertests::singlebuffer()
{
  // We'll put 10 scalers in the buffer with a counting pattern:

  Buffer* pBuffer = (Buffer*)m_Buffer;

  pBuffer->Header.nevt = 10;
  for(int i = 0; i < 10; i++) {
    pBuffer->Body.scalers[i] = i;
  }
  // The start time will be 100, the end time 200:

  pBuffer->Body.etime = 200;
  pBuffer->Body.btime = 100;

  // Call the callback and check:

  (*m_pCallback)(SCALERBF, pBuffer);

  //

  ASSERT(m_Context.s_fScalersThisRun);
  EQ(10, m_Context.s_nScalerCount);
  EQ((size_t)10, m_Context.s_Increments.size());

  for(unsigned long  i =0; i < 10; i++) {
    EQ(i, m_Context.s_Increments[i]);
  }
  EQ((size_t)10, m_Context.s_Totals.size());
  for(unsigned long i = 0; i < 10; i++) {
    EQ((float)(m_Context.s_Increments[i]), 
       m_Context.s_Totals[i]);
  }
  EQ((unsigned long)100, m_Context.s_IntervalStartTime);
  EQ((unsigned long)200, m_Context.s_IntervalEndTime);
}

void
scalertests::multibuffer()
{
  // We'll put 10 scalers in the buffer with a counting pattern:

  Buffer* pBuffer = (Buffer*)m_Buffer;

  pBuffer->Header.nevt = 10;
  for(int i = 0; i < 10; i++) {
    pBuffer->Body.scalers[i] = i;
  }

  pBuffer->Body.etime = 200;
  pBuffer->Body.btime = 100;

  // Call the callback and check:

  (*m_pCallback)(SCALERBF, pBuffer);
  
  pBuffer->Body.etime = 300;
  pBuffer->Body.btime = 200;
  (*m_pCallback)(SCALERBF, pBuffer); // Second call.

  for(unsigned long i = 0; i < 10; i++) {
    EQ((float)(2*m_Context.s_Increments[i]),  m_Context.s_Totals[i]);
  }

}
