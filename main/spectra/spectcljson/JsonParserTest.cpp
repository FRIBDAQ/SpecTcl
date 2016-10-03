
#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/Asserter.h>

#include <TH1.h>
#include <json/json.h>
#include <fstream>
#include <string>
#include <sstream>
#include <iostream>
#include <algorithm>
#include <iterator>
#include "HistInfo.h"
#include "BinInfo.h"
#include "ParameterInfo.h"

#include "config.h"
#include "Asserts.h"

#define private public
#define protected public
#include "JsonParser.h"
#undef protected
#undef private

using namespace std;


ostream& operator<<(ostream& str, const SpJs::BinInfo& info) {
    str << "( "
        << info.s_xbin << ", "
        << info.s_ybin << ", "
        << info.s_zbin << ", "
        << info.s_value << ")";
    return str;
}


class JsonParserTest : public CppUnit::TestFixture {
  public:
  CPPUNIT_TEST_SUITE(JsonParserTest);
  CPPUNIT_TEST( parseListCmd_0 );
  CPPUNIT_TEST( parseListDetail_0 );
  CPPUNIT_TEST( parseContentCmd_0 );
  CPPUNIT_TEST( parseContentCmd_1 );
  CPPUNIT_TEST( parseSummary_0 );
  CPPUNIT_TEST( parseBitmask_0 );
  CPPUNIT_TEST( parseGamma1_0 );
  CPPUNIT_TEST( parseGamma2_0 );
  CPPUNIT_TEST( json_0 );
  CPPUNIT_TEST_SUITE_END();

public:
  void setUp() {
  }
  void tearDown() {
  }
protected:

  void parseListDetail_0();
  void parseListCmd_0();
  void parseContentCmd_0();
  void parseContentCmd_1();
  void parseSummary_0();
  void parseBitmask_0();
  void parseGamma1_0();
  void parseGamma2_0();
  void json_0();
  void parseParameterCmd_0();
};

CPPUNIT_TEST_SUITE_REGISTRATION(JsonParserTest);


void JsonParserTest::parseListCmd_0() 
{
  stringstream ss;
  ss << JSON_TEST_DIR << "/list1d.json";

  Json::Value value;
  std::ifstream file (ss.str().c_str());
  file >> value;

  std::vector<SpJs::HistInfo> parsedResult = SpJs::JsonParser().parseListCmd(value);

  SpJs::HistInfo expected;
  expected.s_name = "raw00";
  expected.s_type = "1";
  expected.s_params = {"event.raw.00"};
  expected.s_axes = {{1.0, 100.0, 100}};
  expected.s_chanType = SpJs::Long;

  CPPUNIT_ASSERT(1 == parsedResult.size());
  CPPUNIT_ASSERT_EQUAL( expected, parsedResult[0] );

}
 

void JsonParserTest::parseListDetail_0() 
{
  stringstream ss;
  ss << JSON_TEST_DIR << "/list1d_detail.json";

  Json::Value value;
  std::ifstream file (ss.str().c_str());
  file >> value;

  SpJs::HistInfo parsedResult = SpJs::JsonParser().parseListDetail(value);

  SpJs::HistInfo expected;
  expected.s_name = "raw00";
  expected.s_type = "1";
  expected.s_params = {"event.raw.00"};
  expected.s_axes = {{1.0, 100.0, 100}};
  expected.s_chanType = SpJs::Long;

  CPPUNIT_ASSERT_EQUAL( expected, parsedResult );

}
 

void JsonParserTest::parseContentCmd_0 () 
{
  stringstream ss;
  ss << JSON_TEST_DIR << "/content1d.json";

  Json::Value value;
  std::ifstream file (ss.str().c_str());

  file >> value;
  file.close();

  auto parsedResult = SpJs::JsonParser().parseContentCmd(value);

  vector<SpJs::BinInfo> expected = {{0,0,0,1.},
                                        {1,0,0,2.},
                                        {2,0,0,2.},
                                        {3,0,0,4.},
                                        {4,0,0,2.},
                                        {6,0,0,2.},
                                        {7,0,0,1.},
                                        {8,0,0,2.}};


  auto& values = parsedResult.getValues();

  for (int i=0; i<min(expected.size(), values.size()); ++i) {
      std::string msg("Parsed result should have same value for index ");
      msg += to_string(i);
      EQMSG( msg.c_str(), expected[i].s_xbin,  values[i].s_xbin);
      EQMSG( msg.c_str(), expected[i].s_value,  values[i].s_value);
  }

  CPPUNIT_ASSERT( 4        == parsedResult.getUnderflow(0) );
  CPPUNIT_ASSERT( 1156269  == parsedResult.getOverflow(0) );

}

void JsonParserTest::parseContentCmd_1()
{

    stringstream ss;
    ss << JSON_TEST_DIR << "/content2d.json";

    Json::Value value;
    std::ifstream file (ss.str().c_str());
    file >> value;
    file.close();

    auto parsedResult = SpJs::JsonParser().parseContentCmd(value);

    vector<SpJs::BinInfo> expected = {{109, 6,  0, 1.},
                                      {119, 14, 0, 2.},
                                      {140, 24, 0, 3.},
                                      {134, 25, 0, 4.},
                                      {166, 46, 0, 5.}};


    CPPUNIT_ASSERT( expected == parsedResult.getValues() );
    CPPUNIT_ASSERT( 1 == parsedResult.getUnderflow(0) );
    CPPUNIT_ASSERT( 3 == parsedResult.getOverflow(0) );
    CPPUNIT_ASSERT( 2 == parsedResult.getUnderflow(1) );
    CPPUNIT_ASSERT( 4 == parsedResult.getOverflow(1) );
}

void JsonParserTest::parseSummary_0()
{
    stringstream ss;
    ss << JSON_TEST_DIR << "/summary.json";

    Json::Value value;
    std::ifstream file (ss.str().c_str());
    file >> value;
    file.close();

//    std::cout << value << std::endl;

    std::vector<SpJs::HistInfo> parsedResult = SpJs::JsonParser().parseListCmd(value);

    SpJs::HistInfo expected;
    expected.s_name = "event.raw.00";
    expected.s_type = "s";
    expected.s_params = {"event.raw.00","event.raw.01","event.raw.02","event.raw.03"};
    expected.s_axes = {{1.0, 100.0, 100}};
    expected.s_chanType = SpJs::Long;

    CPPUNIT_ASSERT_EQUAL( expected, parsedResult.at(0) );
}

void JsonParserTest::parseBitmask_0()
{
    stringstream ss;
    ss << JSON_TEST_DIR << "/list_bitmask.json";

    Json::Value value;
    std::ifstream file (ss.str().c_str());
    file >> value;
    file.close();

//    std::cout << value << std::endl;

    std::vector<SpJs::HistInfo> parsedResult = SpJs::JsonParser().parseListCmd(value);

    SpJs::HistInfo expected;
    expected.s_name = "raw00_bit";
    expected.s_type = "b";
    expected.s_params = {"event.raw.00"};
    expected.s_axes = {{1.0, 100.0, 99}};
    expected.s_chanType = SpJs::Long;

    CPPUNIT_ASSERT_EQUAL( expected, parsedResult.at(0) );
}

void JsonParserTest::parseGamma1_0()
{
    stringstream ss;
    ss << JSON_TEST_DIR << "/list_gamma1.json";

    Json::Value value;
    std::ifstream file (ss.str().c_str());
    file >> value;
    file.close();

//    std::cout << value << std::endl;

    std::vector<SpJs::HistInfo> parsedResult = SpJs::JsonParser().parseListCmd(value);

    SpJs::HistInfo expected;
    expected.s_name = "raw00_g1";
    expected.s_type = "g1";
    expected.s_params = {"event.raw.00","event.raw.01"};
    expected.s_axes = {{1.0, 100.0, 100}};
    expected.s_chanType = SpJs::Long;

    CPPUNIT_ASSERT_EQUAL( expected, parsedResult.at(0) );
}

void JsonParserTest::parseGamma2_0()
{
    stringstream ss;
    ss << JSON_TEST_DIR << "/list_gamma2.json";

    Json::Value value;
    std::ifstream file (ss.str().c_str());
    file >> value;
    file.close();

//    std::cout << value << std::endl;

    std::vector<SpJs::HistInfo> parsedResult = SpJs::JsonParser().parseListCmd(value);

    SpJs::HistInfo expected;
    expected.s_name = "raw00_g2";
    expected.s_type = "g2";
    expected.s_params = {"event.raw.00","event.raw.01"};
    expected.s_axes = {{1.0, 100.0, 100},{1.0,100.0,100}};
    expected.s_chanType = SpJs::Long;

    CPPUNIT_ASSERT_EQUAL( expected, parsedResult.at(0) );
}

void JsonParserTest::json_0 () 
{

    std::string jsonStr(
    "{\n"
    "  \"x\" : 34\n"
    "}"
                );

  Json::Value value;
  Json::Reader reader;
  reader.parse(jsonStr,value);

  CPPUNIT_ASSERT_NO_THROW(value["x"]);
}

