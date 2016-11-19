#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/ui/text/TestRunner.h>

#include <QApplication>
#include <QTimer>

#include <iostream>
using namespace std;

int main( int argc, char* argv[])
{
    // Generate the TestRunner
    CppUnit::TextUi::TestRunner runner;
    CppUnit::TestFactoryRegistry &registry = CppUnit::TestFactoryRegistry::getRegistry();
    
    QApplication app(argc, argv);

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

    QTimer::singleShot(100, &app, SLOT(quit()));
    app.exec();

    return !wasSuccessful;
}
