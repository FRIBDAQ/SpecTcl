
#ifndef QTLINETEST_H
#define QTLINETEST_H

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/Asserter.h>


#define private public
#define protected public
#include "QTLine.h"
#undef protected
#undef private


#include <iostream>
#include <algorithm>

using namespace std;

namespace Viewer
{

class QTLineTest : public CppUnit::TestFixture
{
private:
    QTLine m_l1;
    QTLine m_l2;

  public:
    CPPUNIT_TEST_SUITE( QTLineTest );
    CPPUNIT_TEST( construct_0 );
    CPPUNIT_TEST( assign_0 );
    CPPUNIT_TEST( assign_1 );
    CPPUNIT_TEST( assign_2 );
    CPPUNIT_TEST( assign_3 );
    CPPUNIT_TEST_SUITE_END();

  public:
    void setUp() {
        m_l1 = QTLine(0, 1, 2, 3);
        m_l2 = QTLine(1, 2, 3, 4);
    }
    void tearDown() {
    }

  protected:
    void construct_0();
    void assign_0();
    void assign_1();
    void assign_2();
    void assign_3();

};

CPPUNIT_TEST_SUITE_REGISTRATION(QTLineTest);

#endif

void QTLineTest::construct_0()
{
    CPPUNIT_ASSERT_EQUAL_MESSAGE("Construct x value of first point",
                                 0., m_l1.GetX1());
    CPPUNIT_ASSERT_EQUAL_MESSAGE("Construct y value of first point",
                                 1., m_l1.GetY1());
    CPPUNIT_ASSERT_EQUAL_MESSAGE("Construct x value of second point",
                                 2., m_l1.GetX2());
    CPPUNIT_ASSERT_EQUAL_MESSAGE("Construct y value of second point",
                                 3., m_l1.GetY2());
}

void QTLineTest::assign_0()
{
    CPPUNIT_ASSERT_EQUAL(0., m_l1.GetX1());
  m_l1 = m_l2;
  CPPUNIT_ASSERT_EQUAL_MESSAGE("Assignment of x value of first point",
                               1., m_l1.GetX1());
}
void QTLineTest::assign_1()
{
    CPPUNIT_ASSERT_EQUAL(1., m_l1.GetY1());
  m_l1 = m_l2;
  CPPUNIT_ASSERT_EQUAL_MESSAGE("Assignment of y value of first point",
                               2., m_l1.GetY1());
}
void QTLineTest::assign_2()
{
    CPPUNIT_ASSERT_EQUAL(2., m_l1.GetX2());
  m_l1 = m_l2;
  CPPUNIT_ASSERT_EQUAL_MESSAGE("Assignment of x value of second point",
                               3., m_l1.GetX2());
}
void QTLineTest::assign_3()
{
  m_l1 = m_l2;
  CPPUNIT_ASSERT_EQUAL_MESSAGE("Assignment of y value of second point",
                               4., m_l1.GetY2());
}

} // end of namespace
