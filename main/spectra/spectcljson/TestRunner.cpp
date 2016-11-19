#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/ui/text/TestRunner.h>

#include <iostream>
using namespace std;

int main( int argc, char* argv[])
{
    // Generate the TestRunner
    CppUnit::TextUi::TestRunner runner;
    CppUnit::TestFactoryRegistry &registry = CppUnit::TestFactoryRegistry::getRegistry();
    
    bool wasSuccessful = false;
    try {
        // Add the test suites
        runner.addTest( registry.makeTest() );

        // Run the tests
        wasSuccessful = runner.run();
    } catch (std::string& rFailure) {
        cerr << "Caught a string exception from the test suites.: \n";
        cerr << rFailure << endl;
    }

    return !wasSuccessful;
}
