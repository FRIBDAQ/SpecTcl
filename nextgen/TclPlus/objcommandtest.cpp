// Template for a test suite.

#include <config.h>
#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/Asserter.h>
#include "Asserts.h"
#include "TCLInterpreter.h"
#include "TCLObject.h"
#include "TCLObjectProcessor.h"
#include "TCLResult.h"

#include <vector>


using namespace std;


// The CTCLObjectProcessor class is abstract and therefore cannot be directly
// tested.  Below is the definition and implementation of an object oriented
// echo command.  This command also knows if its been deleted.
//

class ObjectEcho : public CTCLObjectProcessor
{
private:
  bool deleted;
public:
  ObjectEcho(CTCLInterpreter* pInterp,
	     string           command = string("echo"));
  virtual ~ObjectEcho();
private:
  ObjectEcho(const ObjectEcho& rhs);
  ObjectEcho& operator=(const ObjectEcho& rhs);
  int operator==(const ObjectEcho& rhs) const;
  int operator!=(const ObjectEcho& rhs) const;
public:
  // Selectors:

  bool isDeleted();

  // Overrides.
  virtual int operator()(CTCLInterpreter& interp,
			 vector<CTCLObject>& objv);
  virtual void onUnregister();


};

/// Implementation of ObjectEcho:

ObjectEcho::ObjectEcho(CTCLInterpreter* pInterp,
		       string           command) :
  CTCLObjectProcessor(*pInterp, command, true),
  deleted(false)
{}
ObjectEcho::~ObjectEcho() {}

bool
ObjectEcho::isDeleted() {
  return deleted;
}

int
ObjectEcho::operator()(CTCLInterpreter& interp,
		       vector<CTCLObject>& objv)
{
  {
    CTCLResult result(&interp);
    result.setList(objv);
    result.commit();
  }

  return TCL_OK;
}

void 
ObjectEcho::onUnregister()
{
  deleted = true;
}

class objectcommand : public CppUnit::TestFixture {
  CPPUNIT_TEST_SUITE(objectcommand);
  CPPUNIT_TEST(construction);
  CPPUNIT_TEST(unregisterCallback);
  CPPUNIT_TEST(invoke);
  CPPUNIT_TEST_SUITE_END();


private:
  Tcl_Interp*       m_pRawInterp;
  CTCLInterpreter*  m_pInterp;
  ObjectEcho*       m_pCommand;
public:
  void setUp() {
    m_pRawInterp = Tcl_CreateInterp();
    m_pInterp    = new CTCLInterpreter(m_pRawInterp);
    m_pCommand   = new ObjectEcho(m_pInterp, "echo");
  }
  void tearDown() {
    delete m_pCommand;
    delete m_pInterp;
  }
protected:
  void construction();
  void unregisterCallback();
  void invoke();
};

CPPUNIT_TEST_SUITE_REGISTRATION(objectcommand);

// Check that the command is constructed properly.
void objectcommand::construction() {
  EQMSG("name", string("echo"), m_pCommand->getName());

  Tcl_CmdInfo info = m_pCommand->getInfo();

  EQMSG("client data",   (ClientData)m_pCommand, info.objClientData);

  EQMSG("delete data",   (ClientData)m_pCommand, info.deleteData);
}


void objectcommand::unregisterCallback()
{
  m_pCommand->unregister();
  ASSERT(m_pCommand->isDeleted());
}

// Execute the command:

void objectcommand::invoke()
{
  string result = m_pInterp->Eval("echo this is a test evaluation");
  EQ(result, string("echo this is a test evaluation"));
}
