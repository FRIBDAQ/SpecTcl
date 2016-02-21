#ifndef GSLICETEST_H
#define GSLICETEST_H

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/Asserter.h>


#define private public
#define protected public
#include "GSlice.h"
#undef protected 
#undef private

#include "GlobalSettings.h"

#include <GateInfo.h>
#include <iostream>
#include <algorithm>

using namespace std;

namespace Viewer
{


class GSliceTest : public CppUnit::TestFixture
{
private:
  GSlice slice0;
  GSlice slice1;

  public:
    CPPUNIT_TEST_SUITE( GSliceTest );
    CPPUNIT_TEST( construct_0 );
    CPPUNIT_TEST( equality_0 );
    CPPUNIT_TEST( inequality_0 );
    CPPUNIT_TEST( inequality_1 );
    CPPUNIT_TEST( inequality_2 );
    CPPUNIT_TEST( inequality_3 );
    CPPUNIT_TEST_SUITE_END();

  public:
    void setUp() {
      slice0 = GSlice(SpJs::Slice("test", "xparam", 0, 1));
      slice1 = GSlice(SpJs::Slice("test", "xparam", 0, 1));
    }
    void tearDown() {
    }

  protected:
    void construct_0();
    void equality_0();
    void inequality_0();
    void inequality_1();
    void inequality_2();
    void inequality_3();
};

CPPUNIT_TEST_SUITE_REGISTRATION(GSliceTest);

#endif

// Test that we can synchronize to the SpecTcl data
void GSliceTest::construct_0()
{

  CPPUNIT_ASSERT(QString("test") == slice0.getName());
  CPPUNIT_ASSERT(QString("xparam") == slice0.getParameter());
  CPPUNIT_ASSERT(0.0 == slice0.getXLow());
  CPPUNIT_ASSERT(1.0 == slice0.getXHigh());

}

void GSliceTest::equality_0()
{
  CPPUNIT_ASSERT( slice0 == slice0 );

}

void GSliceTest::inequality_0()
{
  slice1 = GSlice(SpJs::Slice("test1", "xparam", 0, 1));
  CPPUNIT_ASSERT_MESSAGE( "DIfferent names is inequal", slice0 != slice1 );

}

void GSliceTest::inequality_1()
{
  slice1 = GSlice(SpJs::Slice("test1", "xparam1", 0, 1));
  CPPUNIT_ASSERT_MESSAGE( "Different parameter is not equal", slice0 != slice1 );

}

void GSliceTest::inequality_2()
{
  slice1 = GSlice(SpJs::Slice("test1", "xparam", 1, 1));
  CPPUNIT_ASSERT_MESSAGE( "Differetn low bound is not equal", slice0 != slice1 );

}

void GSliceTest::inequality_3()
{
  slice1 = GSlice(SpJs::Slice("test1", "xparam", 0, 3));
  CPPUNIT_ASSERT_MESSAGE( "Different upper bound is not equal", slice0 != slice1 );

}



} // end of namespace
