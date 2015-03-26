
#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/Asserter.h>

#include <TH1.h>
#include <json/json.h>
#include <fstream>
#include <memory>
#include <string>
#include <sstream>
#include <algorithm>
#include "GateInfo.h"

#include "config.h"

#define private public
#define protected public
#include "GateCmdParser.h"
#undef protected
#undef private

using namespace std;

class GateCmdParserTest : public CppUnit::TestFixture {
  public:
  CPPUNIT_TEST_SUITE(GateCmdParserTest);
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

CPPUNIT_TEST_SUITE_REGISTRATION(GateCmdParserTest);

void GateCmdParserTest::parseList_0()
{
    using SpJs::Slice;

    std::stringstream ss;
    ss << JSON_TEST_DIR << "/gate1d.json";
    std::ifstream file(ss.str().c_str());

    Json::Value value;
    file >> value;
    file.close();

    auto res = SpJs::GateCmdParser().parseList(value);
    CPPUNIT_ASSERT_EQUAL(size_t(1),res.size());

    Slice act;
    CPPUNIT_ASSERT_NO_THROW( act = dynamic_cast<Slice&>(*(res.at(0).get())) );

    Slice exp("test", "event.raw.00", 0.0, 10.0);

    CPPUNIT_ASSERT(exp == act);

}
