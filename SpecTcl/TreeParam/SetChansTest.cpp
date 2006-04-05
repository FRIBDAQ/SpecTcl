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
#include "SetChannelsVisitor.h"
#include "TreeTestSupport.h"

#ifdef HAVE_STD_NAMESPACE
using namespace std;
#endif


class SetChanTest : public CppUnit::TestFixture {
  CPPUNIT_TEST_SUITE(SetChanTest);
  CPPUNIT_TEST(SetChans);
  CPPUNIT_TEST_SUITE_END();


private:

public:
  void setUp() {
  }
  void tearDown() {
    TreeTestSupport::ClearMap();
  }
protected:
  void SetChans();
};

CPPUNIT_TEST_SUITE_REGISTRATION(SetChanTest);

void SetChanTest::SetChans() {
  CTreeParameterArray g("george", 10, 0);
  SetChannelsVisitor v("*", 200);
  
  double inc = g[0].getInc();
  for_each(CTreeParameter::begin(), CTreeParameter::end(), v);

  inc  = inc /2.0;
  for(int i =0; i < 10; i++) {
    string name = MakeElementName("george", 0, 10, i);
    CheckConstructed(g[i], "testing setchanvisitor", 
		     name, 200, 1.0, 100.0, inc, "unknown", true);
  }
  
  
}
