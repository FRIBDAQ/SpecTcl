// Template for a test suite.

#include <config.h>
#include <buffer.h>
#include <buftypes.h>
#include <string.h>

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/Asserter.h>
#include "Asserts.h"

#include "CStandardControlCallback.h"
#include "SRunContext.h"
#include <string>

#ifdef HAVE_STD_NAMESPACE
using namespace std;
#endif



class controltests : public CppUnit::TestFixture {
  CPPUNIT_TEST_SUITE(controltests);
  CPPUNIT_TEST(neutral);
  CPPUNIT_TEST(begin);
  CPPUNIT_TEST(end);
  CPPUNIT_TEST_SUITE_END();


private:
  CStandardControlCallback* m_pCallback;
  SRunContext context;
  unsigned short m_Buffer[4096];	// test buffer.

public:
  void setUp() {
    m_pCallback = new CStandardControlCallback(context);
    initContext();
    initBuffer();
  }
  void tearDown() {
    delete m_pCallback;
  }
protected:
  void initContext();
  void initBuffer();

  void neutral();
  void begin();
  void end();
};

CPPUNIT_TEST_SUITE_REGISTRATION(controltests);

  struct Buffer {
    bheader Header;
    ctlbody Body;
  };

/// Utilities

void controltests::initContext()
{
  context.s_fScalersThisRun = false;
  context.s_Title = "";
  context.s_RunStarted ="";
  context.s_RunEnded = "";
  context.s_nRunNumber = -1;

}
void controltests::initBuffer()
{
  Buffer* buffer = (Buffer*)m_Buffer;
  
  // We only care about signatures, in the header,
  // the title, the sortime, and the tod in the body.

  buffer->Header.run        = 1234;
  buffer->Header.ssignature = 0x0102;
  buffer->Header.lsignature = 0x01020304;

  // Title will be "this is a test"

  strcpy(buffer->Body.title, "this is a test");
  buffer->Body.sortim = 0;

  // The time will be February 11, 2005 17:01:02.

  buffer->Body.tod.month = 2;
  buffer->Body.tod.day   = 11;
  buffer->Body.tod.year  = 2005;
  buffer->Body.tod.hours  = 17;
  buffer->Body.tod.min   = 01;
  buffer->Body.tod.sec   = 02;

  buffer->Body.tod.tenths = 0;   // unused in unix daq.


}



/// The tests.

/*
   Check some neutral buffer.
*/

void controltests::neutral()
{
  Buffer* pBuffer = (Buffer*)m_Buffer;
  pBuffer->Header.type   = PAUSEBF; // This is a pause buffer.
  pBuffer->Body.sortim  = 1234;
  (*m_pCallback)(PAUSEBF, pBuffer);

  // Process the results of the callback on the context block:

  ASSERT(!context.s_fScalersThisRun);
  EQ(string("this is a test"), context.s_Title);
  EQ(1234, context.s_nRunNumber);
  EQ(string(""), context.s_RunStarted);
  EQ(string(""), context.s_RunEnded);
  
}

void controltests::begin()
{
  Buffer* pBuffer = (Buffer*)m_Buffer;

  context.s_fScalersThisRun = true; // Callback should reset it.
  pBuffer->Header.type = BEGRUNBF;

  (*m_pCallback)(BEGRUNBF, pBuffer);

  ASSERT(!context.s_fScalersThisRun);
  EQ(string("February 11, 2005, 17:01:02"), context.s_RunStarted);

}
void controltests::end()
{
  Buffer* pBuffer = (Buffer*)m_Buffer;

  pBuffer->Header.type = ENDRUNBF;
  (*m_pCallback)(ENDRUNBF, pBuffer);

  EQ(string("February 11, 2005, 17:01:02"), context.s_RunEnded);
}
