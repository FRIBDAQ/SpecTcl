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
    CPPUNIT_TEST(constructFromslice);
    CPPUNIT_TEST(constructFromGammaslice);
    CPPUNIT_TEST(inequality_4);
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
    
    void constructFromslice();
    void constructFromGammaslice();
    void inequality_4();
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

void GSliceTest::constructFromslice()
{
  slice1 = GSlice(SpJs::Slice("slice", "parameter", 0, 1));
  
  CPPUNIT_ASSERT_EQUAL(size_t(1), slice1.parameterCount());
  CPPUNIT_ASSERT_EQUAL(std::string("parameter"), slice1.getParameter().toStdString());
}

void GSliceTest::constructFromGammaslice()
{
  slice1 = GSlice(SpJs::GammaSlice("slice",
        {std::string("param1"), std::string("param2"), std::string("param3")}, 0, 1));
  CPPUNIT_ASSERT_EQUAL(size_t(3), slice1.parameterCount());
  CPPUNIT_ASSERT_EQUAL(std::string("param1"), slice1.getParameter(0).toStdString());
  CPPUNIT_ASSERT_EQUAL(std::string("param2"), slice1.getParameter(1).toStdString());
  CPPUNIT_ASSERT_EQUAL(std::string("param3"), slice1.getParameter(2).toStdString());
}

void GSliceTest::inequality_4()
{
  slice1 = GSlice(SpJs::GammaSlice("slice",
          {std::string("xparam"), std::string("param2"), std::string("param3")}, 0, 1));
  CPPUNIT_ASSERT_MESSAGE("gamma slice not equal slice - extra params", slice1 != slice0);
  
}

} // end of namespace
