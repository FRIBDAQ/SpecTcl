// Template for a test suite.

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/Asserter.h>
#include "Asserts.h"
#include <iostream>
#include <tcl.h>
#include <TCLInterpreter.h>
#include <TCLList.h>
#include <TCLResult.h>
#include <CFitCommand.h>
#include <CFitFactory.h>
#include <CFit.h>

#include <vector>		// STL Vector.
#include <string>		// STL STring.
#include <string.h>		// C runtime string.
#include <stdio.h>
#include <math.h>

void* gpTCLApplication;			// Expected by TCL lib.


class CommandTest : public CppUnit::TestFixture {
  CPPUNIT_TEST_SUITE(CommandTest);
  CPPUNIT_TEST(CreateTest);
  CPPUNIT_TEST(DeleteTest);
  CPPUNIT_TEST(ListTest);
  CPPUNIT_TEST(AddPoints);
  CPPUNIT_TEST(Perform);
  CPPUNIT_TEST(Evaluate);
  CPPUNIT_TEST_SUITE_END();

  // Must define the function for each test.

protected:
  void CreateTest();
  void DeleteTest();
  void ListTest();
  void AddPoints();
  void Perform();
  void Evaluate();

  int PerformUtility(char* pName);
private:

  // These are created and torn down for each test.

  Tcl_Interp*        m_pInterpreter;
  CTCLInterpreter*   m_pInterp;
  CTCLResult*        m_pResult;
  CFitCommand*       m_pFitCommand;


public:
  CommandTest() :
    m_pInterpreter(0),
    m_pInterp(0),
    m_pResult(0),
    m_pFitCommand(0)
  {
  }

  ~CommandTest() {
    tearDown();			// In case destroyed before teardown.
  }

  void setUp() {
    // Create the interpreter, bind it to an object and


    m_pInterpreter = Tcl_CreateInterp();
    Tcl_Init(m_pInterpreter);
    m_pInterp = new CTCLInterpreter(m_pInterpreter);

    
    // Create and register the fit command:

    m_pFitCommand = new CFitCommand(m_pInterp);	// defines "fit".

    // Create a result object bound to the interp:

    m_pResult = new CTCLResult(m_pInterp);

  }

  void tearDown() {
    // Destroy the fit command (unregisters it):

    delete m_pFitCommand;

    // Delete the interpreter object and the interp:

    delete m_pResult;
    delete m_pInterp;

    //    Tcl_DeleteInterp(m_pInterpreter);

    m_pFitCommand = (CFitCommand*)NULL;
    m_pInterp     = (CTCLInterpreter*)NULL;
    m_pInterpreter= (Tcl_Interp*)NULL;
    m_pResult     = (CTCLResult*)NULL;
  }


};

CPPUNIT_TEST_SUITE_REGISTRATION(CommandTest);

/*
Test Name	 CreateTest
Test Objective	 Check ability to create a fit via command parse
Test Description	
1.	Create TCL interpreter/object (setup)
2.	Create CFitCommand object registered on interp (setup)
3.	Invoke Create_parse with new fit name.
4.	Search for fit directly from factory.
5.	Create a duplicate fit.
6.	Delete fit from factory
7.	Destroy CFitCommand (teardown)
8.	Destroy TCL Interpreter/object) (teardown).

Expected Results	
1.	NA
2.	N/A
3.	TCL_OK returned.
4.	Found a fit with that name.
5.	TCL_ERROR returned.
6.	N/A
7.	N/A
8.	N/A

 */

static char* create_args[] = {"linear",
				    "testfit"
};
static const int create_count = sizeof(create_args)/sizeof(char*);
void CommandTest::CreateTest() 
{
  cerr << "Command test\n";

  int status = m_pFitCommand->Create_parse(*m_pInterp, *m_pResult,
					   create_count, create_args);
  EQMSG("Create_parse return", TCL_OK, status);

  CFitFactory::FitIterator i = CFitFactory::FindFit("testfit");
  ASSERT(i != CFitFactory::end());

  // Now go for a duplicate fit:

  status = m_pFitCommand->Create_parse(*m_pInterp, *m_pResult,
				       create_count, create_args);
  EQMSG("Duplicate create", TCL_ERROR, status);	// should have failed.

  CFitFactory::Delete("testfit"); // This cleanup is not done in teardown.
}


/*
Test Name	DeleteTest 
Test Objective	Check command parsing for delete
Test Description	
1.	See setup above
2.	Create a linear fit with known name.
3.	Delete it via Delete_parse
4.	Search for fit directly in factory
5.	Delete it again via Delete_parse
6.	See teardown above.

Expected Results	
1.	N/A
2.	N/A
3.	Return is TCL_OK
4.	end() returned
5.	TCL_ERROR is returned
6.	N/A.

*/

static char* delete_args[] = { "testfit" };
static const int delete_count = sizeof(delete_args)/sizeof(char*);
void CommandTest::DeleteTest()
{
  int status = m_pFitCommand->Create_parse(*m_pInterp, *m_pResult,
					   create_count, create_args);
  EQMSG("Create_parse return", TCL_OK, status);

  status = m_pFitCommand->Delete_parse(*m_pInterp, *m_pResult,
				       delete_count, delete_args);
  EQMSG("Delete_parse return", TCL_OK, status);

  ASSERT(CFitFactory::FindFit("testfit") == CFitFactory::end());

  status = m_pFitCommand->Delete_parse(*m_pInterp, *m_pResult,
				       delete_count, delete_args);
  EQMSG("Delete_parse second return", TCL_ERROR, status);

}

/*
Test Name/#	ListTest 
Test Objective	 Test commands to list fits.
Test Description	
1.	Set Setup above
2.	Create linear fits: fit1 fit2 george.
3.	List for all
4.	Create TCLList of results and query size.
5.	List for fit*
6.	Create TCLList of results and query size
7.	list for george
8.	Create TCLList of results and split
9.	Delete the fits.
10.	Tear down as above

Expected Results	
1.	NA
2.	na
3.	TCL_OK returned.
4.	3 elements
5.	NA
6.	2 elements
7.	NA
8.	Following:
   a.	list size is 1
   b.	Element 0 is "george"
   c.	Element 1 is "accepting"
   d.	Element 2 is empty
   e.	Element 3 is empty
9.	Na
10.	NA
 */

char fitname[1000] = "";

static char* listpattern_args[] = {
  fitname
};
static const int listpattern_count = sizeof(listpattern_args)/sizeof(char*);

void CommandTest::ListTest()
{
  // Create the fits:

  CFitFactory::Create("linear", "fit1");
  CFitFactory::Create("linear", "fit2");
  CFitFactory::Create("linear", "george");

  // List all fits:

  int status = m_pFitCommand->List_parse(*m_pInterp, *m_pResult,
			  0, (char**)NULL); // Default pattern is *.
  EQMSG("list *", TCL_OK, status);
  
  CTCLList listall(m_pInterp, (const char*)(*m_pResult));
  StringArray elements;
  EQMSG("list * split status", TCL_OK,  listall.Split(elements));
  EQMSG("list * size", (size_t)3, elements.size());
  elements.erase(elements.begin(), elements.end());


  // List the fits that match fit*

  strcpy(fitname, "fit*");
  status  = m_pFitCommand->List_parse(*m_pInterp, *m_pResult,
				      listpattern_count, listpattern_args);
  EQMSG("list fit*", TCL_OK, status);
  CTCLList listfitstar(m_pInterp, (const char*)(*m_pResult));
  EQMSG("list fit* split status", TCL_OK, listfitstar.Split(elements));
  EQMSG("list fit* size", (size_t)2, elements.size());
  elements.erase(elements.begin(), elements.end());

		       
  // List only george:

  strcpy(fitname, "george");
  status  = m_pFitCommand->List_parse(*m_pInterp, *m_pResult,
				      listpattern_count, listpattern_args);
  EQMSG("list fit*", TCL_OK, status);
  CTCLList listgeorge(m_pInterp, (const char*)(*m_pResult));
  EQMSG("list george split status", TCL_OK, listgeorge.Split(elements));
  EQMSG("list george size", (size_t)1, elements.size());

  CTCLList george(m_pInterp, elements[0]); // George's desription list
  StringArray georgelist;
  EQMSG("element list status", TCL_OK, george.Split(georgelist));
  EQMSG("element list size",   (size_t)4,  georgelist.size());

  EQMSG("name",   string("george"),    georgelist[0]);
  EQMSG("state",  string("accepting"), georgelist[1]);
  EQMSG("points", string(""),          georgelist[2]);
  EQMSG("params", string(""),          georgelist[3]);
  

  // Delete the fits:

  CFitFactory::Delete("fit1");
  CFitFactory::Delete("fit2");
  CFitFactory::Delete("george");
}
/*
Test Name/#	 AddPoints 
Test Objective	 Test commands to add points.
Test Description	
1.	Setup as above.
2.	Create  a fit.
3.	Add a point to it.
4.	Add a second point to it.
5.	Find the fit in the fit factory.
6.	Ask it how many fits it has
7.	Iterate the fit points
8.	Destroy the fit
9.	tear down as above.
Expected Results	
1.	NA
2.	NA
3.	TCL_OK Returned
4.	TCL_OK returned
5.	NA
6.	2
7.	Find both the points I inserted in the fit.
8.	NA
9.	NA

 */

static CFit::Point testPoints[] = {
  {1.0, 2.5},
  {3.5, 7.25}
};

static int ntestPoints = sizeof(testPoints)/sizeof(CFit::Point);

void CommandTest::AddPoints()
{
  CFit* pFit = CFitFactory::Create("linear", "test"); // easiest way.

  // Create the command string, we'll later split it:

  string command("test");
  for(int i =0; i < ntestPoints; i++) {
    char apoint[100];
    sprintf(apoint, " {%f %f}", testPoints[i].x, testPoints[i].y);
    command += apoint;
  }
  // Turn command into an arg/argv list.

  CTCLList lCommand(m_pInterp, command);
  int    argc;
  char **argv;
  lCommand.Split(argc, &argv);

  // Now add the points:

  int status = m_pFitCommand->AddPoints_parse(*m_pInterp, *m_pResult,
					      argc, argv);
  // Check that all is well.

  EQMSG("add status", TCL_OK, status);
  EQMSG("fit point count", (size_t)2, pFit->size());

  CFit::PointIterator pPoint = pFit->begin();
  int pt = 0;
  while(pPoint != pFit->end()) {
    EQMSG("x: ", testPoints[pt].x, pPoint->x);
    EQMSG("y: ", testPoints[pt].y, pPoint->y);
    pPoint++;
    pt++;
  }

  CFitFactory::Delete("test");

}


/*
Test Name/#	Perform 
Test Objective	 Test command to perform a fit.
Test Description	
1.	Setup as above.
2.	create a fit
3.	Add the points to it near the line y = 2.5x - 7
4.	Request the perform.
5.	Result -> TCLList
6.	size of list?
7.	Break apart list.
    a.	decode slope
    b.	decode offset
8.	Teardown as above (note fit remains for next test).

Expected Results	
1.	Na
2.	NA
3.	NA
4.	TCL_OK returned
5.	NA
6.	3
7.	slope and offset are within 10% of actual.
8.	NA

*/
static double m         = 2.5;
static double b         = -7;
static double jitter    = 0.01;
static double tolerance = 0.1;

int CommandTest::PerformUtility(char* name)
{
  CFitFactory::FitIterator p = CFitFactory::FindFit(name);
  CFit*       pFit = p->second;

  for(int i =0; i < 10; i++) {	// We'll use 10 points.
    CFit::Point pt;
    pt.x = (double)i;
    pt.y = m*pt.x + b;		// This is the unjitter y.
    pt.y += jitter * (drand48() - 0.5); // jitter it.

    pFit->AddPoint(pt);
  }

  // Now do the fit:

  int status = m_pFitCommand->Perform_parse(*m_pInterp, *m_pResult,
					    1, &name);
  return status;
}

void CommandTest::Perform()
{
  char* name = "test";
  CFit* pFit = CFitFactory::Create("linear", name);

  int status = PerformUtility(name);

  EQMSG("status: ", TCL_OK, status);

  // There should be 3 elements in the result list:

  CTCLList params(m_pInterp, (const char*)(*m_pResult));
  vector<string> parameters;
  params.Split(parameters);
  EQMSG("num params:", (size_t)3, parameters.size());

  // Pick them apart and require the slope and offset be in tolerance of m, b

  int found(0);
  for(int i=0; i < parameters.size(); i++) {
    CTCLList aparam(m_pInterp, parameters[i]);
    vector<string>  oneparam;
    aparam.Split(oneparam);
    EQMSG("one param size", (size_t)2, oneparam.size());

    if(oneparam[0] == string("slope")) {
      double slope;
      sscanf(oneparam[1].c_str(), "%lf", &slope);
      ASSERT((fabs(m - slope) < fabs(tolerance*m)) &&
	     (copysign(1.0, slope) == copysign(1.0, m)));
      found++;
    } 
    if(oneparam[0] == string("offset")) { 
      double offset;
      sscanf(oneparam[1].c_str(), "%lf", &offset);
      ASSERT((fabs(b - offset)*copysign(1.0, b) < fabs(tolerance*b)) &&
	     (copysign(1.0, offset) == copysign(1.0, b)));
      found++;
    }
  }
  EQMSG("All params", 2, found);

  CFitFactory::Delete(name);
}
/*
Test Name/#	 Evaluate
Test Objective	 Test evaluate command.
Test Description	
1.	Setup as above and perform the fit as for Peform().
2.	Locate fit in factory (ensure its there)
3.	Check state
4.	For a set of point Evaluate 
5.	Check result value against raw eval of fit.
6.	teardown.
Test Conditions	
Expected Results	
1.	NA
2.	found
3.	performed
4.	TCL_OK for each point.
5.	match
6.	NA


 */
void CommandTest::Evaluate()
{
  char* name = "test";
  CFit* pFit = CFitFactory::Create("linear", "test");
  
  PerformUtility(name);


  // Now evaluate 10 points starting at x = 7.8 and stepping by
  // approximately pi.

  double x = 7.8;
  for(int i=0; i < 10; i++) {
    char pBuffer[100];		// command -> here then split..
    sprintf(pBuffer, "%s %g", name, x);
    CTCLList command(m_pInterp, pBuffer);
    int    argc;
    char** argv;
    command.Split(argc, &argv);

    int status = m_pFitCommand->Evaluate_parse(*m_pInterp, *m_pResult,
					       argc, argv);

    EQMSG("Eval status: ", TCL_OK, status);
    string result((const char*)*m_pResult);
    double value;
    sscanf(result.c_str(), "%lf", &value);
    double actual = (*pFit)(x);

    ASSERT(fabs(actual - value) < (double)0.001);

    Tcl_Free((char*)argv);
    x += 3.14159265359;		// increment of pi.
  }
}
