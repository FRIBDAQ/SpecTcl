#ifndef GATEEDITREQUESTTEST_H
#define GATEEDITREQUESTTEST_H

#include "GateEditRequest.h"
#include "GlobalSettings.h"
#include "GSlice.h"

#include <QtTest/QtTest>
#include <QString>
#include <QObject>

#include <TCutG.h>

class GateEditRequestTest : public QObject
{
    Q_OBJECT
public:
    explicit GateEditRequestTest(QObject *parent = 0);


public slots:
    void fromTCutG_0();
    void fromGSlice_0();

};

#endif // GATEEDITREQUESTTEST_H

GateEditRequestTest::GateEditRequestTest(QObject *parent) :
    QObject(parent)
{
}

void GateEditRequestTest::fromTCutG_0()
{
    // test construction for a band
    TCutG testCut("cut", 3);
    testCut.SetPoint(0, 1, 2);
    testCut.SetPoint(1, 2, 3);
    testCut.SetPoint(2, 3, 4);

    GateEditRequest req(testCut);
    GlobalSettings::setServerHost("localhost");
    GlobalSettings::setServerPort(1);

    QString exp("http:localhost:1/spectcl/gate/edit?name=cut");
    exp += "&type=b";
    exp += "&xparameter=xparam&yparameter=yparam";
    exp += "&xcoords={1 2 3}&ycoords={2 3 4}";

    QCOMPARE(QUrl(exp), req.toUrl());
}

void GateEditRequestTest::fromGSlice_0()
{
    // test construction for a band
    GSlice testCut(this, "cut", "param", 1, 20);

    GateEditRequest req(testCut);

    GlobalSettings::setServerHost("localhost");
    GlobalSettings::setServerPort(1);

    QString exp("http:localhost:1/spectcl/gate/edit?name=cut");
    exp += "&type=s";
    exp += "&parameter=param";
    exp += "&low=1";
    exp += "&high=20";

    QCOMPARE(QUrl(exp), req.toUrl());
}

QTEST_MAIN(GateEditRequestTest)
#include "GateEditRequestTest.moc"
