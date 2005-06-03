// Template for a test suite.

#include <config.h>
#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/Asserter.h>
#include "Asserts.h"

#include "CTreeParameter.h"
#include "CTreeParameterArray.h"
#include "SetWidthVisitor.h"
#include "TreeTestSupport.h"

#ifdef HAVE_STD_NAMESPACE
using namespace std;
#endif




class SetWidthTest : public CppUnit::TestFixture {
  CPPUNIT_TEST_SUITE(SetWidthTest);
  CPPUNIT_TEST(Set);
  CPPUNIT_TEST_SUITE_END();


private:

public:
  void setUp() {
  }
  void tearDown() {
    TreeTestSupport::ClearMap();
  }
protected:
  void Set();
};

CPPUNIT_TEST_SUITE_REGISTRATION(SetWidthTest);

void SetWidthTest::Set() {
  CTreeParameterArray  g("george", 10, 0);
  SetWidthVisitor      v("g*", 0.2);

  for_each(CTreeParameter::begin(), CTreeParameter::end(), v);
  
  for(int i =0 ; i < 10; i++) {
    string name= MakeElementName("george", 0, 10, i);
    CheckConstructed(g[i], "Testing set width",
		     name, 100, 1.0, 1.0+100*0.2, 0.2, "unknown", true);
  }
}
