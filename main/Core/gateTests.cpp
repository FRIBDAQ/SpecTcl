// Template for a test suite.

#include <config.h>
#include <histotypes.h>
#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/Asserter.h>
#include "GateFactory.h"


class GateTests : public CppUnit::TestFixture {
  CPPUNIT_TEST_SUITE(GateTests);
  CPPUNIT_TEST_SUITE_END();
  
protected:

private:

};

CPPUNIT_TEST_SUITE_REGISTRATION(GateTests);