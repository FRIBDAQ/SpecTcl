#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/ui/text/TestRunner.h>

#include <QApplication>
#include <QTimer>

int main( int argc, char* argv[])
{
    // Generate the TestRunner
    CppUnit::TextUi::TestRunner runner;
    CppUnit::TestFactoryRegistry &registry = CppUnit::TestFactoryRegistry::getRegistry();
    
    QApplication app(argc, argv);

    // Add the test suites
    runner.addTest( registry.makeTest() );

    // Run the tests
    runner.run();

    QTimer::singleShot(100, &app, SLOT(quit()));
    return app.exec();
}
