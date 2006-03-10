// Template for a test suite.
#include <config.h>

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/Asserter.h>
#include "Asserts.h"
#include "TCLInterpreter.h"
#include "TCLString.h"
#include <fstream>
#include <errno.h>
#include <cstdio>
#include <cstdlib>

#include <tcl.h>

#ifdef HAVE_STD_NAMESPACE
using namespace std;
#endif

// Test the CTCLInterpreter object.



class interpretertest : public CppUnit::TestFixture {
  CPPUNIT_TEST_SUITE(interpretertest);
  CPPUNIT_TEST(getInterpreter);
  CPPUNIT_TEST(stringEval);
  CPPUNIT_TEST(fileEval);
  CPPUNIT_TEST(expressions);
  CPPUNIT_TEST(tildeSubst);
  CPPUNIT_TEST_SUITE_END();


private:
  Tcl_Interp*        m_pRawInterp;
  CTCLInterpreter*  m_pInterp;

public:
  void setUp() {
    m_pRawInterp = Tcl_CreateInterp();
    m_pInterp    = new CTCLInterpreter(m_pRawInterp);
  }
  void tearDown() {
    delete m_pInterp;

  }
protected:
  void getInterpreter();
  void stringEval();
  void fileEval();
  void expressions();
  void tildeSubst();
};

CPPUNIT_TEST_SUITE_REGISTRATION(interpretertest);

// Test proper storage of interpreter.

void interpretertest::getInterpreter() {
  EQ(m_pRawInterp, m_pInterp->getInterpreter());
}

// Test the various string based evals:

void interpretertest::stringEval() 
{
  EQMSG("const char*", string("4"), m_pInterp->Eval("set a 4"));
  EQMSG("std::string", string("5"), m_pInterp->Eval(string("incr a")));
  CTCLString script("set a 16");
  EQMSG("CTCLString", string("16"), m_pInterp->Eval(script));
}
// Test the various fil evals.
//   Make a test file called ./interptest.tcl
//   put the script:
//      set a 5\n
//  in it and eval in various ways:
void interpretertest::fileEval()
{
  {
    ofstream script("./interptest.tcl");
    script << "set a 5\n";
  }
  // Since we're executing the same script over and over again,
  // we must reset a after each file eval:

  EQMSG("const char*", string("5"), m_pInterp->EvalFile("./interptest.tcl"));
  m_pInterp->Eval("unset a");

  EQMSG("string", string("5"), m_pInterp->EvalFile(string("./interptest.tcl")));
  m_pInterp->Eval("unset a");

  CTCLString script("./interptest.tcl");
  EQMSG("CTCLString", string("5"), m_pInterp->EvalFile(script));
  m_pInterp->Eval("unset a");



  remove("./interptest.tcl");
  
}
// Test various expression evals:

void interpretertest::expressions()
{
  // String results:

  string result = m_pInterp->ExprString("3+4");
  EQMSG("string(const char*)", string("7"), result);

  result = m_pInterp->ExprString(string("5-3*2"));
  EQMSG("string(string)", string("-1"), result);

  CTCLString expression("int(pow(2,3))");
  result = m_pInterp->ExprString(expression);
  EQMSG("string(CTCLString)", string("8"), result);

  // Long results:

  EQMSG("long(const char*)", (long)7, m_pInterp->ExprLong("3+4"));
  EQMSG("long(string)",      (long)-1, m_pInterp->ExprLong(string("5-3*2")));
  expression = "int(pow(2,3))";
  EQMSG("long(CTCLString",  (long)8,  m_pInterp->ExprLong(expression));

  // Double results.

  EQMSG("double(const char*)", (double)4.5, m_pInterp->ExprDouble("1.5*3"));
  EQMSG("double(string)",      (double)4.5, m_pInterp->ExprDouble("5-0.5"));
  expression = "pow(2,3)";
  EQMSG("double(CTCLString)",  (double)8.0, m_pInterp->ExprDouble(expression));

  // Bool results.

  EQMSG("bool(const char*)", kfTRUE, m_pInterp->ExprBoolean("on"));
  EQMSG("bool(string)",     kfFALSE, m_pInterp->ExprBoolean(string("7 > 15")));
  expression = "off";
  EQMSG("bool(CTCLString)", kfFALSE, m_pInterp->ExprBoolean(expression));

}

// Test tilde substitution.

void interpretertest::tildeSubst()
{
  const  char* rawName = "~/testing/~/Subst";
  string expandedName  = getenv("HOME");
  expandedName += "/testing/~/Subst";

  EQMSG("const char*", expandedName, m_pInterp->TildeSubst(rawName));
  EQMSG("string",      expandedName, m_pInterp->TildeSubst(string(rawName)));
  CTCLString file(rawName);
  EQMSG("CTCLString",  expandedName, m_pInterp->TildeSubst(file));
	
}

