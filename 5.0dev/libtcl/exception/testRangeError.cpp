// Template for a test suite.

#include <config.h>
#include <RangeError.h>


#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/Asserter.h>
#include "Asserts.h"


using namespace std;




class testRangeError : public CppUnit::TestFixture {
  CPPUNIT_TEST_SUITE(testRangeError);
  CPPUNIT_TEST(testHigh);
  CPPUNIT_TEST(testLow);
  CPPUNIT_TEST_SUITE_END();


private:

public:
  void setUp() {
  }
  void tearDown() {
  }
protected:
  void testHigh();
  void testLow();
};

CPPUNIT_TEST_SUITE_REGISTRATION(testRangeError);


// Test range error with the value too big.

void testRangeError::testHigh() {
  CRangeError toobig(0, 100, 101, 
		     "I am too high");
  EQ(0, toobig.getLow());
  EQ(100, toobig.getHigh());
  EQ(101, toobig.getRequested());
  EQ(string("Range error: 101 is outside the range [ 0..100]"), 
     string(toobig.ReasonText()));
  EQ(static_cast<int>(CRangeError::knTooHigh), toobig.ReasonCode());

}


void testRangeError::testLow() {
  CRangeError toosmall(0, 100, -1, " Too small");

  EQ(static_cast<int>(CRangeError::knTooLow), toosmall.ReasonCode());

}
