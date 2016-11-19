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


#ifndef GATEEDITREQUESTTEST_H
#define GATEEDITREQUESTTEST_H

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/Asserter.h>


#include "GateEditRequest.h"
#include "GlobalSettings.h"
#include "GSlice.h"
#include "GGate.h"

#include <QString>

#include <iostream>

using namespace std;

namespace Viewer
{

class GateEditRequestTest : public CppUnit::TestFixture
{
  public:
    CPPUNIT_TEST_SUITE( GateEditRequestTest );
    CPPUNIT_TEST( fromGGate_0 );
    CPPUNIT_TEST( fromGSlice_0 );
    CPPUNIT_TEST_SUITE_END();

  public:
    void setUp() {
    }
    void tearDown() {
    }

  protected:
    void fromGGate_0();
    void fromGSlice_0();

};

CPPUNIT_TEST_SUITE_REGISTRATION(GateEditRequestTest);

#endif

void GateEditRequestTest::fromGGate_0()
{
    // test construction for a band
    GGate testCut(SpJs::Band("cut","xparam", "yparam", {{1, 2}, {2, 3}, {3, 4}}));

    GlobalSettings::setServerHost("localhost");
    GlobalSettings::setServerPort(1);

    GateEditRequest req(testCut);

    QString exp("http://localhost:1/spectcl/gate/edit?name=cut");
    exp += "&type=b";
    exp += "&xparameter=xparam&yparameter=yparam";
    exp += "&xcoord(0)=1&ycoord(0)=2";
    exp += "&xcoord(1)=2&ycoord(1)=3";
    exp += "&xcoord(2)=3&ycoord(2)=4";

    CPPUNIT_ASSERT(QUrl(exp) == req.toUrl());
}

void GateEditRequestTest::fromGSlice_0()
{
    // test construction for a band
    GSlice testCut(nullptr , "cut", "param", 1, 20);


    GlobalSettings::setServerHost("localhost");
    GlobalSettings::setServerPort(1);

    GateEditRequest req(testCut);

    QString exp("http://localhost:1/spectcl/gate/edit?name=cut");
    exp += "&type=s";
    exp += "&low=1";
    exp += "&high=20";
    exp += "&parameter=param";

//    cout << "exp : " << exp.toStdString() << endl;
//    cout << "act : " << req.toUrl().toString().toStdString() << endl;
    CPPUNIT_ASSERT(QUrl(exp) == req.toUrl());
}

} // end of namespcae
