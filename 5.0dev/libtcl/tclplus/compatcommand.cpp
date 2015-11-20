// Template for a test suite.

#include <config.h>
#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/Asserter.h>
#include "Asserts.h"
#include "TCLInterpreter.h"
#include "TCLProcessor.h"
#include "TCLResult.h"



using namespace std;



/*  The CTCLProcessor is an abstract base class.  It is therefore necessary
    to create a concrete class and instantiate it to do any testing.
    This command will reverse its arguments (command name included).
*/

class Reverse : public CTCLProcessor
{
  bool deleted;
  bool preCalled;
  bool postCalled;
public:
  Reverse(CTCLInterpreter* pInterp, string command = string("reverse"));
  ~Reverse();

  virtual int operator()(CTCLInterpreter& interp,
			 CTCLResult&      result,
			 int argc, char** argv);
  virtual void OnDelete();

  bool isDeleted();
  bool wasPreCalled();
  bool wasPostCalled();

  virtual void preCommand();
  virtual void postCommand();
};

// Implementing the test class;

Reverse::Reverse(CTCLInterpreter* pInterp,
		 string command) :
  CTCLProcessor(command, pInterp),
  deleted(false),
  preCalled(false),
  postCalled(false)
{
  Register();
}
Reverse::~Reverse()
{
  Unregister();
}

int
Reverse::operator()(CTCLInterpreter& interp,
		    CTCLResult&      result,
		    int argc, char** argv)
{
  for(int i = (argc-1); i >= 0; i--) {
    result.AppendElement(argv[i]);
  }
  return TCL_OK;
}

void
Reverse::OnDelete()
{
  deleted = true;
}
bool
Reverse::isDeleted() 
{
  return deleted;
}
void
Reverse::preCommand()
{
  preCalled = true;
}
void
Reverse::postCommand()
{
  postCalled = true;
}

bool
Reverse::wasPreCalled()
{
  return preCalled;
}
bool
Reverse::wasPostCalled()
{
  return postCalled;
}

///// The tests.

class argvprocessor : public CppUnit::TestFixture {
  CPPUNIT_TEST_SUITE(argvprocessor);
  CPPUNIT_TEST(construction);
  CPPUNIT_TEST(invoke);
  CPPUNIT_TEST(unregister);
  CPPUNIT_TEST_SUITE_END();


private:
  Tcl_Interp*        m_pRawInterp;
  CTCLInterpreter*   m_pInterpreter;
  Reverse*           m_pCommand;
public:
  void setUp() {
    m_pRawInterp   = Tcl_CreateInterp();
    m_pInterpreter = new CTCLInterpreter(m_pRawInterp);
    m_pCommand     = new Reverse(m_pInterpreter);
    m_pCommand->Register();
    
  }
  void tearDown() {
    delete m_pCommand;
    delete m_pInterpreter;

  }
protected:
  void construction();
  void invoke();
  void unregister();
};

CPPUNIT_TEST_SUITE_REGISTRATION(argvprocessor);

// Ensure that we can get this thing constructed.

void argvprocessor::construction() {
  EQMSG("command name", string("reverse"), m_pCommand->getCommandName());

  Tcl_CmdInfo  info;
  int status = Tcl_GetCommandInfo(m_pRawInterp, "reverse", &info);
  ASSERT(status == 1);		// It's been registered.

}
// See if we can invoke the command.../

void argvprocessor::invoke() {
  string result = m_pInterpreter->Eval("reverse a b cd e");
  EQ(string("e cd b a reverse"), result);
  ASSERT(m_pCommand->wasPreCalled());
  ASSERT(m_pCommand->wasPostCalled());
}
// Now see that deletion is relayed correctly.

void argvprocessor::unregister()
{
  m_pCommand->Unregister();
  ASSERT(m_pCommand->isDeleted());
}
