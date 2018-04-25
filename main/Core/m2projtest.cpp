// Template for a test suite.

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/Asserter.h>
#include "Asserts.h"
#include <RangeError.h>
#include <ErrnoException.h>
#include "Contour.h"
#include "GateContainer.h"
#include "TrueGate.h"
#include <histotypes.h>
#include <memory>

// Include the projection so that its private elements can be examined.

#define private public
#include <CM2Projection.h>
#undef private

class m2projtest : public CppUnit::TestFixture {
  CPPUNIT_TEST_SUITE(m2projtest);
  
  // Constructor tests: 
  
  CPPUNIT_TEST(nogatefail);
  CPPUNIT_TEST(badgatetype);
  CPPUNIT_TEST(simpleconstruct);
  CPPUNIT_TEST(reorderconstruct);
  CPPUNIT_TEST(flippedconstruct);
  
  // Query methods:
  
  CPPUNIT_TEST(querymethods);
  CPPUNIT_TEST(getdef);
  
  CPPUNIT_TEST(setget);             // Can only test symmetrically.
  
  
  // Tests for the Increment operator.
  
  CPPUNIT_TEST(emptyEvent);
  CPPUNIT_TEST(allInSame);
  CPPUNIT_TEST(allOkDifferent);
  CPPUNIT_TEST(someOutside);
  CPPUNIT_TEST(allOutside);
  CPPUNIT_TEST(incrementFlip);
  
  CPPUNIT_TEST_SUITE_END();
protected:
  void nogatefail();
  void badgatetype();
  void simpleconstruct();
  void reorderconstruct();
  void flippedconstruct();

  void querymethods();
  void getdef();
  
  void setget();
  
  void emptyEvent();
  void allInSame();
  void allOkDifferent();
  void someOutside();
  void allOutside();
  void incrementFlip();  // Y projections
  
private:
  CM2ProjectionL* makeSpectrum();
public:
  void setUp() {
  }
  void tearDown() {
  }

};

CPPUNIT_TEST_SUITE_REGISTRATION(m2projtest);


/**
 * Make a simple Spectrum common code for many of the methods.
 */
CM2ProjectionL*
m2projtest::makeSpectrum()
{
    std::vector<UInt_t> xpars={1, 3, 5};
    std::vector<UInt_t> ypars={2, 4, 6};
    
    // The statics below ensure these objects don't go out of scope.
    
    Float_t xpts[] = {0.0, 50.0, 50.0, 0.0};  // Contour is a square at origin
    Float_t ypts[] = {0.0, 0.0, 50.0, 50.0};  // 50x50 (roughly).
    static CContour c1(1,2, 4, xpts, ypts);
    static CContour c2(3,4, 4, xpts, ypts);
    static CContour c3(5,6, 4, xpts, ypts);
    static CGateContainer g1("c1", 1, c1);
    static CGateContainer g2("c2", 2, c2);
    static CGateContainer g3("c3", 3, c3);
    std::vector<CGateContainer*> gates={&g1, &g2, &g3};
    
    
    // Gate are static but need to be reset each time.
    // Furthermore gate containers get copy constructed copies of the gates
    // so we must reset via the gate containers not the gates.
    
    g1->Reset();
    g2->Reset();
    g3->Reset();
    
    
    return new
        CM2ProjectionL("good", 0, xpars, ypars, gates, 128, 0.0, 127.0);
}


/////////////////////////////////////////////////////////////////////////////


// Not passing sufficient gates is a range error:

void m2projtest::nogatefail() {
    
    std::vector<UInt_t> xpars={1, 3, 5};
    std::vector<UInt_t> ypars={2, 4, 6};
    std::vector<CGateContainer*> empty;
    
    CPPUNIT_ASSERT_THROW(
        CM2ProjectionL bad("bad", 0, xpars, ypars, empty, 128, 0.0, 127.0),
        CRangeError
    );
}

// Passing gates that are not contours also gives an exception:

void m2projtest::badgatetype() {
    std::vector<UInt_t> xpars={1, 3, 5};
    std::vector<UInt_t> ypars={2, 4, 6};
    
    CTrueGate g;
    CGateContainer g1("g1", 1, g);
    CGateContainer g2("g2", 2, g);
    CGateContainer g3("g3", 3, g);
    std::vector<CGateContainer*> badg = {&g1, &g2, &g3};
    
    CPPUNIT_ASSERT_THROW(
        CM2ProjectionL bad("bad", 0, xpars, ypars, badg, 128, 0.0, 127.0),
        CErrnoException
    );
    
}
// Simple construction... no need to reorder gates, no parameter oder diffs:


void m2projtest::simpleconstruct()
{
    std::vector<UInt_t> xpars={1, 3, 5};
    std::vector<UInt_t> ypars={2, 4, 6};
    
    Float_t xpts[] = {0.0, 50.0, 50.0, 0.0};  // Contour is a square at origin
    Float_t ypts[] = {0.0, 0.0, 50.0, 50.0};  // 50x50 (roughly).
    CContour c1(1,2, 4, xpts, ypts);
    CContour c2(3,4, 4, xpts, ypts);
    CContour c3(5,6, 4, xpts, ypts);
    CGateContainer g1("c1", 1, c1);
    CGateContainer g2("c2", 2, c2);
    CGateContainer g3("c3", 3, c3);
    std::vector<CGateContainer*> gates={&g1, &g2, &g3};
    
    CPPUNIT_ASSERT_NO_THROW(
        CM2ProjectionL good("good", 0, xpars, ypars, gates, 128, 0.0, 127.0)
    );
}
// The gates must be reordered to match parameter order:

void m2projtest::reorderconstruct()
{
    std::vector<UInt_t> xpars={1, 3, 5};
    std::vector<UInt_t> ypars={2, 4, 6};
    
    Float_t xpts[] = {0.0, 50.0, 50.0, 0.0};  // Contour is a square at origin
    Float_t ypts[] = {0.0, 0.0, 50.0, 50.0};  // 50x50 (roughly).
    CContour c1(1,2, 4, xpts, ypts);
    CContour c2(3,4, 4, xpts, ypts);
    CContour c3(5,6, 4, xpts, ypts);
    CGateContainer g1("c1", 1, c1);
    CGateContainer g2("c2", 2, c2);
    CGateContainer g3("c3", 3, c3);
    std::vector<CGateContainer*> gates = {&g3, &g2, &g1};  // Reversed order.
    
    CM2ProjectionL good("good", 0, xpars, ypars, gates, 128, 0.0, 127.0);
      
    std::vector<CGateContainer*> reordered = good.getRoiGates();
    EQ(UInt_t(1), reordered[0]->getNumber());
    EQ(UInt_t(2), reordered[1]->getNumber());
    EQ(UInt_t(3), reordered[2]->getNumber());
}

// Reordering works with the parameters flipped inthe gates:

void m2projtest::flippedconstruct()
{
    std::vector<UInt_t> xpars={1, 3, 5};
    std::vector<UInt_t> ypars={2, 4, 6};
    
    Float_t xpts[] = {0.0, 50.0, 50.0, 0.0};  // Contour is a square at origin
    Float_t ypts[] = {0.0, 0.0, 50.0, 50.0};  // 50x50 (roughly).
    CContour c1(2,1, 4, xpts, ypts);
    CContour c2(4,3, 4, xpts, ypts);
    CContour c3(6,5, 4, xpts, ypts);
    CGateContainer g1("c1", 1, c1);
    CGateContainer g2("c2", 2, c2);
    CGateContainer g3("c3", 3, c3);
    std::vector<CGateContainer*> gates = {&g3, &g2, &g1};  // Reversed order.
    
    CM2ProjectionL good("good", 0, xpars, ypars, gates, 128, 0.0, 127.0);
      
    std::vector<CGateContainer*> reordered = good.getRoiGates();
    EQ(UInt_t(1), reordered[0]->getNumber());
    EQ(UInt_t(2), reordered[1]->getNumber());
    EQ(UInt_t(3), reordered[2]->getNumber());
    
    // While we're at it, this is an X projection:
    
    ASSERT(good.isXprojection());
}

// Check that spectrum query operations return the right stuff:

void m2projtest::querymethods()
{
    std::unique_ptr<CM2ProjectionL> p(makeSpectrum()); // avoids leaks.
    
    // parameters [1,6] are used.
    
    for (int i =1; i < 7; i++) {
        ASSERT(p->UsesParameter(i));
    }
    // Parameters get returned in 1-6 order:
    
    std::vector<UInt_t> params;
    p->GetParameterIds(params);
    EQ(size_t(6), params.size());
    for (UInt_t i =0; i < 6; i++) {
        EQ(i+1, params[i]);
    }
    // only one resolution and it's 128.
    
    std::vector<UInt_t> res;
    p->GetResolutions(res);
    EQ(size_t(1), res.size());
    EQ(UInt_t(128  +  2), res[0]);    // Remember root has 2 extra channels.
    
    // Dimesion(0) is 130, others 1:
    
    EQ(Size_t(128 + 2), p->Dimension(0));
    EQ(Size_t(1), p->Dimension(1));
    EQ(UInt_t(1), p->Dimensionality());    // 1-d spectrum.
    
    ASSERT(!p->needParameter());
    EQ( ke2DmProj, p->getSpectrumType());
    EQ(Size_t(sizeof(UInt_t) * (128 + 2)), p->StorageNeeded());
}


// Check correct definition struct returned:

void m2projtest::getdef()
{
    std::unique_ptr<CM2ProjectionL> p(makeSpectrum());
    
    CSpectrum::SpectrumDefinition d = p->GetDefinition();
    EQ(std::string("good"), d.sName);
    EQ(UInt_t(0), d.nId);
    EQ(ke2DmProj, d.eType);
    EQ(keLong, d.eDataType);
    
    
    std::vector<UInt_t> sbXParams = {1, 3, 5};
    std::vector<UInt_t> sbYParams = {2, 4, 6};
    
    // Do asserts only if they differ:
    
    if (sbXParams != d.vParameters) {
        EQ(sbXParams.size(), d.vParameters.size());
        for (int i = 0; i < sbXParams.size(); i++) {
            EQ(sbXParams[i], d.vParameters[i]);
        }
    }
    if (sbYParams != d.vyParameters) {
        EQ(sbYParams.size(), d.vyParameters.size());
        for (int i = 0; i < sbYParams.size(); i++) {
            EQ(sbYParams[i], d.vyParameters[i]);
        }
    }
    
    EQ(size_t(1), d.nChannels.size());
    EQ(UInt_t(128 + 2), d.nChannels[0]);
    
    EQ(size_t(1), d.fLows.size());
    EQ(Float_t(0.0), d.fLows[0]);
    
    EQ(size_t(1), d.fHighs.size());
    EQ(Float_t(127.0), d.fHighs[0]);
    
}
// Test that set and [] are symmetric.

void m2projtest::setget()
{
    std::unique_ptr<CM2ProjectionL> p(makeSpectrum());
    
    UInt_t index = 100;
    p->set(&index, 111);
    EQ(ULong_t(111), (*p)[&index]);
}
// An empty event passed to Increment doesn't increment squat..

void m2projtest::emptyEvent()
{
    std::unique_ptr<CM2ProjectionL> p(makeSpectrum());
    
    CEvent e;
    p->Increment(e);
    
    ULong_t sum(0);
    for (UInt_t i =0; i < 128; i++) {
        sum += (*p)[&i];
    }
    EQ(ULong_t(0), sum);
}
// All increments in the same position, all inside gates.

void m2projtest::allInSame()
{
    std::unique_ptr<CM2ProjectionL> p(makeSpectrum());
    
    Double_t xpos = 12;         // This position gets incremented.
    CEvent e;
    e[1] = xpos;
    e[2] = 10;                // Yparam just has to be in the gate.
    e[3] = xpos;
    e[4] = 20;
    e[5] = xpos;
    e[6] = 30;
    
    p->Increment(e);
    
    UInt_t index = 12;
    EQ(ULong_t(3), (*p)[&index]);
}
// All points are ok  but in different channels:

void m2projtest::allOkDifferent()
{
    std::unique_ptr<CM2ProjectionL> p(makeSpectrum());
    
    Double_t xpos = 12;         // This position gets incremented.
    CEvent e;
    e[1] = xpos;
    e[2] = 10;                // Yparam just has to be in the gate.
    e[3] = xpos+1;
    e[4] = 20;
    e[5] = xpos+2;
    e[6] = 30;
    
    p->Increment(e);
    UInt_t index(12);
    EQ(ULong_t(1), (*p)[&index]);
    index++;
    EQ(ULong_t(1), (*p)[&index]);
    index++;
    EQ(ULong_t(1), (*p)[&index]);
    
}
// Some points are outside:

void m2projtest::someOutside()
{
    std::unique_ptr<CM2ProjectionL> p(makeSpectrum());
    
    Double_t xpos = 12;         // This position gets incremented.
    CEvent e;
    e[1] = xpos;
    e[2] = 10;                // Yparam just has to be in the gate.
    e[3] = xpos+1;
    e[4] = 101;               // outside.
    e[5] = xpos+2;
    e[6] = 30;
    
    p->Increment(e);
    UInt_t index(12);
    EQ(ULong_t(1), (*p)[&index]);
    index++;
    EQ(ULong_t(0), (*p)[&index]);
    index++;
    EQ(ULong_t(1), (*p)[&index]);
        
}

// All points outside the gate:

void m2projtest::allOutside()
{

    std::unique_ptr<CM2ProjectionL> p(makeSpectrum());
    
    Double_t xpos = 120;      // Outside gate.
    CEvent e;
    e[1] = xpos;
    e[2] = 10;                // Yparam just has to be in the gate.
    e[3] = xpos;
    e[4] = 20;
    e[5] = xpos;
    e[6] = 30;
    
    p->Increment(e);
    ULong_t sum(0);
    for (UInt_t i =0; i < 128; i++) {
        sum += (*p)[&i];
    }
    EQ(ULong_t(0), sum);
    
}

// Increment the y position (y projection)

void m2projtest::incrementFlip()
{
    std::unique_ptr<CM2ProjectionL> p(makeSpectrum());
    p->m_x = kfFALSE;           // Turn this into a y projection.
    
    Double_t xpos = 12;         // This position gets incremented.
    CEvent e;
    e[2] = xpos;
    e[1] = 10;                // Yparam just has to be in the gate.
    e[4] = xpos;
    e[3] = 20;
    e[6] = xpos;
    e[5] = 30;
    
    p->Increment(e);
    
    UInt_t index = 12;
    EQ(ULong_t(3), (*p)[&index]);    
}