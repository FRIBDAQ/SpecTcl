// Template for a test suite.

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/Asserter.h>
#include "Asserts.h"
#include "CTrueCondition.h"
#include "CFalseCondition.h"
class CSpectrum {
};

class ConditionTests : public CppUnit::TestFixture {
  CPPUNIT_TEST_SUITE(ConditionTests);
  CPPUNIT_TEST(trueTest);
  CPPUNIT_TEST(validateTrue);
  CPPUNIT_TEST(falseTest);
  CPPUNIT_TEST(validateFalse);
  CPPUNIT_TEST_SUITE_END();


private:

public:
  void setUp() {
  }
  void tearDown() {
  }
protected:
  void trueTest();
  void validateTrue();
  void falseTest();
  void validateFalse();
};

CPPUNIT_TEST_SUITE_REGISTRATION(ConditionTests);

void ConditionTests::trueTest() {
  CTrueCondition c;
  CSpectrum stub;
  ASSERT(c(stub));
}
/**
 * validateTrue
 *
 *  Even after a validationthe condition should give
 *  true.
 */
void
ConditionTests::validateTrue()
{
  CTrueCondition c;
  CSpectrum      stub;
  c.validate(stub);
  ASSERT(c(stub));
}
/**
 * false ... this predicate always 'fails'.
 */
void
ConditionTests::falseTest()
{
  CFalseCondition c;
  CSpectrum stub;

  ASSERT(!c(stub));
}
void 
ConditionTests::validateFalse()
{
  CFalseCondition c;
  CSpectrum stub;
  c.validate(stub);
  ASSERT(!c(stub));
}
