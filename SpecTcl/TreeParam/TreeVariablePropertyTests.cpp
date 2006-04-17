// This implementation of TreeParameter is based on the ideas and original code of::
//    Daniel Bazin
//    National Superconducting Cyclotron Lab
//    Michigan State University
//    East Lansing, MI 48824-1321
//
// Template for a test suite.
#include <config.h>
#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/Asserter.h>
#include "Asserts.h"
#include "TreeTestSupport.h"
#include "CTreeVariableProperties.h"
#include "TCLInterpreter.h"
#include "TCLVariable.h"
#include "ListVisitor.h"

#include <string>
#include <vector>

#ifdef HAVE_STD_NAMESPACE
using namespace std;
#endif



class TreeVarPropTests : public CppUnit::TestFixture {
  CPPUNIT_TEST_SUITE(TreeVarPropTests);
  CPPUNIT_TEST(Construction);
  CPPUNIT_TEST(Cloning);
  CPPUNIT_TEST(ModUnits);
  CPPUNIT_TEST(DefReset);
  CPPUNIT_TEST(Assigns);
  CPPUNIT_TEST(Linkage);
  CPPUNIT_TEST(Traces);
  CPPUNIT_TEST_SUITE_END();


private:
  CTCLInterpreter* m_pInterp;	// Only used twice but this ensures deletion even
                                // if tests fail
public:
  void setUp() {
    TreeTestSupport::InitTestInterpreter();
    m_pInterp  = TreeTestSupport::getInterpreter();
  }
  void tearDown() {
    TreeTestSupport::TeardownTestInterpreter();
  }
protected:
  void Construction();
  void Cloning();
  void ModUnits();
  void DefReset();		// Reset def changed flag.
  void Assigns();
  void Linkage();		// Test link to TCL variable.
  void Traces();		// Test fireTrace.
};

CPPUNIT_TEST_SUITE_REGISTRATION(TreeVarPropTests);

// Test normal parameterized construction

void 
TreeVarPropTests::Construction() 
{
  CTreeVariableProperties props("george", 3.1416, "radians");

  EQMSG("construction name", string("george"), props.getName());
  EQMSG("construction value", (double)3.1416, (double)props);
  EQMSG("construction units", string("radians"), props.getUnits());
  EQMSG("construction defch", false, props.definitionChanged());
  EQMSG("construction valch", false, props.valueChanged());
}
// test construction by example

void
TreeVarPropTests::Cloning()
{
  CTreeVariableProperties props("george", 3.1416, "radians");
  CTreeVariableProperties harry("harry", props);

  EQMSG("cloned name", string("harry"), harry.getName());
  EQMSG("cloned value", (double)3.1416, (double)harry);
  EQMSG("cloned units", string("radians"), harry.getUnits());
  EQMSG("cloned defch", false, harry.definitionChanged());
  EQMSG("cloned valch", false, harry.valueChanged());
}

// Test units modification (should also set the defchanged flag).

void
TreeVarPropTests::ModUnits()
{
  CTreeVariableProperties props("george", 3.1416, "radians");

  props.setUnits("cm");
  EQMSG("new units", string("cm"), props.getUnits());
  EQMSG("new units changed", true, props.definitionChanged());
}

void
TreeVarPropTests::DefReset()
{
  CTreeVariableProperties props("george", 3.1416, "radians");

  props.setUnits("cm");		// force changed..
  props.resetDefinitionChanged();
  EQMSG("resetdef", false, props.definitionChanged());
}
//  Note that the assignment from a CTreeVariable is tested in 
//  TreeVariableTests
//  to ensure that we have enough working tree variable infrastructure to do the
//  test
//
void 
TreeVarPropTests::Assigns()
{
  CTreeVariableProperties a("a", 1.234, "cm");
  CTreeVariableProperties b("b", 4.231, "mm");

  // First assign to a from double:

  a = 55.0;
  EQMSG("a from double", (double)55.0, (double)a);

  // Next assign to a from b:

  a = b;
  EQMSG("a from b", (double)b, (double)a);
}
//
void
TreeVarPropTests::Linkage()
{
  CTreeVariableProperties a("a", 1.234, "cm");
  a.Link(*m_pInterp);

  CTCLVariable va(m_pInterp, "a", false);
  const char* value = va.Get();
  ASSERT(value != (const char*)NULL);
  EQMSG("Just after link", string("1.234"), string(value));

  // Change via TCL and see if the value changes:

  va.Set("6.66");
  EQMSG("Link after set: ", (double)6.66, (double)a);

  // Change via variable and see if TCL changes:

  va.Set("4.32");
  value = va.Get();
  ASSERT(value != (const char*)NULL);
  EQMSG("just after tcl change", string("4.32"), string(value));

}

// The stuff below is related to testing traces..

static int tracecount;		// counts trace firing.

static char*
myTrace(ClientData cd, Tcl_Interp* pRawInterp, 
	const char* basename, const char* index, int flags)
{
  tracecount++;
  return NULL;
}

void
TreeVarPropTests::Traces()
{
  tracecount = 0;

  CTreeVariableProperties a("a", 1.234, "cm");
  a.Link(*m_pInterp);

  // Establish a trace on the variable a:

  Tcl_Interp* pRaw = m_pInterp->getInterpreter();
  Tcl_TraceVar(pRaw, "a", TCL_TRACE_WRITES , myTrace, (ClientData)NULL);

  a = 0.456;			// should not fire the trace..yet.
  EQMSG("no trace fired", 0, tracecount);

  a.fireTraces();
  EQMSG("trace fired manually", 1, tracecount);

  Tcl_SetVar(pRaw, "a", "532.0", TCL_GLOBAL_ONLY); // should fire trace.
  EQMSG("trace fired by tcl", 2, tracecount);

}
