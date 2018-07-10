// Template for a test suite.

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/Asserter.h>
#include <Asserts.h>



class FaileEvpTests : public CppUnit::TestFixture {
  CPPUNIT_TEST_SUITE(FaileEvpTests);
  CPPUNIT_TEST(aTest);
  CPPUNIT_TEST_SUITE_END();


private:

public:
  void setUp() {
  }
  void tearDown() {
  }
protected:
  void aTest();
};

CPPUNIT_TEST_SUITE_REGISTRATION(FaileEvpTests);

void FaileEvpTests::aTest() {
}
