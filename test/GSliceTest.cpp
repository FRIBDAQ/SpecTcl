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

class GSliceTest : public CppUnit::TestFixture
{
  public:
    CPPUNIT_TEST_SUITE( GSliceTest );
    CPPUNIT_TEST( construct_0 );
    CPPUNIT_TEST_SUITE_END();

  public:
    void setUp() {
    }
    void tearDown() {
    }

  protected:
    void construct_0();

};

CPPUNIT_TEST_SUITE_REGISTRATION(GSliceTest);

#endif

// Test that we can synchronize to the SpecTcl data
void GSliceTest::construct_0()
{

  SpJs::Slice slice("test", "xparam", 0, 1);

  GSlice gsl(slice);

  CPPUNIT_ASSERT(QString("test") == gsl.getName());
  CPPUNIT_ASSERT(QString("xparam") == gsl.getParameter());
  CPPUNIT_ASSERT(0.0 == gsl.getXLow());
  CPPUNIT_ASSERT(1.0 == gsl.getXHigh());

}

