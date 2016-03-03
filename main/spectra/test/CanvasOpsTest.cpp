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


#ifndef CANVASOPSTEST_H
#define CANVASOPSTEST_H

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/Asserter.h>
#include "Asserts.h"

#include "CanvasOps.h"
#include "QRootCanvas.h"

#include "TH1.h"
#include "TH2.h"
#include "TCanvas.h"

#include <QString>

#include <iostream>
#include <algorithm>

using namespace std;


ostream& operator<<(ostream& stream, const QString& msg) {
    stream << msg.toStdString() << endl;
    return stream;
}

namespace Viewer
{

class CanvasOpsTest : public CppUnit::TestFixture
{
  public:
    CPPUNIT_TEST_SUITE( CanvasOpsTest );
    CPPUNIT_TEST( extractAllHistNames_0 );
    CPPUNIT_TEST( extractAllHistNames_1 );
    CPPUNIT_TEST_SUITE_END();

  public:
    void setUp() {
    }
    void tearDown() {
    }

  protected:
    void extractAllHistNames_0()
    {
        QRootCanvas canvas(nullptr);

        TH1F h1f("h1f", "", 10, 0, 10);
        TH2D h2d("h2d", "", 10, 0, 10, 10, 0, 10);

        h1f.Draw();
        h2d.Draw("same");

        auto names = CanvasOps::extractAllHistNames(canvas);

        sort(begin(names), end(names));

        EQMSG("Two histograms should be found",
              size_t(2), names.size());

        EQMSG("1D histograms should be found",
              QString("h1f"), names[0]);

        EQMSG("2D histograms should be found",
              QString("h2d"), names[1]);

    }

    // test that we can recursively search subpads
    void extractAllHistNames_1()
    {
        QRootCanvas canvas(nullptr);
        canvas.getCanvas()->Divide(2,1);

        canvas.cd(1);
        TH1F h1f("h1f", "", 10, 0, 10);
        canvas.cd(2);
        TH2D h2d("h2d", "", 10, 0, 10, 10, 0, 10);

        h1f.Draw();
        h2d.Draw("same");

        auto names = CanvasOps::extractAllHistNames(canvas);

        sort(begin(names), end(names));

        EQMSG("Recursion into subpads finds proper number of hists",
              size_t(2), names.size());

        EQMSG("Recursive search finds 1D histograms",
              QString("h1f"), names[0]);

        EQMSG("Recursive search finds 2D histograms",
              QString("h2d"), names[1]);

    }

};

CPPUNIT_TEST_SUITE_REGISTRATION(CanvasOpsTest);

#endif


} // end of namespcae
