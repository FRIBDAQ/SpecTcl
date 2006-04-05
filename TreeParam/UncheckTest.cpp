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
#include "UncheckVisitor.h"
#include "SetUnitsVisitor.h"
#include "TreeTestSupport.h"

#ifdef HAVE_STD_NAMESPACE
using namespace std;
#endif




class UncheckTest : public CppUnit::TestFixture {
  CPPUNIT_TEST_SUITE(UncheckTest);
  CPPUNIT_TEST(theTest);
  CPPUNIT_TEST_SUITE_END();


private:

public:
  void setUp() {
  }
  void tearDown() {
    TreeTestSupport::ClearMap();
  }
protected:
  void theTest();
};

CPPUNIT_TEST_SUITE_REGISTRATION(UncheckTest);

void UncheckTest::theTest() 
{
  CTreeParameterArray g("george", 10, 0);
  UncheckVisitor      v("*");
  SetUnitsVisitor     vu("*", "cm");

  // First get them all changed:

  for_each(CTreeParameter::begin(), CTreeParameter::end(), vu);

  // Now reset the change flags:

  for_each(CTreeParameter::begin(), CTreeParameter::end(), v);

  for(int i=0; i < 10; i++) {
    ASSERT(!(g[i].hasChanged()));
  }
}
