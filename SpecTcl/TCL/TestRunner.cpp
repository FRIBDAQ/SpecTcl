#include <config.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/ui/text/TestRunner.h>
#include <string>

#include <Iostream.h>

#ifdef HAVE_STD_NAMESPACE
using namespace std;
#endif


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
