/**
 * Tests for the CSpectrum1DVec spectrum type.
 */

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/Asserter.h>
#include "Asserts.h"
#include <vector>
#include "CTreeParameterVector.h"
#include "Parameter.h"
#define private public
#include "Spectrum1DVec.h"
#undef private
#include <Event.h>



class Test1DVec : public CppUnit::TestFixture {
    CPPUNIT_TEST_SUITE(Test1DVec);
    CPPUNIT_TEST(construct_1);
    CPPUNIT_TEST(construct_2);
    CPPUNIT_TEST(setget_1);
    CPPUNIT_TEST_SUITE_END();

protected:
    void construct_1();
    void construct_2();
    void setget_1();
public:
    void setUp() {}
    void tearDown() {}
};

CPPUNIT_TEST_SUITE_REGISTRATION(Test1DVec);


void Test1DVec::construct_1() {
    // constuctor with just bins:

    CTreeParameterVector p("Testing", 0.0, 100.0, "junk");
    CSpectrum1DVecL spec("test", 0, p, 100);

    EQ(std::string("Testing"), spec.m_parameters.name());   // correct parameter.
    EQ(UInt_t(100), spec.m_nChannels);

    // Assume all the other stuff is right in the base class.
}

void Test1DVec::construct_2() {
    CTreeParameterVector p("Testing", 0.0, 100.0, "junk");
    CSpectrum1DVecL spec("test", 0, p, 100, 0.0, 100.0);

    EQ(std::string("Testing"), spec.m_parameters.name());   // correct parameter.
    EQ(UInt_t(100), spec.m_nChannels);
}
// Before we can do interesting stuff, we need to be able to set/get chanels.

void Test1DVec::setget_1() {
    CTreeParameterVector p("Testing", 0.0, 100.0, "junk");
    CSpectrum1DVecL spec("test", 0, p, 100, 0.0, 100.0);
    UInt_t index=50;
    spec.set(&index, 1234);

    ULong_t value = spec[&index];

    EQ(ULong_t(1234), value);
}