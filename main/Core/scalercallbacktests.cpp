#include <config.h>
#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/Asserter.h>
#include "Asserts.h"

#include "ScalerProcessorCallbacks.h"
#include <CBufferProcessor.h>
#include <TCLInterpreter.h>
#include <TCLVariable.h>
#include <buffer.h>
#include <buftypes.h>

#include <string>


#ifdef HAVE_STD_NAMESPACE
using namespace std;
#endif


class ScalerTests : public CppUnit::TestFixture
{
  CPPUNIT_TEST_SUITE(ScalerTests);
  CPPUNIT_TEST(ScalerBuffer);
  CPPUNIT_TEST_SUITE_END();
private:
  Tcl_Interp*                    m_pRawInterpreter;
  CTCLInterpreter*               m_pInterpreter;
  CBufferProcessor*              m_pProcessor;
  CSpecTclScalerBufferCallback*  m_pCallback;

public:
  void setUp()
  {
    // Create the interpreter, initialize it and wrap it in an object:

    m_pRawInterpreter = Tcl_CreateInterp();
    Tcl_Init(m_pRawInterpreter);
    m_pInterpreter = new CTCLInterpreter(m_pRawInterpreter);

    // Create the processor, the call back and register it for scaler buffers.

    m_pProcessor = new CBufferProcessor;
    m_pCallback  = new CSpecTclScalerBufferCallback(m_pProcessor, m_pInterpreter);

    m_pProcessor->addCallback(SCALERBF, *m_pCallback);

  }
  void tearDown()
  {
    delete m_pInterpreter;
    Tcl_DeleteInterp(m_pRawInterpreter);
    delete m_pProcessor;
    delete m_pCallback;


  }
protected:
  void ScalerBuffer();
};


CPPUNIT_TEST_SUITE_REGISTRATION(ScalerTests);


void
ScalerTests::ScalerBuffer()
{
  // Setup the buffer for the callback... if more than one test
  // is developed, it will be worthwhile to package this or much of it into setUp.
  struct {
    bheader header;
    sclbody body;
    INT32   scalers[20];	// One too many actually.
  } Buffer;

  Buffer.header.type       = SCALERBF;
  Buffer.header.nevt       = 20;
  Buffer.header.ssignature = 0x0102;
  Buffer.header.lsignature = 0x01020304;

  Buffer.body.etime = 200;
  Buffer.body.btime = 190;
  for(INT32 i =0; i < 20; i++) {
    Buffer.body.scalers[i] = i;	// Since I added 20 more longs at least.
  }

  (*m_pProcessor)(&Buffer);

  // Now check it out!!

  ASSERT(   m_pProcessor->scalersSeen());
  EQ(20U,   m_pProcessor->scalerCount());
  EQ(190UL, m_pProcessor->lastIntervalStart());
  EQ(200UL, m_pProcessor->lastIntervalEnd());
  for(unsigned int i =0; i < 20; i++) {
    EQ((unsigned long)i, m_pProcessor->lastIncrement(i));
    EQ((float)i,         m_pProcessor->Total(i));
  }
}
