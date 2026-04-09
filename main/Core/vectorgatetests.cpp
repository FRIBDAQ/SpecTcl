#include <config.h>
#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/Asserter.h>
#include "Asserts.h"

#include "CTreeParameter.h"
#include "CTreeException.h"
#define private public
#define protected public
#include "CTreeParameterVector.h"
#include "CVectorGates.h"
#undef private
#undef protected
#include <Event.h>
#include <Parameter.h>

#include "TreeTestSupport.h"

#include <stdexcept>


class VGateTests : public CppUnit::TestFixture {
    CPPUNIT_TEST_SUITE(VGateTests);
    CPPUNIT_TEST(construct_and_1);
    CPPUNIT_TEST(construct_and_2);
    CPPUNIT_TEST(assign_and_1);
    CPPUNIT_TEST(assign_and_2);
    CPPUNIT_TEST(equal_and_1);
    CPPUNIT_TEST(equal_and_2);
    CPPUNIT_TEST(equal_and_3);
    CPPUNIT_TEST(equal_and_4);
    CPPUNIT_TEST(ne_and_1);
    CPPUNIT_TEST(ne_and_2);
    CPPUNIT_TEST(ne_and_3);
    CPPUNIT_TEST(ne_and_4);
    CPPUNIT_TEST(low_1);
    CPPUNIT_TEST(high_1);
    CPPUNIT_TEST(getVector_1);
    CPPUNIT_TEST(setlow_1);
    CPPUNIT_TEST(sethigh_1);
    CPPUNIT_TEST(setvector_1);
    CPPUNIT_TEST_SUITE_END();

protected:
    void construct_and_1();
    void construct_and_2();

    void assign_and_1();
    void assign_and_2();

    void equal_and_1();
    void equal_and_2();
    void equal_and_3();
    void equal_and_4();

    void ne_and_1();
    void ne_and_2();
    void ne_and_3();
    void ne_and_4();
    
    // And gates are used as a vehicle for testing
    // the base class methods:

    void low_1();
    void high_1();             // Selectors.
    void getVector_1();

    void setlow_1();
    void sethigh_1();
    void setvector_1();

private:
    CTreeParameterVector* m_pVector;
public:
    void setUp() {
        m_pVector = new CTreeParameterVector("test-vector");
    }
    void tearDown() {
        delete m_pVector;
        CTreeParameterVector::ClearMap();
        TreeTestSupport::ClearMap();
    }
};

CPPUNIT_TEST_SUITE_REGISTRATION(VGateTests);

void 
VGateTests::construct_and_1() {
    //  Basic construction.

    CVectorAndGate gate(100.0, 200.0, *m_pVector);

    // Was it correctly constructed:

    EQ(float(100.0), gate.m_fLow);
    EQ(float(200.0), gate.m_fHigh);
    EQ(m_pVector->name(), gate.m_vector.name());
}
void
VGateTests::construct_and_2() {
    // Copy construction test:

    CVectorAndGate source(100.0, 200.0, *m_pVector);
    CVectorAndGate gate(source);    // Copy construct
 
    EQ(float(100.0), gate.m_fLow);
    EQ(float(200.0), gate.m_fHigh);
    EQ(m_pVector->name(), gate.m_vector.name());
}

void
VGateTests::assign_and_1() {
    // Assing from self:

    CVectorAndGate source(100.0, 200.0, *m_pVector);

    auto& dest = source = source;
    ASSERT(&dest == &source);    // return value is the same object.

    EQ(float(100.0), source.m_fLow);
    EQ(float(200.0), source.m_fHigh);
    EQ(m_pVector->name(), source.m_vector.name());
}
    
void
VGateTests::assign_and_2() {
    // Assign from other:

    CTreeParameterVector vec("another");
    CVectorAndGate gate(1234.0, 4670.0, vec);
    
    CVectorAndGate source(100.0, 200.0, *m_pVector);

    auto& dest = gate = source;

    ASSERT(&dest == &gate);
    EQ(float(100.0), gate.m_fLow);
    EQ(float(200.0), gate.m_fHigh);
    EQ(m_pVector->name(), gate.m_vector.name());
}

void
VGateTests::equal_and_1() {
    // compare two identical gates for equality:

    CVectorAndGate g1(100.0, 200.0, *m_pVector);
    CVectorAndGate g2(100.0, 200.0, *m_pVector);

    ASSERT( g1 == g2);
}
void
VGateTests::equal_and_2() {
    // Compare gate that differ by low:

    CVectorAndGate g1(100.0, 200.0, *m_pVector);
    CVectorAndGate g2(150.0, 200.0, *m_pVector);

    ASSERT(!(g1 == g2));    // Not the same function call as g1 != g2.
}
void
VGateTests::equal_and_3() {
    // compare gates that differ by high:

    CVectorAndGate g1(100.0, 200.0, *m_pVector);
    CVectorAndGate g2(100.0, 250.0, *m_pVector);

    ASSERT(!(g1 == g2));
}
void
VGateTests::equal_and_4() {
    // Compare gates that differ by the vetor:

    CTreeParameterVector v("another");
    CVectorAndGate g1(100.0, 200.0, *m_pVector);
    CVectorAndGate g2(100.0, 200.0, v);
    ASSERT(!(g1 == g2));
}


void
VGateTests::ne_and_1() {
    // but they are equal:

    CVectorAndGate g1(100.0, 200.0, *m_pVector);
    CVectorAndGate g2(100.0, 200.0, *m_pVector);

    ASSERT(! (g2 != g2));
}

void 
VGateTests::ne_and_2() {
    // Compare gate that differ by low:

    CVectorAndGate g1(100.0, 200.0, *m_pVector);
    CVectorAndGate g2(150.0, 200.0, *m_pVector);

    ASSERT(g1 != g2);    // Not the same function call as g1 != g2.
}
void 
VGateTests::ne_and_3() {
    // compare gates that differ by high:

    CVectorAndGate g1(100.0, 200.0, *m_pVector);
    CVectorAndGate g2(100.0, 250.0, *m_pVector);

    ASSERT(g1 != g2);
}
void 
VGateTests::ne_and_4() {
    // Compare gates that differ by the vetor:

    CTreeParameterVector v("another");
    CVectorAndGate g1(100.0, 200.0, *m_pVector);
    CVectorAndGate g2(100.0, 200.0, v);
    ASSERT(g1 != g2);
}

void
VGateTests::low_1() {
    CVectorAndGate gate(100.0, 200.0, *m_pVector);
    EQ(float(100.0), gate.low());
}
void
VGateTests::high_1() {
    CVectorAndGate gate(100.0, 200.0, *m_pVector);
    EQ(float(200.0), gate.high());
}
void
VGateTests::getVector_1() {
    CVectorAndGate gate(100.0, 200.0, *m_pVector);
    EQ(m_pVector->name(), gate.getVector().name());
}

void
VGateTests::setlow_1() {
    CVectorAndGate gate(100.0, 200.0, *m_pVector);
    gate.setLow(150);
    EQ(float(150.0), gate.low());
}
void
VGateTests::sethigh_1() {
    CVectorAndGate gate(100.0, 200.0, *m_pVector);
    gate.setHigh(250.0);
    EQ(float(250.0), gate.high());
}
void
VGateTests::setvector_1() {
    CVectorAndGate gate(100.0, 200.0, *m_pVector);
    CTreeParameterVector g("george");
    gate.setVector(g);
    EQ(g.name(), gate.getVector().name());
}