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
    CPPUNIT_TEST_SUITE_END();

protected:
    void construct_and_1();
    void construct_and_2();

    void assign_and_1();
    void assign_and_2();

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