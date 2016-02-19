
#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/Asserter.h>

#include <TH1.h>
#include <json/json.h>
#include <fstream>
#include <string>
#include <iostream>
#include <sstream>
#include <utility>
#include <memory>
#include "BinInfo.h"

#include "config.h"

#define private public
#define protected public
#include "HistFiller.h"
#undef protected
#undef private

using namespace std;

class HistFillerTest : public CppUnit::TestFixture {
  public:
  CPPUNIT_TEST_SUITE(HistFillerTest);
  CPPUNIT_TEST( fillTH1_0 );
  CPPUNIT_TEST_SUITE_END();

public:
  void setUp() {
  }
  void tearDown() {
  }
protected:
  void fillTH1_0();
};

CPPUNIT_TEST_SUITE_REGISTRATION(HistFillerTest);

 
// this is a behavioral test. It merely record the default functionality that
// has been tested to work.
void HistFillerTest::fillTH1_0()
{
  unique_ptr<TH1> pHist(new TH1F("test","test",10,0,10));
  vector<SpJs::BinInfo> contents = {{1,0,0,1},{2,0,0,4.},{3,0,0,9.0},{5,0,0,2.}};

  SpJs::HistFiller fill;
  fill(*pHist,contents);

  CPPUNIT_ASSERT_EQUAL(1.0, pHist->GetBinContent(1));
  CPPUNIT_ASSERT_EQUAL(4.0, pHist->GetBinContent(2));
  CPPUNIT_ASSERT_EQUAL(9.0, pHist->GetBinContent(3));
  CPPUNIT_ASSERT_EQUAL(2.0, pHist->GetBinContent(5));
}

