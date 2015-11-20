// Template for a test suite.

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/Asserter.h>
#include "Asserts.h"

#define private public
#include "CValidValue.h"
#undef private

#include <string>



class ValidValueTests : public CppUnit::TestFixture {
  CPPUNIT_TEST_SUITE(ValidValueTests);
  // Canonical tests:

  CPPUNIT_TEST(cons1);
  CPPUNIT_TEST(cons2);
  CPPUNIT_TEST(copycons);
  CPPUNIT_TEST(copyconsthrow);
  CPPUNIT_TEST(assign);
  CPPUNIT_TEST(assignthrows);
  CPPUNIT_TEST(equals);
  CPPUNIT_TEST(equalsthrows);
  CPPUNIT_TEST(notequals);
  CPPUNIT_TEST(nethrows);

  // Validity manipulation/testing

  CPPUNIT_TEST(reset);
  CPPUNIT_TEST(valid);

  // Assign from T - this constructs then sets.
 
  CPPUNIT_TEST(Tassignset);

  // Get from T (operator T()).
  
  CPPUNIT_TEST(Tget);
  CPPUNIT_TEST(Tgetthrows);


  CPPUNIT_TEST_SUITE_END();


private:

public:
  void setUp() {
  }
  void tearDown() {
  }
protected:
  void cons1();
  void cons2();
  void copycons();
  void copyconsthrow();
  void assign();
  void assignthrows();
  void equals();
  void equalsthrows();
  void notequals();
  void nethrows();

  void reset();
  void valid();

  void Tassignset();

  void Tget();
  void Tgetthrows();

  void equalsT();

};

CPPUNIT_TEST_SUITE_REGISTRATION(ValidValueTests);

/**
 * Constructs with m_modified false.
 */

void ValidValueTests::cons1() {
  CValidValue<int> v;
  ASSERT(!v.m_modified);
}
/**
 * Constructs with initial value makes it modified and keeps the value:
 */
void
ValidValueTests::cons2() {
  CValidValue<int> v(1234);
  ASSERT(v.m_modified);
  EQ(1234, v.m_value);
}
/**
 * Copy construction results in the same value and validity:
 */
void ValidValueTests::copycons()
{
  CValidValue<int> v1(1234);
  CValidValue<int> v2(v1);

  ASSERT(v2.m_modified);
  EQ(1234, v2.m_value);
}
/**
 * copy constructing an invalid value throws.
 */
void
ValidValueTests::copyconsthrow()
{
  CValidValue<int> v1;		// invalid value.
  bool threw = false;
  bool rthrew = false;
  std::string msg;
  try {
    CValidValue<int> v2(v1);
  }
  catch (invalid_exception& e) {
    threw = true;
    rthrew = true;
    msg = e.what();
  }
  catch (...) {
    threw = true;
  }
  ASSERT(threw);
  ASSERT(rthrew);
  EQ(std::string("Valid value used on lhs when not valid"), msg);
}
/**
 * Assign from other ValidValue.
 */
void ValidValueTests::assign()
{
  CValidValue<int> v1(6);
  CValidValue<int> v2;
  v2 = v1;

  ASSERT(v2.m_modified);
  EQ(v1.m_value, v2.m_value);
 
}
/**
 * assignthrows
 *   Assignment from an uninitialized obj throws.
 */
void
ValidValueTests::assignthrows()
{
  CValidValue<int> v1;
  CValidValue<int> v2;

  bool threw = false;
  bool rthrew = false;
  std::string msg;
  try {
    v2 = v1;;
  }
  catch (invalid_exception& e) {
    threw = true;
    rthrew = true;
    msg = e.what();
  }
  catch (...) {
    threw = true;
  }
  ASSERT(threw);
  ASSERT(rthrew);
  EQ(std::string("Valid value used on lhs when not valid"), msg);
}
/**
 * equals
 *   Equals implies the values are the same.
 */
void
ValidValueTests::equals()
{
  CValidValue<int> v1(5);
  CValidValue<int> v2(5);
  CValidValue<int> v3(75);	// Won't match.

  ASSERT(v1 == v2);
  ASSERT(v1 != v3);
  ASSERT(!(v2 == v3));
}
/**
 * equalsthrows
 *  If either lhs or rhs of == is not valid this should throw.
 */
void
ValidValueTests::equalsthrows()
{
  CValidValue<int> v1;
  CValidValue<int> v2(3);
  
  bool threw = false;
  bool rthrew = false;
  std::string msg;
  try {
    v1 == v2;
  }
  catch (invalid_exception& e) {
    threw = true;
    rthrew = true;
    msg = e.what();
  }
  catch (...) {
    threw = true;
  }
  ASSERT(threw);
  ASSERT(rthrew);
  EQ(std::string("Valid value used on lhs when not valid"), msg);

  threw = false;
  rthrew = false;
  msg = "";
  try {
    v2 == v1;
  }
  catch (invalid_exception& e) {
    threw = true;
    rthrew = true;
    msg = e.what();
  }
  catch (...) {
    threw = true;
  }
  ASSERT(threw);
  ASSERT(rthrew);
  EQ(std::string("Valid value used on lhs when not valid"), msg);
}
/**
 * notequals
 *
 *    Not equal if the values differ.
 */
void 
ValidValueTests::notequals()
{
  CValidValue<int> v1(17);
  CValidValue<int> v2(17);
  CValidValue<int> v3(1234);

  ASSERT(v1 != v3);
  ASSERT(v2 != v3);
  ASSERT(!(v1 != v2));
  
}
void
ValidValueTests::nethrows()
{
  CValidValue<int> v1;
  CValidValue<int> v2(8);
 
  bool threw = false;
  bool rthrew = false;
  std::string msg;
  try {
    v1 != v2;
  }
  catch (invalid_exception& e) {
    threw = true;
    rthrew = true;
    msg = e.what();
  }
  catch (...) {
    threw = true;
  }
  ASSERT(threw);
  ASSERT(rthrew);
  EQ(std::string("Valid value used on lhs when not valid"), msg);

  threw = false;
  rthrew = false;
  msg = "";
  try {
    v2 != v1;
  }
  catch (invalid_exception& e) {
    threw = true;
    rthrew = true;
    msg = e.what();
  }
  catch (...) {
    threw = true;
  }
  ASSERT(threw);
  ASSERT(rthrew);
  EQ(std::string("Valid value used on lhs when not valid"), msg);
}
/**
 * reset - reset the valid value flag:
 */
void
ValidValueTests::reset()
{
  CValidValue<int> v(1);		// Valid.
  v.reset();
  ASSERT(!v.m_modified);
}
/**
 * valid
 *
 * Ensure that valid returns the correct state of the object
 */
void 
ValidValueTests::valid()
{
  CValidValue<int> v1(1);	// Valid.
  ASSERT(v1.isvalid());

  CValidValue<int> v2;		// Invalid.
  ASSERT(!v2.isvalid());
}
/**
 * Tassignset
 *
 *  Assigning a T should set the m_value:
 */
void
ValidValueTests::Tassignset()
{
  CValidValue<int> v(1);

  v = 1234;
  EQ(1234, v.m_value);
}
/**
 * Tget
 *
 *  Get the valuie back out of the object.
 */
void
ValidValueTests::Tget()
{
  CValidValue<int> v(1234);
  int i = v;

  EQ(1234, i);
}
/**
 * Tgetthrows
 */
void
ValidValueTests::Tgetthrows()
{
  CValidValue<int> v;		// invalid.

  bool threw = false;
  bool rthrew= false;
  std::string msg;

  try {
    int i = v;
  }
  catch (invalid_exception& e) {
    threw = true;
    rthrew= true;
    msg = e.what();
  }
  catch (...) {
    threw = true;
  }
  ASSERT(threw);
  ASSERT(rthrew);
  EQ(std::string("Valid value used on lhs when not valid"), msg);
}
