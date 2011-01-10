// Template for a test suite.

#include  <config.h>
#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/Asserter.h>
#include "Asserts.h"
#include <string>


using namespace std;


#include "TCLInterpreter.h"
#include "TCLTracedVariable.h"
#include "VariableTraceCallback.h"


/// Test trace class.

class TestTracer : public CVariableTraceCallback
{
public:				// for test classes that's ok.
  string            m_pName;
  string            m_pEle;
  int              m_flags;
  CTCLInterpreter* m_pInterp;
public:
  TestTracer() :
    m_flags(0),
    m_pInterp(0) {}

  virtual const char* operator()(CTCLInterpreter* pInterp,
			   char* pVariable,
			   char* pElement, 
			   int flags) {
    m_pName    = pVariable;
    m_pEle     = pElement;
    m_flags    = flags;
    m_pInterp  = pInterp;

    return NULL;
  }

};

// The test suite.

class tracevartests : public CppUnit::TestFixture {
  CPPUNIT_TEST_SUITE(tracevartests);
  CPPUNIT_TEST(construction);
  CPPUNIT_TEST(manualcall);
  CPPUNIT_TEST(tcltrace);
  CPPUNIT_TEST_SUITE_END();


private:
  Tcl_Interp*           m_pRawInterpreter;
  CTCLInterpreter*      m_pInterp;
  CTCLTracedVariable*   m_pVariable;
  TestTracer*           m_pTestTrace;

public:
  void setUp() {
    m_pRawInterpreter = Tcl_CreateInterp();
    Tcl_Init(m_pRawInterpreter);
    m_pInterp   = new CTCLInterpreter(m_pRawInterpreter);
    m_pTestTrace= new TestTracer;
    m_pVariable = new CTCLTracedVariable(m_pInterp,
					 "atest(anelement)",
					 *m_pTestTrace,
					 TCL_TRACE_WRITES);

  }
  void tearDown() {
    delete m_pVariable;
    delete m_pTestTrace;
    delete m_pInterp;
    Tcl_DeleteInterp(m_pRawInterpreter);
    
  }
protected:
  void construction();
  void manualcall();
  void tcltrace();
};

CPPUNIT_TEST_SUITE_REGISTRATION(tracevartests);


void 
tracevartests::construction()
{
  EQ((CVariableTraceCallback*)m_pTestTrace,
     &(m_pVariable->getCallback()));
}

void
tracevartests::manualcall()
{
  (*m_pVariable)((char*)"atest", (char*)"anelement",
		 TCL_TRACE_WRITES);

  EQ(m_pInterp, m_pTestTrace->m_pInterp);
  EQ(string("atest"), string(m_pTestTrace->m_pName));
  EQ(string("anelement"), string(m_pTestTrace->m_pEle));
  EQ(TCL_TRACE_WRITES, m_pTestTrace->m_flags);
}

void
tracevartests::tcltrace()
{
  Tcl_SetVar2(m_pRawInterpreter,
	      "atest", "anelement", "george", TCL_GLOBAL_ONLY);

  EQ(m_pInterp, m_pTestTrace->m_pInterp);
  EQ(string("atest"), string(m_pTestTrace->m_pName));
  EQ(string("anelement"), string(m_pTestTrace->m_pEle));
  EQ(TCL_TRACE_WRITES, m_pTestTrace->m_flags & TCL_TRACE_WRITES);  
}
