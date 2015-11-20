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
#include "SetAllVisitor.h"
#include "TreeTestSupport.h"
#include <algorithm>

#ifdef HAVE_STD_NAMESPACE
using namespace std;
#endif





class SetAllVTest : public CppUnit::TestFixture {
  CPPUNIT_TEST_SUITE(SetAllVTest);
  CPPUNIT_TEST(SetArray);
  CPPUNIT_TEST_SUITE_END();


private:

public:
  void setUp() {
  }
  void tearDown() {
    TreeTestSupport::ClearMap();
  }
protected:
  void SetArray();
};

CPPUNIT_TEST_SUITE_REGISTRATION(SetAllVTest);

void 
SetAllVTest::SetArray() 
{
  CTreeParameterArray array("george", 10, 0);    // Defaults...

  SetAllVisitor visitor("*", 200, 0, 200, "cm");
  std::for_each(CTreeParameter::begin(), CTreeParameter::end(), visitor);

  for (int i = 0; i < 10; i++) {
    string name = MakeElementName("george", 0, 10, i);
    CheckConstructed(array[i], "SetArray",
		     name, 200, 0.0, 200.0, 
		     1.0, "cm", true);
  }
}
