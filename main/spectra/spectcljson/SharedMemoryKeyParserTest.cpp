
#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/Asserter.h>

#include <json/json.h>
#include <fstream>
#include <string>

#include "config.h"

#include "SharedMemoryKeyParser.h"

using namespace std;

class SharedMemoryKeyParserTest : public CppUnit::TestFixture {
  public:

  CPPUNIT_TEST_SUITE(SharedMemoryKeyParserTest);
  CPPUNIT_TEST(parse_0);
  CPPUNIT_TEST(parseSize_0);
  CPPUNIT_TEST_SUITE_END();

public:
  void setUp() {
  }
  void tearDown() {
  }
protected:

  void parse_0 () {

      string jsonPath(JSON_TEST_DIR);
      jsonPath += "/shmemkey.json";

      ifstream file(jsonPath.c_str());

      Json::Value value;
      file >> value;
      file.close();

      string keyString = SpJs::SharedMemoryKeyParser().parseKey(value);
      CPPUNIT_ASSERT_EQUAL_MESSAGE(
                  "Parsing key response should extract correct key",
                  string("XAee"), keyString
                  );
  }

  void parseSize_0 () {

      string jsonPath(JSON_TEST_DIR);
      jsonPath += "/shmemsize.json";

      ifstream file(jsonPath.c_str());

      Json::Value value;
      file >> value;
      file.close();

      string sizeString = SpJs::SharedMemoryKeyParser().parseSize(value);
      CPPUNIT_ASSERT_EQUAL_MESSAGE(
                  "Parsing size response should extract correct size",
                  string("262144000"), sizeString
                  );
  }

};

CPPUNIT_TEST_SUITE_REGISTRATION(SharedMemoryKeyParserTest);

