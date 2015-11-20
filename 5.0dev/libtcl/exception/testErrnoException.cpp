// Template for a test suite.
#include <config.h>
#include <ErrnoException.h>
#include <errno.h>

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/Asserter.h>
#include "Asserts.h"

#include <string>
#include <string.h>

using namespace std;



class testErrnoException : public CppUnit::TestFixture {
  CPPUNIT_TEST_SUITE(testErrnoException);
  CPPUNIT_TEST(errnoTest);
  CPPUNIT_TEST_SUITE_END();


private:

public:
  void setUp() {
  }
  void tearDown() {
  }
protected:
  void errnoTest();
};

CPPUNIT_TEST_SUITE_REGISTRATION(testErrnoException);

void testErrnoException::errnoTest() {
  errno = EINVAL;		// Fake an error...

  CErrnoException eno("It is a fake");
  EQ(EINVAL, eno.getErrno());
  EQ(EINVAL, eno.ReasonCode());
  EQ(string(strerror(EINVAL)), string(eno.ReasonText()));
  EQ(string("It is a fake"),   string(eno.WasDoing()));

}
