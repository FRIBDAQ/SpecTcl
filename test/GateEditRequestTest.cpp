#ifndef GATEEDITREQUESTTEST_H
#define GATEEDITREQUESTTEST_H

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/Asserter.h>


#include "GateEditRequest.h"
#include "GlobalSettings.h"
#include "GSlice.h"

#include <QString>

#include <TCutG.h>

#include <iostream>

using namespace std;

class GateEditRequestTest : public CppUnit::TestFixture
{
  public:
    CPPUNIT_TEST_SUITE( GateEditRequestTest );
    CPPUNIT_TEST( fromTCutG_0 );
    CPPUNIT_TEST( fromGSlice_0 );
    CPPUNIT_TEST_SUITE_END();

  public:
    void setUp() {
    }
    void tearDown() {
    }

  protected:
    void fromTCutG_0();
    void fromGSlice_0();

};

CPPUNIT_TEST_SUITE_REGISTRATION(GateEditRequestTest);

#endif

void GateEditRequestTest::fromTCutG_0()
{
    // test construction for a band
    TCutG testCut("cut", 3);
    testCut.SetPoint(0, 1, 2);
    testCut.SetPoint(1, 2, 3);
    testCut.SetPoint(2, 3, 4);

    GlobalSettings::setServerHost("localhost");
    GlobalSettings::setServerPort(1);

    GateEditRequest req(testCut);

    QString exp("http://localhost:1/spectcl/gate/edit?name=cut");
    exp += "&type=b";
    exp += "&xparameter=xparam&yparameter=yparam";
    exp += "&xcoords={1 2 3}&ycoords={2 3 4}";

    cout << "exp : " << exp.toStdString() << endl;
    cout << "act : " << req.toUrl().toString().toStdString() << endl;
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

    cout << "exp : " << exp.toStdString() << endl;
    cout << "act : " << req.toUrl().toString().toStdString() << endl;
    CPPUNIT_ASSERT(QUrl(exp) == req.toUrl());
}

