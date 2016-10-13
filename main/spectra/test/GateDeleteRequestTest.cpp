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


#ifndef GATEDELETEREQUESTTEST_H
#define GATEDELETEREQUESTTEST_H

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/Asserter.h>


#include "GateDeleteRequest.h"
#include "GlobalSettings.h"
#include "GSlice.h"
#include "GGate.h"

#include <QString>

#include <iostream>

using namespace std;

namespace Viewer
{

class GateDeleteRequestTest : public CppUnit::TestFixture
{
  public:
    CPPUNIT_TEST_SUITE( GateDeleteRequestTest );
    CPPUNIT_TEST( fromGGate_0 );
    CPPUNIT_TEST( fromGSlice_0 );
    CPPUNIT_TEST( fromName_0 );
    CPPUNIT_TEST_SUITE_END();

  public:
    void setUp() {
    }
    void tearDown() {
    }

  protected:
    void fromGGate_0();
    void fromGSlice_0();
    void fromName_0();

};

CPPUNIT_TEST_SUITE_REGISTRATION(GateDeleteRequestTest);

#endif

void GateDeleteRequestTest::fromGGate_0()
{
    // test construction for a band
    GGate testCut(SpJs::Band("cut","xparam", "yparam", {{1, 2}, {2, 3}, {3, 4}}));

    GlobalSettings::setServerHost("localhost");
    GlobalSettings::setServerPort(1);

    GateDeleteRequest req(testCut);

    QString exp("http://localhost:1/spectcl/gate/delete?name=cut");

    CPPUNIT_ASSERT(QUrl(exp) == req.toUrl());
}

void GateDeleteRequestTest::fromGSlice_0()
{
    // test construction for a band
    GSlice testCut(nullptr , "cut", "param", 1, 20);


    GlobalSettings::setServerHost("localhost");
    GlobalSettings::setServerPort(1);

    GateDeleteRequest req(testCut);

    QString exp("http://localhost:1/spectcl/gate/delete?name=cut");

    CPPUNIT_ASSERT(QUrl(exp) == req.toUrl());
}

void GateDeleteRequestTest::fromName_0()
{
    // test construction for a band
    GlobalSettings::setServerHost("localhost");
    GlobalSettings::setServerPort(1);

    GateDeleteRequest req("testCut");

    QString exp("http://localhost:1/spectcl/gate/delete?name=testCut");

    CPPUNIT_ASSERT(QUrl(exp) == req.toUrl());
}

} // end of namespace
