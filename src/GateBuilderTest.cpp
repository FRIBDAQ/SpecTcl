//    This software is Copyright by the Board of Trustees of Michigan
//    State University (c) Copyright 2015.
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

static const char* Copyright = "(C) Copyright Michigan State University 2015, All rights reserved";

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
