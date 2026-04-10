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
    CPPUNIT_TEST(iterate_1);
    CPPUNIT_TEST(size_1);
    CPPUNIT_TEST(constituent_1);
    CPPUNIT_TEST(constituent_2) ;
    CPPUNIT_TEST(points_1);
    CPPUNIT_TEST(vname_1);
    CPPUNIT_TEST(type_and_1);
    CPPUNIT_TEST(ingate_and_1);
    CPPUNIT_TEST(ingate_and_2);
    CPPUNIT_TEST(ingate_and_3);
    CPPUNIT_TEST(clone_and_1);

    // Now test the or gates with similar stuff:

    CPPUNIT_TEST(construct_or_1);
    CPPUNIT_TEST(construct_or_2);
    CPPUNIT_TEST(assign_or_1);
    CPPUNIT_TEST(assign_or_2);
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

    void iterate_1();
    void size_1();
    void constituent_1();
    void constituent_2();
    void points_1();
    void vname_1();

    void type_and_1();     // type of an and gate.

    void ingate_and_1();  
    void ingate_and_2();
    void ingate_and_3();

    void clone_and_1();

    void construct_or_1();
    void construct_or_2();

    void assign_or_1();
    void assign_or_2();

private:
    CTreeParameterVector* m_pVector;
    CEvent*               m_pEvent;
public:
    void setUp() {
        m_pVector = new CTreeParameterVector("test-vector");
        m_pEvent = new CEvent;
        CTreeParameterVector::BeginEvent();
        CTreeParameter::setEvent(*m_pEvent);
    }
    void tearDown() {
        delete m_pVector;
        delete m_pEvent;
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

void
VGateTests::iterate_1() {
    // I only get one iteration before I hit end:

    CVectorAndGate gate(100.0, 200.0, *m_pVector);
    auto p = gate.Begin();
    p++;
    ASSERT(p == gate.End());
}
void 
VGateTests::size_1() {
    CVectorAndGate gate(100.0, 200.0, *m_pVector);
    EQ(UInt_t(1), gate.Size());
}
void
VGateTests::constituent_1() {
    CVectorAndGate gate(100.0, 200.0, *m_pVector);

    auto p = gate.Begin();
    std::string actual  = gate.GetConstituent(p);
    EQ(std::string("test-vector 100 200"), actual);
}
void
VGateTests::constituent_2() {
    CVectorAndGate gate(100.0, 200.0, *m_pVector);

    auto p = gate.Begin();
    p++;
    std::string actual  = gate.GetConstituent(p);
    EQ(std::string(""), actual);
}

void
VGateTests::points_1() {
    CVectorAndGate gate(100.0, 200.0, *m_pVector);

    std::vector<FPoint> pts = gate.getPoints();

    EQ(size_t(2), pts.size());
    
    EQ(float(100.0), pts[0].X());
    EQ(float(0), pts[0].Y());

    EQ(float(200.0), pts[1].X());
    EQ(float(0), pts[1].Y());
}
void
VGateTests::vname_1() {
    CVectorAndGate gate(100.0, 200.0, *m_pVector);
    EQ(std::string("test-vector"), gate.getVectorName());
}

void
VGateTests::type_and_1() {
    CVectorAndGate gate(100.0, 200.0, *m_pVector);
    EQ(std::string("vs*"), gate.Type());
}

void
VGateTests::ingate_and_1() {
    // THe vector is in the gate.
    CVectorAndGate gate(100.0, 200.0, *m_pVector);
 
    //  Put some points, all in the vector:

    m_pVector->push_back(100.0);   // on the left edge is ok.
    m_pVector->push_back(110.0);

    ASSERT(gate.inGate(*m_pEvent));
}
void
VGateTests::ingate_and_2() {
    // The vector has a point on the right edge (out of gate):

    CVectorAndGate gate(100.0, 200.0, *m_pVector);
 
    m_pVector->push_back(110.0);  // ok.
    m_pVector->push_back(200.0);  // not ok.
    m_pVector->push_back(150.0);  // ok.

    ASSERT(!gate.inGate(*m_pEvent));
}

void
VGateTests::ingate_and_3() {
    // empty vector -> kfFalse.

    CVectorAndGate gate(100.0, 200.0, *m_pVector);
    ASSERT(!gate.inGate(*m_pEvent));
}

void 
VGateTests::clone_and_1() {
    // Cloning an and gate should give the same
    // basic gate:

    CVectorAndGate g1(100.0, 200.0, *m_pVector);
    CGate*  g2generic = g1.clone();
    CVectorGate* g2 = dynamic_cast<CVectorGate*>(g2generic);

    ASSERT(g2);
    

    EQ(std::string("vs*"), g2generic->Type());
    EQ(float(100.0), g2->low());
    EQ(float(200.0), g2->high());
    EQ(m_pVector->name(), g2->getVectorName());

    delete g2;
}

void
VGateTests::construct_or_1() {
    //  Basic construction.

    CVectorOrGate gate(100.0, 200.0, *m_pVector);

    // Was it correctly constructed:

    EQ(float(100.0), gate.m_fLow);
    EQ(float(200.0), gate.m_fHigh);
    EQ(m_pVector->name(), gate.m_vector.name());
}
void
VGateTests::construct_or_2() {
    // Copy construction test:

    CVectorOrGate source(100.0, 200.0, *m_pVector);
    CVectorOrGate gate(source);    // Copy construct
 
    EQ(float(100.0), gate.m_fLow);
    EQ(float(200.0), gate.m_fHigh);
    EQ(m_pVector->name(), gate.m_vector.name());
}


void
VGateTests::assign_or_1() {
    // Assing from self:

    CVectorOrGate source(100.0, 200.0, *m_pVector);

    auto& dest = source = source;
    ASSERT(&dest == &source);    // return value is the same object.

    EQ(float(100.0), source.m_fLow);
    EQ(float(200.0), source.m_fHigh);
    EQ(m_pVector->name(), source.m_vector.name());
}
    
void
VGateTests::assign_or_2() {
    // Assign from other:

    CTreeParameterVector vec("another");
    CVectorOrGate gate(1234.0, 4670.0, vec);
    
    CVectorOrGate source(100.0, 200.0, *m_pVector);

    auto& dest = gate = source;

    ASSERT(&dest == &gate);
    EQ(float(100.0), gate.m_fLow);
    EQ(float(200.0), gate.m_fHigh);
    EQ(m_pVector->name(), gate.m_vector.name());
}
