// Tests for packages and packaged commands.

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/Asserter.h>
#include "Asserts.h"

#include "TCLObjectPackage.h"
#include "TCLPackagedObjectProcessor.h"

#include <tcl.h>
#include "TCLInterpreter.h"

using namespace std;

///////////////////////////////////////////////////////

class testCommand : public CTCLPackagedObjectProcessor
{
  bool initialized;
public:
  testCommand(CTCLInterpreter& interp);

  virtual int operator()(CTCLInterpreter& interp,
			 vector<CTCLObject>& objv);
  CTCLObjectPackage* package();
  bool isInitialized() const;
  virtual void Initialize();
};
testCommand::testCommand(CTCLInterpreter& interp) :
  CTCLPackagedObjectProcessor(interp, "test"),
  initialized(false)
{
  
}
int
testCommand::operator()(CTCLInterpreter& interp,
		vector<CTCLObject>& objv)
{
  return TCL_OK;
}

CTCLObjectPackage*
testCommand::package()
{
  return getPackage();
}

bool
testCommand::isInitialized() const
{
  return initialized;
}
void
testCommand::Initialize()
{
  initialized = true;
}


///////////////////////////////////////////////////

class PackageTest : public CppUnit::TestFixture {
  CPPUNIT_TEST_SUITE(PackageTest);
  CPPUNIT_TEST(hooktest);
  CPPUNIT_TEST_SUITE_END();


private:
  Tcl_Interp*     m_pRawInterpreter;
  CTCLInterpreter* m_pInterp;
public:
  void setUp() {
    m_pRawInterpreter = Tcl_CreateInterp();
    Tcl_Init(m_pRawInterpreter);
    m_pInterp   = new CTCLInterpreter(m_pRawInterpreter);
  }
  void tearDown() {
    delete m_pInterp;
    Tcl_DeleteInterp(m_pRawInterpreter);
  }
protected:
  void hooktest();
};

CPPUNIT_TEST_SUITE_REGISTRATION(PackageTest);

void PackageTest::hooktest() {
  CTCLObjectPackage pkg;
  testCommand       cmd(*m_pInterp);
  pkg.addCommand(&cmd);

  // should be one and only one command in the
  // pacakge and should be &cmd:

  CTCLObjectPackage::CommandIterator i = pkg.begin();
  EQ(reinterpret_cast<CTCLPackagedObjectProcessor*>(&cmd), *i);

  i++;
  ASSERT(i == pkg.end());

  // Package should be initialized and can get the package:

  EQ(&pkg, cmd.package());
  EQ(true, cmd.isInitialized());
  
}
