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
#include "CTreeVariableCommand.h"
#include "CTreeVariable.h"
#include "CTreeVariableArray.h"
#include "TreeTestSupport.h"
#include "TCLInterpreter.h"
#include "TCLResult.h"
#include "TCLList.h"


#include <string>
#include <vector>

#include <stdio.h>

#ifdef HAVE_STD_NAMESPACE
using namespace std;
#endif




class TreeVarCommandTest : public CppUnit::TestFixture {
  CPPUNIT_TEST_SUITE(TreeVarCommandTest);
  CPPUNIT_TEST(ListFunc);
  CPPUNIT_TEST(ListDispat);
  CPPUNIT_TEST(SetChanged);
  CPPUNIT_TEST(SetProperties);
  CPPUNIT_TEST(SetNoUnits);	// Set omitting units.
  CPPUNIT_TEST(CheckChanged);
  CPPUNIT_TEST(Usage);
  CPPUNIT_TEST(Traces);
  CPPUNIT_TEST_SUITE_END();


private:
  CTreeVariableCommandActual*   m_pCommand;
  CTreeVariable*          m_pIndividual;
  CTreeVariableArray*     m_pArray;

  CTCLInterpreter*        m_pInterp;
  
public:
  void setUp() {
    TreeTestSupport::InitTestInterpreter();
    
    
    m_pInterp    = TreeTestSupport::getInterpreter();
    m_pCommand   = new CTreeVariableCommandActual(m_pInterp);
    m_pIndividual= new CTreeVariable("indiv", 1.234, "cm");
    m_pArray     = new CTreeVariableArray("multi", 5.432,  "in", 10, 10);
  }
  void tearDown() {
    
    delete m_pArray;
    delete m_pIndividual;
    TreeTestSupport::ClearVariableMap();
    //m_pCommand->unregister();
    delete m_pCommand;
    TreeTestSupport::TeardownTestInterpreter();
  }
protected:
  void ListFunc();
  void ListDispat();
  void SetChanged();
  void SetProperties();
  void SetNoUnits();
  void CheckChanged();
  void Usage();
  void Traces();
private:
  // Utilities:

  vector<string> StringToList(string list);
  vector <string> ResultToList();


  void ListAllCheck(const char* comment);
  void ListIndivCheck(const char* comment);
  void ListArrayCheck(const char* comment);
  void ConsistentDescription(const char* pcomment, vector<string> desc);
};

CPPUNIT_TEST_SUITE_REGISTRATION(TreeVarCommandTest);


// Utilities.


// Ensure the description provided is consistent.

void
TreeVarCommandTest::ConsistentDescription(const char* pComment,
					  vector<string> desc)
{
  // Desc has three elements.

  EQMSG(pComment, (size_t)3, desc.size());

  // name must be present:

  CTreeVariable::TreeVariableIterator p = CTreeVariable::find(desc[0]);
  if(p == CTreeVariable::end()) {
    string msg = "Can't find variable" + desc[0] + pComment;
    FAIL(msg);
  }
  CTreeVariableProperties* pVar = p->second;
  double value = (double)(*pVar);
  string units = pVar->getUnits();

  EQMSG(pComment, units, desc[2]); // Units match.
  double dvalue;
  sscanf(desc[1].c_str(), "%lg", &dvalue);
  EQMSG(pComment, value, dvalue);
}

// Turn the parameter string into a string vector by pulling it apart as a TCL
// list.

vector<string>
TreeVarCommandTest::StringToList(string list)
{
  CTCLList tcllist(m_pInterp, list);
  vector<string> result;
  tcllist.Split(result);
  return result;
}

// Turn the current value of *the result string into a string vector that
// is the result of separating the result into its component list
// elements.
vector<string>
TreeVarCommandTest::ResultToList()
{
  string result = m_pInterp->GetResultString();
  return StringToList(result);
}

// Ensures that *the result string contains a string that is
// consistent with a listing of all the variables we defined.
//

void
TreeVarCommandTest::ListAllCheck(const char* comment)
{
  vector<string> result = ResultToList();
  EQMSG(comment, (size_t)11, result.size());
  for (int i =0; i < result.size(); i++) {
    vector<string> item = StringToList(result[i]);
    ConsistentDescription(item[0].c_str(), item);
  }
}

// Ensures that *the result string contains a string that is consistent with
// a listing of the "inidiv" variable.
//
void
TreeVarCommandTest::ListIndivCheck(const char* comment)
{
  vector<string> result = ResultToList();
  EQMSG(comment, (size_t)1, result.size()); // Exactly one element I think.
  vector<string> item   = StringToList(result[0]);
  ConsistentDescription(comment, item);      // Check consistency.
}
// Ensures that *the result string contains a string that is consistent with 
// a listing of the multi array.
//
void
TreeVarCommandTest::ListArrayCheck(const char* comment)
{
  vector<string> result = ResultToList();
  EQMSG(comment, (size_t)10U, result.size());
  for (int i = 0; i < result.size(); i++) {
    vector<string> item = StringToList(result[i]);
    ConsistentDescription(item[0].c_str(), item);
  }
}

// The syntax expected is:
//    treeparameter -list ?pattern?
// When the dispatcher calls the function, treeparameter and -list have been 
// eaten up leaving 0 or 1 parameers on the command line.
// We check for proper handling of the following conidtions:
//    - Too many command parameters.
//    - Correct list of single
//    - Correct list of all
//    - Correct list of array via wildcards.
//
void TreeVarCommandTest::ListFunc() 
{
  // Ensure proper handling of parameter count problems.

  char const* argv[2] = {"*", "extra"};

  int status = m_pCommand->List(*m_pInterp,
				2, (argv));
  EQMSG("2 parameters", TCL_ERROR, status);
  m_pInterp->setResult("");

  // Calling the function with no parameters should be the same
  // as calling it with "*"
  //

  status = m_pCommand->List(*m_pInterp,  0, (const char**)NULL);
  EQMSG("0 parameters ok", TCL_OK,  status);
  ListAllCheck("Listing all implicitly via direct list call");
  m_pInterp->setResult("");

  status = m_pCommand->List(*m_pInterp,  1, (argv));
  EQMSG("1 parameter '*' ok", TCL_OK,  status);
  ListAllCheck("Listing all explicitly via direct list call");  
  m_pInterp->setResult("");
  

  // Now look for "indiv" and list it:

  argv[0] = "indiv";
  status  = m_pCommand->List(*m_pInterp,  1, (argv));
  EQMSG("1 parameter 'indiv' ok", TCL_OK, status);
  ListIndivCheck("Listing 'indiv' via direct list call");
  m_pInterp->setResult("");

  // Look for all elements of the array:

  argv[0] = "multi*";
  status  = m_pCommand->List(*m_pInterp,  1, (argv));
  EQMSG("1 parameter 'multi*'", TCL_OK, status);
  ListArrayCheck("Listing 'multi.*' via direct list call");
  m_pInterp->setResult("");

}

// Same kinds of tests, but with the dispatcher involved.

void
TreeVarCommandTest::ListDispat()
{
  char const* argv[4] = {"treevariable", "-list", "[]", "extra"};
  std::vector<CTCLObject> objv;
  for (int i = 0; i < 4; i++) {
    CTCLObject o;
    o  = argv[i];
    objv.push_back(o);
  }
  // Bad parameter count:
  
  int status = (*m_pCommand)(*m_pInterp, objv);
  EQMSG("bad parameters dispatch", TCL_ERROR, status);
  m_pInterp->setResult("");

  // Implicit "*":

  objv.pop_back();
  objv.pop_back();
  status = (*m_pCommand)(*m_pInterp, objv);
  EQMSG("Implicit *", TCL_OK, status);
  ListAllCheck("Implicit * via dispatch");


  // explicit *:

  // This seems odd but the push_back does a copy of o so it
  // should be ok.
  objv.clear();
  CTCLObject o;
  o = argv[0];
  objv.push_back(o);
  o = argv[1];
  objv.push_back(o);
  o = "*";
  objv.push_back(o);
  
  status = (*m_pCommand)(*m_pInterp, objv);
  EQMSG("explicit *", TCL_OK, status);
  ListAllCheck("Explicit* via dispatch");
  m_pInterp->setResult("");

  //  Single element:

  objv[2] = "indiv";
  status = (*m_pCommand)(*m_pInterp, objv);
  EQMSG("individ", TCL_OK, status);
  ListIndivCheck("individ via dispatch");
  m_pInterp->setResult("");

  // All array elements.
  
  objv[2] = "multi*";
  status  = (*m_pCommand)(*m_pInterp, objv);
  EQMSG("Multi", TCL_OK, status);
  ListArrayCheck("multi via dispatch");
  m_pInterp->setResult("");
  
}

// Setchanged sets the changed flag for the variable (the state changed).
// Conditions we look for are:
//    Too few parameters.
//    Too many parameters
//    No such parameter.
//    That the properly functioning command sets the changed flag of the
//    variable it specifies.
void
TreeVarCommandTest::SetChanged()
{
  char const* argv[2]   = {"indiv", "extra"};
  char const *cmdargv[3] = {"treevariable", "-setchanged", "indiv", }; 

  // Too few parameters:

  int status = m_pCommand->SetChanged(*m_pInterp,  0, (argv));
  EQMSG("Too few params", TCL_ERROR, status);

  // Too many params.

  status = m_pCommand->SetChanged(*m_pInterp, 2, (argv));
  EQMSG("Too many parameters", TCL_ERROR, status);

  // Correct functionality.
  
  status = m_pCommand->SetChanged(*m_pInterp, 1, (argv));
  EQMSG("Just right parameter count", TCL_OK, status);
  ASSERT(m_pIndividual->hasChanged());
  m_pIndividual->resetChanged();



  // Fails correctly when nonexistent parameter.

  argv[0] = "nonexist";
  status  = m_pCommand->SetChanged(*m_pInterp, 1, (argv));
  EQMSG("nonexistent parameter", TCL_ERROR, status);

  // Works correctly via command dispatch.
  
  std::vector<CTCLObject> objv;
  for (int i = 0; i < 3; i++) {
    CTCLObject o;
    o = cmdargv[i];
    objv.push_back(o);
  }
  status = (*m_pCommand)(*m_pInterp,  objv);
  EQMSG("Dispatch", TCL_OK, status);
  ASSERT(m_pIndividual->hasChanged());


}
//
// Set the properties of the variable.  This expects three parameters
//   name, units value.
//  We check for the following conditions:
//    Too few parameters.
//    Too many parameters.
//    Value is not a good double.
//    name is not a variable name.
//    Correct functioning including modification of the underlying variable.
//

void 
TreeVarCommandTest::SetProperties()
{
  char const* argv[4] = {"nosuch", "aaaa", "mm", "extra"};

  int status = m_pCommand->SetProperties(*m_pInterp,
					 1, (argv));
  EQMSG("too few params", TCL_ERROR, status);
  m_pInterp->setResult("");

  status = m_pCommand->SetProperties(*m_pInterp, 4, (argv));
  EQMSG("too many parameters", TCL_ERROR, status);
  m_pInterp->setResult("");


  argv[0] = "indiv";
  status = m_pCommand->SetProperties(*m_pInterp, 3, (argv));
  EQMSG("Invalid fp value", TCL_ERROR , status);
  m_pInterp->setResult("");


  argv[0] = "nosuch";
  argv[1] = "55.55";
  status = m_pCommand->SetProperties(*m_pInterp, 3, (argv));
  EQMSG("Invalid variable name", TCL_ERROR, status);
  m_pInterp->setResult("");

  argv[0] = "indiv";
  status = m_pCommand->SetProperties(*m_pInterp, 3, (argv));
  EQMSG("indiv good set via function", TCL_OK, status);
  EQMSG("indiv value", 55.55, m_pIndividual->getValue());
  EQMSG("indiv units", string("mm"), m_pIndividual->getUnit());
  


  // Set via command dispatch:

  char const* cmdargv[5] = {"treevariable", "-set", "indiv",  "2.22", "in"};
  std::vector<CTCLObject> objv;
  for (int i =0; i < 5; i++) {
    CTCLObject o;
    o = cmdargv[i];
    objv.push_back(o);
  }
  status = (*m_pCommand)(*m_pInterp, objv);
  EQMSG("indiv good set via dispatch", TCL_OK, status);
  EQMSG("Indiv value", 2.22, m_pIndividual->getValue());
  EQMSG("Indiv units", string("in"),  m_pIndividual->getUnit());

  m_pInterp->setResult("");

  

}

// New functionality test:
//   Check for treevariableset without supplying units:
//
void
TreeVarCommandTest::SetNoUnits()
{
  char const* argv[4] = {"treevariable", "-set", "indiv", "2.22"};
  std::vector<CTCLObject> objv;
  for (int i =0; i < 4; i++) {
    CTCLObject o;
    o = argv[i];
    objv.push_back(o);
  }
  int status = (*m_pCommand)(*m_pInterp, objv);
  EQMSG("Nounits set status", TCL_OK, status);
  EQMSG("Nounits set value",  2.22, m_pIndividual->getValue());
  EQMSG("Nounits set units", string(""), m_pIndividual->getUnit());
}

// Test for:
//   function handles too few arguments.
//   function handles too many arguments.
//   function handles non-existent variable.
//   function returns 0 for unmodified variable.
//   function returns 1 for modified variable.
//   dispatch for modified variable works to get 1.
//
void
TreeVarCommandTest::CheckChanged()
{
  char const* argv[2] = {"nosuch", "extra"};
  int   status;

  // parameter count errors.

  status = m_pCommand->CheckChanged(*m_pInterp, 0, (argv));
  EQMSG("too few params", TCL_ERROR, status);
  m_pInterp->setResult("");

  status = m_pCommand->CheckChanged(*m_pInterp, 2, (argv));
  EQMSG("Too many params", TCL_ERROR, status);
  m_pInterp->setResult("");

  // No such variable:

  status = m_pCommand->CheckChanged(*m_pInterp, 1, (argv));
  EQMSG("bad name", TCL_ERROR, status);
  m_pInterp->setResult("");

  // unmodified:

  argv[0]= "indiv";
  status = m_pCommand->CheckChanged(*m_pInterp, 1, (argv));
  EQMSG("good", TCL_OK, status);
  string result(m_pInterp->GetResultString());
  EQMSG("good -unchanged", string("0"), result);
  m_pInterp->setResult("");


  // Modify "inidiv" and check that CheckChanged reflects this:

  (*m_pIndividual) = 3.1416;
  status = m_pCommand->CheckChanged(*m_pInterp, 1, (argv));
  EQMSG("good[mod]", TCL_OK, status);
  result = (m_pInterp->GetResultString());
  EQMSG("good[modified]", string("1"), result);
  m_pInterp->setResult("");


  // Check that command dispatch also gives us the same thing:

  char const* cmdargv[3] = {"treevariable", "-check", "indiv"};
  std::vector<CTCLObject> objv;
  for (int i=0; i < 3; i++) {
    CTCLObject o;
    o = cmdargv[i];
    objv.push_back(o);
  }
  status = (*m_pCommand)(*m_pInterp,objv); 
  EQMSG("good[mod]dispatch", TCL_OK, status);
  result = m_pInterp->GetResultString();
  EQMSG("good[modified]dispatch", string("1"), result);
  m_pInterp->setResult("");
  
}
//

static string usagesb("Usage\n\
    treevariable -list ?pattern?\n\
    treevariable -set name value ?units?\n\
    treevariable -check name\n\
    treevariable -setchanged name\n\
    treevariable -firetraces ?pattern?\n");

void 
TreeVarCommandTest::Usage()
{
  // Can't dispatch as we'll get extra stuff

  string usage = m_pCommand->Usage();
  EQ(usagesb, usage);
}

// The stuff below is used in trace testing:

static int traces;
static char*
myTrace(ClientData cd, Tcl_Interp *pRawInterp, const char* basename,
	const char* index, int flags)
{
  traces++;
  return NULL;
}

void
TreeVarCommandTest::Traces()
{
  CTreeVariable::BindVariables(*m_pInterp); // need bindings for traces.
  traces = 0;
  Tcl_TraceVar(m_pInterp->getInterpreter(), "indiv", TCL_TRACE_WRITES,
	       myTrace, (ClientData)NULL);

  // Just modifying the values now fires traces.

  char const* setargv[5] = {"treevariable", "-set", "indiv", "2.22", "in", };
  std::vector<CTCLObject> objv;
  for (int i = 0; i < 5; i++) {
    CTCLObject o;
    o = setargv[i];
    objv.push_back(o);
  }
  int status = (*m_pCommand)(*m_pInterp, objv);
  EQMSG("set status", TCL_OK, status);
  EQMSG("set trace count", 1, traces);

  // Now fire the traces:

  char const* traceargv[2] = {"treevariable", "-firetraces"};
  objv.clear();
  for (int i =0; i < 2; i++) {
    CTCLObject o;
    o = traceargv[i];
    objv.push_back(o);
  }
  status = (*m_pCommand)(*m_pInterp, objv);
  EQMSG("trace status", TCL_OK, status);
  EQMSG("trace trace count", 1, traces);
}
