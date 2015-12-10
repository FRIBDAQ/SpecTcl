

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/Asserter.h>
#include <Asserts.h>

#include "DisplayInterface.h"
#include "Histogrammer.h"

class CFakeDisplayer : public CDisplayInterface {

public:

    CFakeDisplayer* clone() const { return new CFakeDisplayer(*this); }

    int operator==(const CDisplayInterface& rhs) { return true; }

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
    CHistogrammer* m_pHister;

public:
    void setUp() {

        m_pHister = new CHistogrammer(CFakeDisplayer());
    }

    void tearDown() {
        delete m_pHister;
    }


    void testConstruct_0 () {

    }

    void testAddSpectrum_0 () {

    }

    void testRemoveSpectrum_0 () {

    }

    void testAddParameter_0 () {

    }


    void testRemoveParameter_0 () {

    }

};


CPPUNIT_TEST_SUITE_REGISTRATION(TestHistogrammer);
