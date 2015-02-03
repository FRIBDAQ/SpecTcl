// Template for a test suite.
// This implementation of TreeParameter is based on the ideas and original code of::
//    Daniel Bazin
//    National Superconducting Cyclotron Lab
//    Michigan State University
//    East Lansing, MI 48824-1321
//

#include <config.h>
#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/Asserter.h>
#include "Asserts.h"
#include "TreeTestSupport.h"
#include "CTreeVariableProperties.h"
#include "CTreeVariable.h"
#include "CTreeVariableArray.h"
#include "TCLInterpreter.h"
#include "TCLVariable.h"
#include "ListVisitor.h"

#include <tcl.h>

#include <string>
#include <vector>

#ifdef HAVE_STD_NAMESPACE
using namespace std;
#endif




class TreeVarArrayTests : public CppUnit::TestFixture {
  CPPUNIT_TEST_SUITE(TreeVarArrayTests);
  CPPUNIT_TEST(Construction);
  CPPUNIT_TEST(Assignment);
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
  void Assignment();
};

CPPUNIT_TEST_SUITE_REGISTRATION(TreeVarArrayTests);

void TreeVarArrayTests::Construction() 
{
  CTCLInterpreter* pInterp = TreeTestSupport::getInterpreter();
  Tcl_Interp*      pRaw    = pInterp->getInterpreter();

  CTreeVariableArray nothing;	// Default construction.
  EQMSG("nothing.size", 0U, nothing.size());
  EQMSG("nothing.first", 0, nothing.firstIndex());

  CTreeVariableArray something("something", 1.234, "cm", 10, -5);
  CTreeVariable::BindVariables(*pInterp);
  EQMSG("something.size", 10U, something.size());
  EQMSG("something.first", -5, something.firstIndex());


  // Now I want to ensure that the somethings all exist and have the value 1.234
  // and units cm e.g.

  for(int i = 0; i < 10; i++) {
    int index = i - 5;
    string name = MakeElementName("something", -5, 10, index);
    const char* pValue = Tcl_GetVar(pRaw, name.c_str(), TCL_GLOBAL_ONLY);
    if(!pValue) {
      string message = name + " gives null pValue";
      FAIL(message);
    }
    EQMSG(name.c_str(), string("1.234"), string(pValue));
    EQMSG("Units", string("cm"), something[index].getUnit());
  }
  // Copy construction should produce the same sort of thing:

  CTreeVariableArray b(something);
  for(int i=-5; i < 5; i++) {
    string name = MakeElementName("b", -5, 10, i);
    EQMSG(name, 1.234, (double)b[i]);
  }
}
//
void TreeVarArrayTests::Assignment()

{
  CTCLInterpreter* pInterp = TreeTestSupport::getInterpreter();
  Tcl_Interp*      pRaw    = pInterp->getInterpreter();
  CTreeVariableArray something("something", 1.234, "cm", 10, -5);
  CTreeVariableArray another("another", 2.543, "in", 15, 0); // will get killed.

  CTreeVariable::BindVariables(*pInterp);

  another = something;   	// Should now be alias for something.
  EQMSG("First index", something.firstIndex(), another.firstIndex());
  EQMSG("Size",        something.size(), another.size());
  for (int i = -5; i < 5; i++) {
    string name = MakeElementName("something", -5, 10, i);
    EQMSG("element name", name, another[i].getName());
  }
}
