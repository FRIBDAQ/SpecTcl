//    This software is Copyright by the Board of Trustees of Michigan
//    State University (c) Copyright 2016.
//
//    This program is free software: you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation, either version 3 of the License, or
//    any later version.
//
//    This program is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License
//    along with this program.  If not, see <http://www.gnu.org/licenses/>.
//
//    Authors:
//    Jeromy Tompkins
//    NSCL
//    Michigan State University
//    East Lansing, MI 48824-1321

#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/ui/text/TestRunner.h>
#include <stdlib.h>
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

    exit( !wasSuccessful);
}
