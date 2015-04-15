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
    GGate testCut("cut",
                  SpJs::Band("cut","xparam", "yparam", {{1, 2}, {2, 3}, {3, 4}}));

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

