

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/Asserter.h>
#include <Asserts.h>

#include "Display.h"
#include "Histogrammer.h"
#include "Spectrum1DL.h"

#include <vector>
#include <string>


using namespace std;


class CFakeDisplayer : public CDisplay {

public:

    CFakeDisplayer* clone() const { return new CFakeDisplayer(*this); }

    int operator==(const CDisplay& rhs) { return true; }

    void Start() {};
    void Stop() {};
    Bool_t isAlive() { return true; }
    volatile Xamine_shared* getXamineMemory() const { return NULL; }
    Address_t DefineSpectrum(CXamineSpectrum& rSpectrum) {return NULL; }
    void setInfo(std::string name, UInt_t slot) {}
    void setTitle(std::string name, UInt_t slot) {}
    UInt_t getTitleSize() const {return 0;}
    void EnterGate(CDisplayGate& rGate) {}
    CXamineGates* GetGates(UInt_t nSpectrum) { return NULL; }
    void RemoveGate(UInt_t, UInt_t, GateType_t) {}
    void FreeSpectrum(UInt_t) {}
    void setOverflows(unsigned, unsigned, unsigned) {}
    void setUnderflows(unsigned, unsigned, unsigned) {}
    UInt_t GetEventFd() {};
};

class TestHistogrammer : public CppUnit::TestFixture {

    CPPUNIT_TEST_SUITE(TestHistogrammer);
    CPPUNIT_TEST(testConstruct_0);
    CPPUNIT_TEST(testAddSpectrum_0);
    CPPUNIT_TEST(testRemoveSpectrum_0);
    CPPUNIT_TEST(testAddParameter_0);
    CPPUNIT_TEST(testRemoveParameter_0);
    CPPUNIT_TEST_SUITE_END();


private:
    CParameter* m_pParam;
    CHistogrammer* m_pHister;

public:
    void setUp() {
        m_pParam = new CParameter(1, "test.0",0, 0, 1, "arb");

        m_pHister = new CHistogrammer(CFakeDisplayer());
    }

    void tearDown() {
        delete m_pHister;
        delete m_pParam;
    }

    void testConstruct_0 () {
        // this is intentionally blank
    }

    CSpectrum* addSpectrum(string name) {

        CSpectrum* pSpec = new CSpectrum1DL(name, 0, *m_pParam, 100);
        m_pHister->AddSpectrum(*pSpec);
        return pSpec;
    }

    void testAddSpectrum_0 () {

        CSpectrum* pSpec = addSpectrum("test");
        EQMSG("Spectrum add should be findable", pSpec, m_pHister->FindSpectrum("test"));
    }

    void testRemoveSpectrum_0 () {
        addSpectrum("test");

        m_pHister->RemoveSpectrum("test");

        EQMSG("Spectrum should not exist after remove", (CSpectrum*)0, m_pHister->FindSpectrum("test"));
    }

    void testAddParameter_0 () {
        m_pHister->AddParameter("test.1", 1, 1, 0, 100, "arb");

        CParameter* pParam = m_pHister->FindParameter("test.1");
        EQMSG("Parameter name comparison", string("test.1"), pParam->getName());
        EQMSG("Parameter id comparison", UInt_t(1), pParam->getNumber());
        EQMSG("Parameter scale comparison", UInt_t(1), pParam->getScale());
        EQMSG("Parameter low comparison", Float_t(0), pParam->getLow());
        EQMSG("Parameter high comparison", Float_t(100), pParam->getHigh());
        EQMSG("Parameter high comparison", string("arb"), pParam->getUnits());
    }


    void testRemoveParameter_0 () {
        m_pHister->AddParameter("test.1", 1, 1, 0, 100, "arb");
        m_pHister->RemoveParameter("test.1");

        CParameter* pParam = m_pHister->FindParameter("test.1");
        EQMSG("Removing param should make it not findable", (CParameter*)0, pParam);
    }

};


CPPUNIT_TEST_SUITE_REGISTRATION(TestHistogrammer);
