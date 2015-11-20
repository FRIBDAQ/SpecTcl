// Template for a test suite.
#include <config.h>
#include <Exception.h>
#include <string>

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/Asserter.h>
#include "Asserts.h"


using namespace std;

class testException : public CppUnit::TestFixture {
  CPPUNIT_TEST_SUITE(testException);
  CPPUNIT_TEST(baseTest);
  CPPUNIT_TEST_SUITE_END();


private:

public:
  void setUp() {
  }
  void tearDown() {
  }
protected:
  void baseTest();
};

CPPUNIT_TEST_SUITE_REGISTRATION(testException);

// Test the exception base class.
void testException::baseTest() {

  CException base("This is a test");
  EQ(-1, base.ReasonCode());
  EQ(string("Unspecified Exception"), string(base.ReasonText()));
  EQ(string("This is a test"), string(base.WasDoing()));


}
