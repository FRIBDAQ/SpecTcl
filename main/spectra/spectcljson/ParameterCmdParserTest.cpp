
#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/Asserter.h>

#include <TH1.h>
#include <json/json.h>
#include <fstream>
#include <string>
#include <sstream>
#include <algorithm>
#include "HistInfo.h"
#include "BinInfo.h"
#include "ParameterInfo.h"

#include "config.h"

#define private public
#define protected public
#include "ParameterCmdParser.h"
#undef protected
#undef private

using namespace std;

class ParameterCmdParserTest : public CppUnit::TestFixture {
  public:
  CPPUNIT_TEST_SUITE(ParameterCmdParserTest);
  CPPUNIT_TEST( parseList_0 );
  CPPUNIT_TEST_SUITE_END();

public:
  void setUp() {
  }
  void tearDown() {
  }
protected:

  void parseList_0();
};

CPPUNIT_TEST_SUITE_REGISTRATION(ParameterCmdParserTest);


void ParameterCmdParserTest::parseList_0()
{
    std::stringstream ss;
    ss << JSON_TEST_DIR << "/paramList.json";
    std::ifstream file(ss.str().c_str());

    Json::Value value;
    file >> value;
    file.close();

    auto res = SpJs::ParameterCmdParser().parseList(value);
    CPPUNIT_ASSERT_EQUAL(size_t(2),res.size());

    vector<SpJs::ParameterInfo> expected = {{"event.raw.00", 0, 100, 1, 100, "channels"},
                                      {"event.raw.01", 1, 100, 1, 100, "channels"}};
    CPPUNIT_ASSERT(expected == res);
}

