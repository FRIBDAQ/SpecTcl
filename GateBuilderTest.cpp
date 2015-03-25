
#include "GateBuilder.h"
#include <QObject>
#include <QtTest/QtTest>
#include <TCanvas.h>
#include <utility>

class GateBuilderTest : public QObject
{

    Q_OBJECT

private slots:
    void initTestCase() {}
    void cleanupTestCase() {}
    void init() {}
    void cleanup() {}

    void newPoint_0() {
//        GateBuilder builder;
//        TCanvas canvas("testcan");

//        QObject::connect(&canvas, SIGNAL(PadDoubleClicked(TPad*)),&builder,SLOT(newPoint(TPad*)));

//        canvas.HandleInput(kButton1Down,100,100);
//        canvas.HandleInput(kButton1Down,100,100);


//        QCOMPARE(1,builder.getNPoints());
//        QCOMPARE(std::make_pair<int,int>(100,100),builder.getPoints().at(0))

    }

};

QTEST_MAIN(GateBuilderTest)
#include "GateBuilderTest.moc"
