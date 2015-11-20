// Template for a test suite.
#include <config.h>
#include <StreamIOError.h>
#include <iostream>

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/Asserter.h>
#include "Asserts.h"



using namespace std;



class testStreamException : public CppUnit::TestFixture {
  CPPUNIT_TEST_SUITE(testStreamException);
  CPPUNIT_TEST(endfileTest);
  CPPUNIT_TEST(badsetTest);
  CPPUNIT_TEST(failsetTest);
  CPPUNIT_TEST(negativeTest);
  CPPUNIT_TEST(toobigTest);
  CPPUNIT_TEST_SUITE_END();


private:

public:
  void setUp() {
  }
  void tearDown() {
  }
protected:
  void endfileTest();
  void badsetTest();
  void failsetTest();
  void negativeTest();
  void toobigTest();


};

CPPUNIT_TEST_SUITE_REGISTRATION(testStreamException);

// Test the creation of an end file exception.
//
void testStreamException::endfileTest() {
  CStreamIOError endfile(CStreamIOError::EndFile, 
			 "End File Test", cin);
  EQ(0, endfile.ReasonCode());
  EQ(string("End of file encountered (prematurely) "), 
     string(endfile.ReasonText()));
}
// Test a bad bit set exception.

void testStreamException::badsetTest() {
  CStreamIOError badset(CStreamIOError::BadSet,
			"Bad bit set", cin);
  EQ(1, badset.ReasonCode());
  EQ(string("Bad bit set in ios:: "),
     string(badset.ReasonText()));
}
// Test a Fail bit set exceptino:

void testStreamException::failsetTest() {
  CStreamIOError failset(CStreamIOError::FailSet,
			 "Fail bit set", cin);
  EQ(2, failset.ReasonCode());
  EQ(string("Fail bit set in ios:: "),
     string(failset.ReasonText()));

}
// Test an exception with a bad reason (negative).

void testStreamException::negativeTest() {
  CStreamIOError negative(static_cast<CStreamIOError::IoStreamConditions>(-1),
			  "Negative",
			  cin);
  EQ(string("Invalid error enumerator!!!"),
     string(negative.ReasonText()));

}
// Test an exception with a reason out of range positive.

void testStreamException::toobigTest() {
  CStreamIOError toobig(static_cast<CStreamIOError::IoStreamConditions>(3),
			"Positive",
			cin);
  EQ(string("Invalid error enumerator!!!"),
     string(toobig.ReasonText()));
}
