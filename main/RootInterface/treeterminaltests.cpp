// Template for a test suite.

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/Asserter.h>

#include <Asserts.h>
#include <TreeBuilder.h>



class terminaltests : public CppUnit::TestFixture {
  CPPUNIT_TEST_SUITE(terminaltests);
  CPPUNIT_TEST(constructtest);
  CPPUNIT_TEST(notfolder);
  CPPUNIT_TEST_SUITE_END();


private:

public:
  void setUp() {
  }
  void tearDown() {
  }
protected:
  void constructtest();
  void notfolder();
};

CPPUNIT_TEST_SUITE_REGISTRATION(terminaltests);

void terminaltests::constructtest() {
  TreeTerminal t("test", 1234);
  EQ(std::string("test"), t.getName());
  EQ(unsigned(1234), t.id());
}

void terminaltests::notfolder() {
  TreeTerminal t("test", 1234);
  ASSERT(!t.isFolder());
}