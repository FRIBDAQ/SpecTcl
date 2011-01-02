#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/ui/text/TestRunner.h>
#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/Asserter.h>
#include "Asserts.h"
#include <string>
#include <iostream>
#include "TclCreate.h"

using namespace std;

int main(int argc, char** argv)
{

  CppUnit::TextUi::TestRunner   
               runner; // Control tests.
  CppUnit::TestFactoryRegistry& 
               registry(CppUnit::TestFactoryRegistry::getRegistry());

  runner.addTest(registry.makeTest());

  bool wasSucessful;
  try {
    wasSucessful = runner.run("",false);
  } 
  catch(string& rFailure) {
    cerr << "Caught a string exception from test suites.: \n";
    cerr << rFailure << endl;
    wasSucessful = false;
  }
  return !wasSucessful;
}


class tclhandles : public CppUnit::TestFixture {
  CPPUNIT_TEST_SUITE(tclhandles);
  CPPUNIT_TEST(instance);
  CPPUNIT_TEST_SUITE_END();


private:

public:
  void setUp() {
  }
  void tearDown() {
  }
protected:
  void instance();
};

CPPUNIT_TEST_SUITE_REGISTRATION(tclhandles);


// We should be able to get the handle manager instance 
// it sould not be null and it should always be the same:
//

void tclhandles::instance()
{
  CHandleManager* p1 = CTclDBCommand::getHandleManager();
  CHandleManager* p2 = CTclDBCommand::getHandleManager();

  ASSERT(reinterpret_cast<CHandleManager*>(NULL) != p1);

  EQ(p1, p2);
}

void* gpTCLApplication(0);
