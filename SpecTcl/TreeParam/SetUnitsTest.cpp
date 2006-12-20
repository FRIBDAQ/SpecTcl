// Template for a test suite.
// This implementation of TreeParameter is based on the ideas and original code of::
//    Daniel Bazin
//    National Superconducting Cyclotron Lab
//    Michigan State University
//    East Lansing, MI 48824-1321
//
#include <config.h>
#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/Asserter.h>
#include "Asserts.h"

#include "CTreeParameter.h"
#include "CTreeParameterArray.h"
#include "SetUnitsVisitor.h"
#include "TreeTestSupport.h"

#ifdef HAVE_STD_NAMESPACE
using namespace std;
#endif


class SetUnitsTest : public CppUnit::TestFixture {
  CPPUNIT_TEST_SUITE(SetUnitsTest);
  CPPUNIT_TEST(Units);
  CPPUNIT_TEST_SUITE_END();


private:

public:
  void setUp() {
  }
  void tearDown() {
    TreeTestSupport::ClearMap();
  }
protected:
  void Units();
};

CPPUNIT_TEST_SUITE_REGISTRATION(SetUnitsTest);

void SetUnitsTest::Units() {
  CTreeParameterArray g("george", 10, 0);
  SetUnitsVisitor v("*", "cm");
  for_each(CTreeParameter::begin(), CTreeParameter::end(), v);

  for(int i =0; i < 10; i++) {
    string name = MakeElementName("george", 0, 10, i);
    CheckConstructed(g[i], "Testing set units",
		     name, 100, 1.0, 100.0, 99.0/100.0, "cm", true);
  }
}
