

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


#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/Asserter.h>

#include "Asserts.h"

#define private public
#include "PrintingSystem.h"
#undef private

#include "TestUtils.h"

#include <QTextStream>
#include <QString>
#include <QStringList>
#include <QFile>

using namespace std;

namespace Viewer
{

class PrintingSystemTests : public CppUnit::TestFixture
{
  public:
    CPPUNIT_TEST_SUITE( PrintingSystemTests );
    CPPUNIT_TEST( getAvailablePrinters_0 );
    CPPUNIT_TEST( getAvailablePrinters_1 );
    CPPUNIT_TEST( askSystemForAvailablePrinters_0);
    CPPUNIT_TEST_SUITE_END();


  public:
    void setUp() {}
    void tearDown() {}

  protected:
    void getAvailablePrinters_0()
    {

        QString printers("printer0\nprinter1\nprinter2");
        QStringList expected;
        expected << "printer0";
        expected << "printer1";
        expected << "printer2";

        QTextStream stream(&printers);

        QStringList printerList = PrintingSystem::instance().extractPrintersFromStream(stream);

        ASSERTMSG("All printers make it into the list of available printers",
                  expected == printerList);
    }

    void getAvailablePrinters_1()
    {

        QString printers;
        QStringList expected;
        expected << "lp";

        QTextStream stream(&printers);

        QStringList printerList = PrintingSystem::instance().extractPrintersFromStream(stream);

        ASSERTMSG("If no printers are found, create a default lp printer",
                  expected == printerList);
    }


    // Helper class to remove file when we leave scope
    class ScopedFileDeleter {
    private:
        QString m_path;
    public:
        ScopedFileDeleter(const QString& path) : m_path(path) {}
        ~ScopedFileDeleter() { std::remove(m_path.toUtf8().constData()); }
    };

    void askSystemForAvailablePrinters_0() {

        // this shouyld create a file filled with all of the available printers
        PrintingSystem::instance().askSystemForAvailablePrinters();

        // Even if the assertion fails, we still want to attempt removing the file
        // that was created. This is necessary if we fail to open it
        // because of permission problems.
        ScopedFileDeleter janitor(".__temp_printers.txt");

        QFile file(".__temp_printers.txt");
        ASSERTMSG("askSystemForAvailablePrinters() must create a file",
                  file.open(QIODevice::ReadOnly| QIODevice::Text));


    }
};

CPPUNIT_TEST_SUITE_REGISTRATION(PrintingSystemTests);


} // end of namespace
