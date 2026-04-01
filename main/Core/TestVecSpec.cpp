/**
 * Tests for the CSpectrum1DVec spectrum type.
 */

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/Asserter.h>
#include "Asserts.h"
#include <vector>
#include "CTreeParameterVector.h"
#include "CTreeParameter.h"
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
    CPPUNIT_TEST(incr_1);
    CPPUNIT_TEST(uses_1);
    CPPUNIT_TEST(uses_2);
    CPPUNIT_TEST(getids_1);
    CPPUNIT_TEST(resolutions_1);
    CPPUNIT_TEST_SUITE_END();

protected:
    void construct_1();
    void construct_2();
    void setget_1();
    void incr_1();

    void uses_1();
    void uses_2();

    void getids_1();

    void resolutions_1();
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

void Test1DVec::incr_1() {
    CEvent event;
    CTreeParameterVector p("test");

    CSpectrum1DVecL spec("testing", 0, p, 100);   // Unmapped spectrum 0-99.

    // Set up for histogramming.

    CTreeParameter::setEvent(event);
    CTreeParameterVector::BeginEvent();

    // SEt the  parameters
    p.push_back(1.0);
    p.push_back(2.0);
    p.push_back(50.0);

    spec.Increment(event);

    UInt_t index(1.0);
    EQ(ULong_t(1), spec[&index]);
    index = 2;
    EQ(ULong_t(1), spec[&index]);
    index = 50;
    EQ(ULong_t(1), spec[&index]);

}

void Test1DVec::uses_1() {
    CEvent event;
    CTreeParameterVector p("test");

    CSpectrum1DVecL spec("testing", 0, p, 100);   // Unmapped spectrum 0-99.

    // Set up for histogramming.

    CTreeParameter::setEvent(event);
    CTreeParameterVector::BeginEvent();

    // SEt the  parameters
    p.push_back(1.0);
    p.push_back(2.0);
    p.push_back(50.0);
    
    // These are consecutive parameters from the first one.

    UInt_t id = p.createdParams()[0]->getId();

    ASSERT(spec.UsesParameter(id));
    ASSERT(spec.UsesParameter(id+1));
    ASSERT(spec.UsesParameter(id+2));
}

void Test1DVec::uses_2() {
    CEvent event;
    CTreeParameterVector p("test");

    CSpectrum1DVecL spec("testing", 0, p, 100);   // Unmapped spectrum 0-99.

    // Set up for histogramming.

    CTreeParameter::setEvent(event);
    CTreeParameterVector::BeginEvent();

    // SEt the  parameters
    p.push_back(1.0);
    p.push_back(2.0);
    p.push_back(50.0);
    

    // Not going to use the last one+1

    UInt_t id = p.createdParams()[2]->getId() + 1;
    ASSERT(!spec.UsesParameter(id));

}

void Test1DVec::getids_1() {
    CEvent event;
    CTreeParameterVector p("test");

    CSpectrum1DVecL spec("testing", 0, p, 100);   // Unmapped spectrum 0-99.

    // Set up for histogramming.

    CTreeParameter::setEvent(event);
    CTreeParameterVector::BeginEvent();

    // SEt the  parameters
    p.push_back(1.0);
    p.push_back(2.0);
    p.push_back(50.0);

    std::vector<UInt_t> ids;
    spec.GetParameterIds(ids);

    EQ(p.createdParams().size(), ids.size());
    for (int i =0; i < ids.size(); i++) {
        UInt_t id = p.createdParams()[i]->getId();
        EQ(id, ids[i]);
    }
}

void Test1DVec::resolutions_1() {
    CEvent event;
    CTreeParameterVector p("test");

    CSpectrum1DVecL spec("testing", 0, p, 127);
    std::vector<UInt_t> res;
    spec.GetResolutions(res);

    EQ(size_t(1), res.size());
    EQ(UInt_t(7), res.at(0));
}