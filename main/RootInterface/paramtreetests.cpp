// Template for a test suite.

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/Asserter.h>



class Testname : public CppUnit::TestFixture {
  CPPUNIT_TEST_SUITE(Testname);
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

CPPUNIT_TEST_SUITE_REGISTRATION(Testname);

void Testname::aTest() {
}
