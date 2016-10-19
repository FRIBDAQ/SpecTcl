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


#ifndef MAINTEST_H
#define MAINTEST_H

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/Asserter.h>

#include <Main.h>

#include <QList>
#include <QHostAddress>

using namespace std;


namespace Viewer
{

class MainTest : public CppUnit::TestFixture
{

  public:
    CPPUNIT_TEST_SUITE( MainTest );
    CPPUNIT_TEST( listsShareContent_0 );
    CPPUNIT_TEST( listsShareContent_1 );
    CPPUNIT_TEST_SUITE_END();

  public:
    void setUp() {
    }
    void tearDown() {
    }

  protected:
    void listsShareContent_0() {

        Main m;

        QList<QHostAddress> addressList0;
        QList<QHostAddress> addressList1;

        addressList0.push_back(QHostAddress(0xff00ff00));
        addressList0.push_back(QHostAddress(0x00ff00ff));
        addressList1.push_back(QHostAddress(0x00ff00ff));
        addressList1.push_back(QHostAddress(0x80000001));

        bool overlapDetected = m.listsShareContent(addressList0, addressList1);
        CPPUNIT_ASSERT_EQUAL_MESSAGE("Two lists with one common element",
                                     true, overlapDetected);

    }

    void listsShareContent_1() {

        Main m;

        QList<QHostAddress> addressList0;
        QList<QHostAddress> addressList1;

        addressList0.push_back(QHostAddress(0xff00ff00));
        addressList1.push_back(QHostAddress(0x00ff00ff));
        addressList1.push_back(QHostAddress(0x80000001));

        bool overlapDetected = m.listsShareContent(addressList0, addressList1);
        CPPUNIT_ASSERT_EQUAL_MESSAGE("Two lists with no common element",
                                     false, overlapDetected);

    }

};

CPPUNIT_TEST_SUITE_REGISTRATION(MainTest);

#endif

} // end of namespace

