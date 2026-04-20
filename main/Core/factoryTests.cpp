// Template for a test suite.

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/Asserter.h>
#include "Spectrum.h"

#define protected public            // acc setNextId
#define private public
#include <SpectrumFactory.h>
#undef protected
#undef private
#include "SpectrumFactoryException.h"
#include "DictionaryException.h"

#include "Parameter.h"
#include "GateContainer.h"
#include "TrueGate.h"
#include "OrGate.h"
#include "Contour.h"
#include <Asserts.h>
#include <vector>


class FactoryTests : public CppUnit::TestFixture {
  CPPUNIT_TEST_SUITE(FactoryTests);
  CPPUNIT_TEST(oddparams);
  CPPUNIT_TEST(badGateType);
  CPPUNIT_TEST(badSubGateCount);
  CPPUNIT_TEST(constructok);
  CPPUNIT_TEST(vec1d_1);
  CPPUNIT_TEST(vec1d_2);
  CPPUNIT_TEST(vec1d_3);
  CPPUNIT_TEST(vec1d_4);
  CPPUNIT_TEST(vec1d_5);
  CPPUNIT_TEST(vec1d_6);
  CPPUNIT_TEST(vec1d_7);
  CPPUNIT_TEST_SUITE_END();


protected:
  void oddparams();
  void badGateType();
  void badSubGateCount();
  void constructok();

  void vec1d_1();
  void vec1d_2();
  void vec1d_3();
  void vec1d_4();
  void vec1d_5();
  void vec1d_6();
  void vec1d_7();
private:
    CSpectrumFactory *pFact;
public:
  void setUp() {
    pFact = new CSpectrumFactory;         // By default exceptions are on.
    pFact->setNextId(0);
  }
  void tearDown() {
    CTreeParameterVector::ClearMap();
    delete pFact;
  }};

CPPUNIT_TEST_SUITE_REGISTRATION(FactoryTests);

// It's an error for there to be an odd numnber of params.

void FactoryTests::oddparams() {
    CParameter p("aname", 0, "Furlongs");
    std::vector<CParameter> params = {p};
    CTrueGate t;
    CGateContainer truegate("T", 1, t);
    
    CPPUNIT_ASSERT_THROW(
        pFact->CreateM2ProjectionSpectrum(
            "aname", keLong, params, &truegate, false, 128, 0, 127
        ),
        CSpectrumFactoryException
    );
    
    pFact->ExceptionMode(kfFALSE);
    
    ASSERT(!pFact->CreateM2ProjectionSpectrum(
        "aname", keLong, params, &truegate, false, 128, 0, 127
    ));
    
}

// It's an error for the gate not to contain a + gate.

void
FactoryTests::badGateType()
{
    CParameter px("aname", 1, "Furlongs");
    CParameter py("yname", 2, "Fortnights");
    std::vector<CParameter> params= {px, py};

    CTrueGate t;              // Must be a + gate.
    CGateContainer truegate("T", 1, t);
    
    CPPUNIT_ASSERT_THROW(
        pFact->CreateM2ProjectionSpectrum(
            "aname", keLong, params, &truegate, false, 128, 0, 127
        ),
        CSpectrumFactoryException
    );
    
    pFact->ExceptionMode(kfFALSE);
    ASSERT(!pFact->CreateM2ProjectionSpectrum(
        "aname", keLong, params, &truegate, false, 128, 0, 127
    ));
}

// Number of gates in the + gate must e the same as the number of paramter pairs.
// Note that this tests the ablity to catch exceptions thrown by the constructor
// and remap them into factory exceptions.
void
FactoryTests::badSubGateCount()
{
    CParameter px("aname", 1, "Furlongs");
    CParameter py("yname", 2, "Fortnights");
    std::vector<CParameter> params= {px, py};

    // For this just make an OR gate with two true gates:
    
    CTrueGate t;              // Must be a + gate.
    CGateContainer truegate("T", 1, t);
    std::vector<CGateContainer*> gates = {&truegate, &truegate};
    COrGate org(gates);
    CGateContainer orContainer("or", 2, org);
    
    
    CPPUNIT_ASSERT_THROW(
        pFact->CreateM2ProjectionSpectrum(
            "aname", keLong, params, &orContainer, false, 128, 0, 127
        ),
        CSpectrumFactoryException
    );
    
    pFact->ExceptionMode(kfFALSE);
    ASSERT(!pFact->CreateM2ProjectionSpectrum(
        "aname", keLong, params, &orContainer, false, 128, 0, 127
    ));
}    
    
// OK construction:

void
FactoryTests::constructok()
{
    CParameter px("xname", 1 ,"Furlongs");
    CParameter py("yname", 2, "Fortnights");
    std::vector<CParameter> params = {px, py};
    
    // Simple contour:
    
    Float_t xcoords[] = {10, 20, 20, 10};
    Float_t ycoords[] = {50, 50, 75, 75};
    CContour c(1, 2, 4, xcoords, ycoords);
    CGateContainer contour("contour", 1, c);
    std::vector<CGateContainer*> contours={&contour};
    COrGate orgate(contours);
    CGateContainer orContainer("or", 2, orgate);
    
    CSpectrum* pSpec;
    CPPUNIT_ASSERT_NO_THROW(
         pSpec = pFact->CreateM2ProjectionSpectrum(
            "aname", keLong, params, &orContainer, false, 128, 0, 127
        )
    );
    
    
    
    // Be sure the spectrum is right.
    
    std::vector<UInt_t> paramIds;  pSpec->GetParameterIds(paramIds);
    EQ(size_t(1), paramIds.size());
    EQ(py.getNumber(), paramIds[0]);
    
    EQ(UInt_t(1),     pSpec->Dimensionality());
    EQ(Size_t(128+2), pSpec->Dimension(0));
    
    EQ(ke2DmProj, pSpec->getSpectrumType());
}

void
FactoryTests::vec1d_1() {
    // Invalid data type:

    std::string name("testname");
    CTreeParameterVector param("testparam");
    std::vector<std::string> params; params.push_back(std::string("testparam"));
    std::vector<UInt_t> bins; bins.push_back(1024);
    std::vector<Float_t> lows; lows.push_back(0.0);
    std::vector<Float_t> highs; highs.push_back(1024.0);

    CPPUNIT_ASSERT_THROW(
        pFact->CreateSpectrum(
            name, ke1DVec, keByte, params, bins, &lows, &highs
        ), 
        CSpectrumFactoryException
    );
}
void FactoryTests::vec1d_2() {
    // Parameter name required:

    std::string name("testname");
    CTreeParameterVector param("testparam");
    std::vector<std::string> params;
    std::vector<UInt_t> bins; bins.push_back(1024);
    std::vector<Float_t> lows; lows.push_back(0.0);
    std::vector<Float_t> highs; highs.push_back(1024.0);

    CPPUNIT_ASSERT_THROW(
        pFact->CreateSpectrum(
            name, ke1DVec, keLong, params, bins, &lows, &highs
        ), 
        CSpectrumFactoryException
    );
}
void FactoryTests::vec1d_3() {
    // Need bins size =1.
    std::string name("testname");
    CTreeParameterVector param("testparam");
    std::vector<std::string> params; params.push_back(std::string("testparam"));
    std::vector<UInt_t> bins; bins.push_back(1024); bins.push_back(512);
    std::vector<Float_t> lows; lows.push_back(0.0);
    std::vector<Float_t> highs; highs.push_back(1024.0);

    CPPUNIT_ASSERT_THROW(
        pFact->CreateSpectrum(
            name, ke1DVec, keLong, params, bins, &lows, &highs
        ), 
        CSpectrumFactoryException
    );
}

void FactoryTests::vec1d_4() {
    // Both lows and highs must be non-null

    std::string name("testname");
    CTreeParameterVector param("testparam");
    std::vector<std::string> params; params.push_back(std::string("testparam"));
    std::vector<UInt_t> bins; bins.push_back(1024);
    std::vector<Float_t> lows; lows.push_back(0.0);
    std::vector<Float_t> highs; highs.push_back(1024.0);

    CPPUNIT_ASSERT_THROW(
        pFact->CreateSpectrum(
            name, ke1DVec, keLong, params, bins, nullptr, &highs
        ), 
        CSpectrumFactoryException
    );
    CPPUNIT_ASSERT_THROW(
        pFact->CreateSpectrum(
            name, ke1DVec, keLong, params, bins, &lows, nullptr
        ), 
        CSpectrumFactoryException
    );
}
void FactoryTests::vec1d_5() {
    // Low and high vectors must be 1 element only:

    std::string name("testname");
    CTreeParameterVector param("testparam");
    std::vector<std::string> params; params.push_back(std::string("testparam"));
    std::vector<UInt_t> bins; bins.push_back(1024);
    std::vector<Float_t> lows; lows.push_back(0.0);
    std::vector<Float_t> highs; highs.push_back(1024.0);

    lows.push_back(-1.0);
    CPPUNIT_ASSERT_THROW(
        pFact->CreateSpectrum(
            name, ke1DVec, keLong, params, bins, &lows, &highs
        ), 
        CSpectrumFactoryException
    );
    lows.pop_back();
    highs.push_back(2048.0);
    CPPUNIT_ASSERT_THROW(
        pFact->CreateSpectrum(
            name, ke1DVec, keLong, params, bins, &lows, &highs
        ), 
        CSpectrumFactoryException
    );

}

void FactoryTests::vec1d_6() {
    // parameter can't be found.

    std::string name("testname");
    
    std::vector<std::string> params; params.push_back(std::string("testparam"));
    std::vector<UInt_t> bins; bins.push_back(1024);
    std::vector<Float_t> lows; lows.push_back(0.0);
    std::vector<Float_t> highs; highs.push_back(1024.0);
    CPPUNIT_ASSERT_THROW(
        pFact->CreateSpectrum(
            name, ke1DVec, keLong, params, bins, &lows, &highs
        ),
        CDictionaryException
    );
}
void FactoryTests::vec1d_7() {
    // success:

    std::string name("testname");
    CTreeParameterVector param("testparam");
    std::vector<std::string> params; params.push_back(std::string("testparam"));
    std::vector<UInt_t> bins; bins.push_back(1024);
    std::vector<Float_t> lows; lows.push_back(0.0);
    std::vector<Float_t> highs; highs.push_back(1024.0);

    CSpectrum* pSpec;
    CPPUNIT_ASSERT_NO_THROW(
        pSpec = pFact->CreateSpectrum(
            name, ke1DVec, keLong, params, bins, &lows, &highs
        )
    );

    ASSERT(pSpec);

}