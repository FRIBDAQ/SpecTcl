
#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/Asserter.h>

#include <TH1.h>
#include <json/json.h>
#include <fstream>
#include <string>

#define private public
#define protected public
#include "HistFactory.h"
#undef protected
#undef private

using namespace std;

class HistFactoryTest : public CppUnit::TestFixture {
  public:
  CPPUNIT_TEST_SUITE(HistFactoryTest);
  CPPUNIT_TEST( createTH1_0 );
  CPPUNIT_TEST( createTH2_0 );
  CPPUNIT_TEST_SUITE_END();

public:
  void setUp() {
  }
  void tearDown() {
  }
protected:
  void createTH1_0();
  void createTH2_0();
};

CPPUNIT_TEST_SUITE_REGISTRATION(HistFactoryTest);

 
// this is a behavioral test. It merely record the default functionality that
// has been tested to work.
void HistFactoryTest::createTH1_0()
{
  std::ifstream file("json/list1d.json");
  Json::Value value;
  file >> value;

  auto pHist = SpJs::HistFactory().create(value);

  CPPUNIT_ASSERT(pHist);
  CPPUNIT_ASSERT_EQUAL(string("raw00"),string(pHist->GetTitle()));
  auto pX = pHist->GetXaxis();
  CPPUNIT_ASSERT_EQUAL(1.0,   pX->GetXmin());
  CPPUNIT_ASSERT_EQUAL(100.0, pX->GetXmax());
  CPPUNIT_ASSERT_EQUAL(100,   pX->GetNbins());
}

// has been tested to work.
void HistFactoryTest::createTH2_0()
{
  std::ifstream file("json/list2d.json");
  Json::Value value;
  file >> value;

  auto pHist = SpJs::HistFactory().create(value);

  CPPUNIT_ASSERT(pHist);
  CPPUNIT_ASSERT_EQUAL(string("raw00vsraw01"),string(pHist->GetTitle()));
  auto pX = pHist->GetXaxis();
  CPPUNIT_ASSERT_EQUAL(1.0,   pX->GetXmin());
  CPPUNIT_ASSERT_EQUAL(100.0, pX->GetXmax());
  CPPUNIT_ASSERT_EQUAL(100,   pX->GetNbins());
  auto pY = pHist->GetYaxis();
  CPPUNIT_ASSERT_EQUAL(0.0,   pY->GetXmin());
  CPPUNIT_ASSERT_EQUAL(200.0, pY->GetXmax());
  CPPUNIT_ASSERT_EQUAL(200,   pY->GetNbins());
}
