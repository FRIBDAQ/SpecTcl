// Template for a test suite.



#include <config.h>
#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/Asserter.h>
#include "Asserts.h"

#include "CTreeParameter.h"
#include "CTreeParameterArray.h"
#include "SetLimitsVisitor.h"
#include "TreeTestSupport.h"

#ifdef HAVE_STD_NAMESPACE
using namespace std;
#endif


class SetLimitsTest : public CppUnit::TestFixture {
  CPPUNIT_TEST_SUITE(SetLimitsTest);
  CPPUNIT_TEST(limits);
  CPPUNIT_TEST_SUITE_END();


private:

public:
  void setUp() {
  }
  void tearDown() {
  }
protected:
  void limits();
};

CPPUNIT_TEST_SUITE_REGISTRATION(SetLimitsTest);

void SetLimitsTest::limits() {
  CTreeParameterArray g("george", 10, 0);
  SetLimitsVisitor v("*", 1.0, 200.0);
  double inc = g[0].getInc();
  for_each(CTreeParameter::begin(), CTreeParameter::end(), v);


  for(int i=0; i< 10; i++) {
    string name = MakeElementName("george", 0, 10, i);
    CheckConstructed(g[i], "testing setlimitsvis",
		     name, 100, 1.0, 200.0, 199.0/100.0, "unknown", true);
  }
}
