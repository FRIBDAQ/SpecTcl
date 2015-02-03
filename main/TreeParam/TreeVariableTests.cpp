// This implementation of TreeParameter is based on the ideas and original code of::
//    Daniel Bazin
//    National Superconducting Cyclotron Lab
//    Michigan State University
//    East Lansing, MI 48824-1321
//
// Note that this also tests assignment to CTreeVariableProperties from
// a CTreeVariable as we need to ensure that enough infrastructure in the
// CTreeVariable world works before we can do that test.

#include <config.h>
#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/Asserter.h>
#include "Asserts.h"
#include "TreeTestSupport.h"
#include "CTreeVariableProperties.h"
#include "CTreeVariable.h"
#include "TCLInterpreter.h"
#include "TCLVariable.h"
#include "ListVisitor.h"

#include <tcl.h>

#include <string>
#include <vector>

#ifdef HAVE_STD_NAMESPACE
using namespace std;
#endif




class TreeVarTests : public CppUnit::TestFixture {
  CPPUNIT_TEST_SUITE(TreeVarTests);
  CPPUNIT_TEST(Construction);
  CPPUNIT_TEST(Binding);
  CPPUNIT_TEST(SimpleAssigns);
  CPPUNIT_TEST(OpAssigns);
  CPPUNIT_TEST(AutoOps);
  CPPUNIT_TEST(ChangeMonitoring);
  CPPUNIT_TEST(AssignFromProperties);
  CPPUNIT_TEST_SUITE_END();


private:

public:
  void setUp() {
    TreeTestSupport::InitTestInterpreter();
  }
  void tearDown() {
    TreeTestSupport::TeardownTestInterpreter();
    TreeTestSupport::ClearVariableMap();
  }
protected:
  void Construction();
  void Binding();
  void SimpleAssigns();
  void OpAssigns();
  void AutoOps();
  void ChangeMonitoring();
  void AssignFromProperties();
};

CPPUNIT_TEST_SUITE_REGISTRATION(TreeVarTests);

void TreeVarTests::Construction() 
{
  // Default construction builds an uninitialized parameter.

  CTreeVariable anonymous;
  EQMSG("anonymous null properties", (CTreeVariableProperties*)NULL,
	TreeTestSupport::getVariableProperties(anonymous));

  // Parameterized constructor builds a parameter bound to a 
  // tree variable properties.

  CTreeVariable a("a", 3.54, "cm");
  CTreeVariableProperties* aProperties = TreeTestSupport::getVariableProperties(a);
  ASSERT(aProperties != (CTreeVariableProperties*)NULL);
  EQMSG("name ok", string("a"), a.getName());
  EQMSG("bound name same", string("a"), aProperties->getName());
  EQMSG("value transferred: ", 3.54, (double)(*aProperties));
  EQMSG("units transferred: ", string("cm"), a.getUnit());

  // duplicate names get bound to the same underlying parameter.

  CTreeVariable aa("a", 3.54, "cm");
  EQMSG("same pointers",  aProperties, 
	TreeTestSupport::getVariableProperties(aa));

  // Should only be one entry in the map:

  EQMSG("Map size", 1, CTreeVariable::size());

  // Two cases for the construct from properties:
  // properties exist implies we'll bind to the existing one
  // which will override evrything but the name:

  CTreeVariableProperties duplicate("a", 1.00, "in");
  CTreeVariable aaa(duplicate);
  EQMSG("from properties duplicate",
	aProperties, TreeTestSupport::getVariableProperties(aaa));
  EQMSG("from properties dup value", 3.54, (double)aaa);
  EQMSG("from properties dup units", string("cm"), aaa.getUnit());

  CTreeVariableProperties different("b", 2.22, "in");
  CTreeVariable b(different);
  ASSERT(TreeTestSupport::getVariableProperties(b) != aProperties);
  EQMSG("Map size after second", 2, CTreeVariable::size());

}
//
//
void
TreeVarTests::Binding()
{
  CTreeVariable a("a", 3.1416, "cm");
  a.Bind();

  // I should be able to read this value from the variable:

  Tcl_Interp* pRaw = TreeTestSupport::getInterpreter()->getInterpreter();
  const char* pValue = Tcl_GetVar(pRaw, "a", TCL_GLOBAL_ONLY);
  ASSERT(pValue != (const char*)NULL);
  EQMSG("bound value: ", string("3.1416"), string(pValue));

}
// Do the simple assigns (from double and from another tree variable.

void
TreeVarTests::SimpleAssigns()
{
  CTreeVariable a("a", 0.0, "cm");
  CTreeVariable b("b", 5.0, "in");
  CTreeVariable::BindVariables(*TreeTestSupport::getInterpreter());

  Tcl_Interp* pRaw = TreeTestSupport::getInterpreter()->getInterpreter();
  const char* value;
  

  // Assign to a from double:

  a     = 3.1416;
  value = Tcl_GetVar(pRaw, "a", TCL_GLOBAL_ONLY);
  EQMSG("assign from double", string("3.1416"), string(value));

  // Assign from another variable:

  a      = b;
  value  = Tcl_GetVar(pRaw, "a", TCL_GLOBAL_ONLY);
  EQMSG("assign from variable", string("5.0"), string(value));
  
}
//
void
TreeVarTests::OpAssigns()
{
  // Do assigns with arithmetic operations as well.

  CTreeVariable base("a", 0.0, "cm"); // Base value.
  Tcl_Interp*   pRaw = TreeTestSupport::getInterpreter()->getInterpreter();
  CTreeVariable::BindVariables(*(TreeTestSupport::getInterpreter()));
  const char*   pValue;

  // += 1.23:

  base += 1.23;
  pValue = Tcl_GetVar(pRaw, "a", TCL_GLOBAL_ONLY);
  EQMSG("+= from tree variable", (double)1.23, (double)base);
  EQMSG("+= from TCL variable", string("1.23"), string(pValue));

  // -= 0.5

  base  -= 0.5;
  pValue = Tcl_GetVar(pRaw, "a", TCL_GLOBAL_ONLY);
  EQMSG("-= from tree variable", (double)(0.73), double(base));
  EQMSG("-= from tcl variable", string("0.73"), string(pValue));

  // *= 2.0

  base  *= 2.0;
  pValue = Tcl_GetVar(pRaw, "a", TCL_GLOBAL_ONLY);
  EQMSG("*= from tree variable", (double)(1.46), double(base));
  EQMSG("*= from tcl variable", string("1.46"), string(pValue));

  // /= 4.0
  base    /= 4.0;
  pValue   = Tcl_GetVar(pRaw, "a", TCL_GLOBAL_ONLY);
  EQMSG("/= from tree variable", (double)(0.365), double(base));
  EQMSG("/= from tcl variable", string("0.365"), string(pValue));
  
}
// 
void
TreeVarTests::AutoOps()
{
  CTreeVariable a("a", 1.5, "cm");
  Tcl_Interp*    pRaw = TreeTestSupport::getInterpreter()->getInterpreter();
  CTreeVariable::BindVariables(*(TreeTestSupport::getInterpreter()));
  const char*    pValue;
  double         result;

  // a++ - increments but returns prior value.

  result = a++;
  EQMSG("prior++", 1.5, result);
  EQMSG("a++ result", 2.5, (double)a);

  // ++a - also increments but returns the value after the increment.

  result = ++a;
  EQMSG("++prior", 3.5, result);
  EQMSG("++prior result", 3.5, (double)a);

  // a-- - decrements returning prior value.

  result = a--;
  EQMSG("prior--", 3.5, result);
  EQMSG("prior-- result", 2.5, (double)a);

  // --a decrements returning after:

  result = --a;
  EQMSG("--prior", 1.5, result);
  EQMSG("--prior result", 1.5, (double)a);
}
//
// The stuff below here is used in the ChangeMonitoring test to deal
// with counting trace firings.

static int traces;

static char*
myTrace(ClientData cd, Tcl_Interp *pRawInterp, const char* basename,
	const char* index, int flags)
{
  traces++;
  return NULL;
}

void 
TreeVarTests::ChangeMonitoring()
{
  CTreeVariable a("a", 1.5, "cm");
  Tcl_Interp*    pRaw = TreeTestSupport::getInterpreter()->getInterpreter();
  CTreeVariable::BindVariables(*(TreeTestSupport::getInterpreter()));
  traces = 0;
  Tcl_TraceVar(pRaw, "a", TCL_TRACE_WRITES, myTrace, (ClientData)NULL);

  // Changes without reset will set the flags but not fire the traces.

  a.Initialize("a", 1.5, "mm");	// Should set change def.
  a = 2.5;			// Should set value changed.

  EQMSG(" haschanged   - ", true, a.hasChanged());
  EQMSG(" valuechanged - ", true, a.valueChanged());
  EQMSG(" pre trace count", 0, traces);

  // Reset will reset the change flags and fire the trace.

  a.resetChanged();
  EQMSG("haschanged    - ", false, a.hasChanged());
  EQMSG("valchanged    - ", false, a.valueChanged());
  EQMSG("trace count", 1, traces);


}

void
TreeVarTests::AssignFromProperties()
{
  CTreeVariable a("a", 1.5, "cm");
  Tcl_Interp*    pRaw = TreeTestSupport::getInterpreter()->getInterpreter();
  CTreeVariable::BindVariables(*(TreeTestSupport::getInterpreter()));
  CTreeVariableProperties b("b", 6.54, "mm");

  b = a;
  EQMSG("value : ", 1.5, (double)b);
}
