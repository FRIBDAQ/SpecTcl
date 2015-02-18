
#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/Asserter.h>

#include <TH1.h>
#include <json/json.h>
#include <fstream>
#include <string>
#include <algorithm>

#define private public
#define protected public
#include "HistInfo.h"
#undef protected
#undef private

using namespace std;

class HistInfoTest : public CppUnit::TestFixture {
  public:
  CPPUNIT_TEST_SUITE(HistInfoTest);
  CPPUNIT_TEST( copy_0 );
  CPPUNIT_TEST( compareEqual_0 );
  CPPUNIT_TEST_SUITE_END();

public:
  void setUp() {
  }
  void tearDown() {
  }
protected:

  void copy_0();
  void compareEqual_0();
};

CPPUNIT_TEST_SUITE_REGISTRATION(HistInfoTest);


void HistInfoTest::copy_0() 
{
  SpJs::HistInfo info;
  info.s_name = "asdf";
  info.s_type = 1;
  info.s_params = {"param0","param1"};
  info.s_axes = {{0.0,100.0,100},{200.0,300.0,100}};
  info.s_chanType = SpJs::Short;

  SpJs::HistInfo info1(info);

  // Make sure they are the same
  CPPUNIT_ASSERT_EQUAL(info.s_name,     info1.s_name);
  CPPUNIT_ASSERT_EQUAL(info.s_type,     info1.s_type);
  CPPUNIT_ASSERT_EQUAL(info.s_params.at(0), info1.s_params.at(0));
  CPPUNIT_ASSERT_EQUAL(info.s_params.at(1), info1.s_params.at(1));
  CPPUNIT_ASSERT_EQUAL(size_t(2), info.s_axes.size());
  CPPUNIT_ASSERT_EQUAL(info.s_axes.size(), info1.s_axes.size());
  CPPUNIT_ASSERT_EQUAL(info.s_axes.at(0).s_low, info1.s_axes.at(0).s_low);
  CPPUNIT_ASSERT_EQUAL(info.s_axes.at(0).s_high, info1.s_axes.at(0).s_high);
  CPPUNIT_ASSERT_EQUAL(info.s_axes.at(0).s_nbins, info1.s_axes.at(0).s_nbins);
  CPPUNIT_ASSERT_EQUAL(info.s_axes.at(1).s_low, info1.s_axes.at(1).s_low);
  CPPUNIT_ASSERT_EQUAL(info.s_axes.at(1).s_high, info1.s_axes.at(1).s_high);
  CPPUNIT_ASSERT_EQUAL(info.s_axes.at(1).s_nbins, info1.s_axes.at(1).s_nbins);
  CPPUNIT_ASSERT_EQUAL(info.s_chanType, info1.s_chanType);

}
 

void HistInfoTest::compareEqual_0()
{
  SpJs::HistInfo info;
  info.s_name = "asdf";
  info.s_type = 1;
  info.s_params = {"param0","param1"};
  info.s_axes = {{0.0,100.0,100},{200.0,300.0,100}};
  info.s_chanType = SpJs::Short;

  // create an identical copy
  SpJs::HistInfo info1;
  info1.s_name = "asdf";
  info1.s_type = 1;
  info1.s_params = {"param0","param1"};
  info1.s_axes = {{0.0,100.0,100},{200.0,300.0,100}};
  info1.s_chanType = SpJs::Short;

  // test equal
  CPPUNIT_ASSERT(info == info1);
  CPPUNIT_ASSERT( !(info != info1) );

  // change one value and make sure it is not equal
  info.s_name = "hahaha";
  // assert that they are no longer equal
  CPPUNIT_ASSERT(info != info1);
  CPPUNIT_ASSERT( !(info == info1) );
};
