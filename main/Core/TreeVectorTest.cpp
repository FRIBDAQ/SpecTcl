//
// Test the tree parameter vector class:

#include <config.h>
#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/Asserter.h>
#include "Asserts.h"

#include "CTreeParameter.h"
#include "CTreeException.h"
#define private public
#include "CTreeParameterVector.h"
#undef private
#include <Event.h>
#include <Parameter.h>

#include "TreeTestSupport.h"


#ifdef HAVE_STD_NAMESPACE
using namespace std;
#endif


class TreeVectorTests : public CppUnit::TestFixture {
    CPPUNIT_TEST_SUITE(TreeVectorTests);
    CPPUNIT_TEST_SUITE_END();
public:
  void setUp() {
  }
  void tearDown() {
    CTreeParameterVector::ClearMap();
    TreeTestSupport::ClearMap();
  }

};

CPPUNIT_TEST_SUITE_REGISTRATION(TreeVectorTests);