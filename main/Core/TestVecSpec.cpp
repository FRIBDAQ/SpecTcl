/**
 * Tests for the CSpectrum1DVec spectrum type.
 */

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/Asserter.h>
#include "Asserts.h"
#include <vector>

#include "Parameter.h"
#include "Spectrum1DVec.h"
#include <Event.h>



class Test1DVec : public CppUnit::TestFixture {
    CPPUNIT_TEST_SUITE(Test1DVec);
    CPPUNIT_TEST_SUITE_END();
};

CPPUNIT_TEST_SUITE_REGISTRATION(Test1DVec);