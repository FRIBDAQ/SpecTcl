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
#include <string.h>

#include <string>

#ifdef HAVE_STD_NAMESPACE
using namespace std;
#endif


// List of buffer types on which to register the callback:

static const int callbacktypes[] = {
  BEGRUNBF, 
  ENDRUNBF, 
  PAUSEBF, 
  RESUMEBF
};
static const int numcallbacks = sizeof(callbacktypes)/sizeof(int);

class ControlTests : public CppUnit::TestFixture {
  CPPUNIT_TEST_SUITE(ControlTests);
  CPPUNIT_TEST(Begin);
  CPPUNIT_TEST(End);
  CPPUNIT_TEST(Pause);
  CPPUNIT_TEST(Resume);
  CPPUNIT_TEST_SUITE_END();


private:
  CBufferProcessor*              m_pProcessor;
  CSpecTclControlBufferCallback* m_pCallback;
  CTCLInterpreter*               m_pInterpreter;
  Tcl_Interp*                    m_pRawInterp;
  
  unsigned short                 m_Buffer[4096]; // Full data buffer.


public:
  void setUp() {
    m_pProcessor   = new CBufferProcessor;
    m_pRawInterp   = Tcl_CreateInterp();
    Tcl_Init(m_pRawInterp);
    m_pInterpreter = new CTCLInterpreter(m_pRawInterp);
    m_pCallback    = new CSpecTclControlBufferCallback(m_pProcessor, m_pInterpreter); 


    for(int i =0; i < numcallbacks; i++) {
      m_pProcessor->addCallback(callbacktypes[i], *m_pCallback);
    }
    InitBuffer();

    // Dynamic loading can cause problems so:

    m_pInterpreter->GlobalEval("set auto_path \"\"");
  }
  void tearDown() {
    delete m_pProcessor;
    delete m_pCallback;
    delete m_pInterpreter;
    Tcl_DeleteInterp(m_pRawInterp);
  }
protected:
  void Begin();
  void End();
  void Pause();
  void Resume();
private:
  void InitBuffer();
};

CPPUNIT_TEST_SUITE_REGISTRATION(ControlTests);

/// Do a generic initialization of the data buffer.  Parts of this will be 
//  supplemented/overridden by the individual tests.
//

struct Buffer {
  bheader  s_Header;
  ctlbody  s_Body;
};

void
ControlTests::InitBuffer()
{
  Buffer* pBuf = (Buffer*)m_Buffer;

  pBuf->s_Header.run        = 1234;
  pBuf->s_Header.ssignature = 0x0102;
  pBuf->s_Header.lsignature = 0x01020304;
  
  strcpy(pBuf->s_Body.title, "This is a test");

}


void ControlTests::Begin() {
  sleep(1);
  Buffer* pBuf = (Buffer*)m_Buffer;
  pBuf->s_Header.type = BEGRUNBF;
  pBuf->s_Body.sortim = 0;	// Runs always start at zero.
  pBuf->s_Body.tod.month = 2;	// Feb I think.
  pBuf->s_Body.tod.day   = 17;
  pBuf->s_Body.tod.year  = 2005;
  pBuf->s_Body.tod.hours = 8;
  pBuf->s_Body.tod.min   = 15;
  pBuf->s_Body.tod.sec   = 45;
  pBuf->s_Body.tod.tenths = 0;

  (*m_pProcessor)(m_Buffer);

  // We need to check that BeginRun gets called at integration test time!!

  CTCLVariable elapsed(m_pInterpreter, "ElapsedRunTime", kfFALSE);
  EQ(string("0"), string(elapsed.Get()));

  CTCLVariable run(m_pInterpreter, "RunNumber", kfFALSE);
  EQ(string("1234"), string(run.Get()));

  CTCLVariable rstate(m_pInterpreter, "ScalerRunState", kfFALSE);
  EQ(string("Active"), string(rstate.Get()));

  CTCLVariable title(m_pInterpreter, "RunTitle", kfFALSE);
  EQ(string("This is a test"), string(title.Get()));

  CTCLVariable stime(m_pInterpreter, "RunStartTime", kfFALSE);
  EQ(string("February 17, 2005, 08:15:45"), string(stime.Get()));

}

void ControlTests::End()
{
  Buffer* pBuf = (Buffer*)m_Buffer;
  pBuf->s_Header.type = ENDRUNBF;
  pBuf->s_Body.sortim = 3600;	// Runs always start at zero.
  pBuf->s_Body.tod.month = 2;	// Feb I think.
  pBuf->s_Body.tod.day   = 17;
  pBuf->s_Body.tod.year  = 2005;
  pBuf->s_Body.tod.hours = 9;
  pBuf->s_Body.tod.min   = 15;
  pBuf->s_Body.tod.sec   = 45;
  pBuf->s_Body.tod.tenths = 0;

  (*m_pProcessor)(m_Buffer);

  CTCLVariable etime(m_pInterpreter, "RunEndTime", kfFALSE);
  CTCLVariable rstate(m_pInterpreter, "ScalerRunState", kfFALSE);
  CTCLVariable elapsed(m_pInterpreter, "ElapsedRunTime", kfFALSE);


  EQ(string("Halted"), string(rstate.Get()));
  EQ(string("February 17, 2005, 09:15:45"), string(etime.Get()));
  EQ(string("3600"), string(elapsed.Get()));
}

void ControlTests::Pause()
{
  Buffer* pBuf = (Buffer*)m_Buffer;
  pBuf->s_Header.type = PAUSEBF;
  pBuf->s_Body.sortim = 60;	// Runs always start at zero.
  pBuf->s_Body.tod.month = 2;	// Feb I think.
  pBuf->s_Body.tod.day   = 17;
  pBuf->s_Body.tod.year  = 2005;
  pBuf->s_Body.tod.hours = 8;
  pBuf->s_Body.tod.min   = 16;
  pBuf->s_Body.tod.sec   = 45;
  pBuf->s_Body.tod.tenths = 0;



  (*m_pProcessor)(m_Buffer);

  CTCLVariable rstate(m_pInterpreter, "ScalerRunState", kfFALSE);
  CTCLVariable elapsed(m_pInterpreter, "ElapsedRunTime", kfFALSE);

  EQ(string("Paused"), string(rstate.Get()));
  EQ(string("60"), string(elapsed.Get()));

}
void ControlTests::Resume()
{
  Buffer* pBuf = (Buffer*)m_Buffer;
  pBuf->s_Header.type = RESUMEBF;
  pBuf->s_Body.sortim = 60;	// Runs always start at zero.
  pBuf->s_Body.tod.month = 2;	// Feb I think.
  pBuf->s_Body.tod.day   = 17;
  pBuf->s_Body.tod.year  = 2005;
  pBuf->s_Body.tod.hours = 8;
  pBuf->s_Body.tod.min   = 17;
  pBuf->s_Body.tod.sec   = 10;
  pBuf->s_Body.tod.tenths = 0;

  (*m_pProcessor)(m_Buffer);

  CTCLVariable rstate(m_pInterpreter, "ScalerRunState", kfFALSE);
  CTCLVariable elapsed(m_pInterpreter, "ElapsedRunTime", kfFALSE);

  EQ(string("Active"), string(rstate.Get()));
  EQ(string("60"),   string(elapsed.Get()));
}
