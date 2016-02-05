

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/Asserter.h>
#include <Asserts.h>

#include "Histogrammer.h"
#include "GatingObserver.h"
#include "Spectrum1DL.h"
#include "FalseGate.h"

#include <vector>
#include <string>
#include <sstream>

using namespace std;


class CTestObserver : public CGatingObserver
{
private:
    std::vector<std::string> m_record;

public:
    void onApply(const CGateContainer &rGate, CSpectrum &rSpectrum, CHistogrammer&) {
        ostringstream record;
        record << "apply:" << rGate.getName() << ":" << rSpectrum.getName();
        m_record.push_back(record.str());
    }

    void onRemove(const CGateContainer &rGate, CSpectrum &rSpectrum, CHistogrammer &rSorter) {}

    std::vector<std::string> getRecord() { return m_record; }
};


class TestHistogrammer : public CppUnit::TestFixture {

    CPPUNIT_TEST_SUITE(TestHistogrammer);
    CPPUNIT_TEST(testConstruct_0);
    CPPUNIT_TEST(testAddSpectrum_0);
    CPPUNIT_TEST(testRemoveSpectrum_0);
    CPPUNIT_TEST(testAddParameter_0);
    CPPUNIT_TEST(testRemoveParameter_0);
    CPPUNIT_TEST(applyGate_0);
    CPPUNIT_TEST(ungate_0);
    CPPUNIT_TEST_SUITE_END();


private:
    CParameter* m_pParam;
    CHistogrammer* m_pSorter;
    CTestObserver* m_pGatingObserver;

public:
    void setUp() {
        m_pParam = new CParameter(1, "test.0",0, 0, 1, "arb");

        m_pSorter = new CHistogrammer();
        m_pGatingObserver = new CTestObserver();
        m_pSorter->addGatingObserver(m_pGatingObserver);

    }

    void tearDown() {
        delete m_pSorter;
        delete m_pParam;
        delete m_pGatingObserver;
    }

    void testConstruct_0 () {
        // this is intentionally blank
    }

    CSpectrum* addSpectrum(string name) {

        CSpectrum* pSpec = new CSpectrum1DL(name, 0, *m_pParam, 100);
        m_pSorter->AddSpectrum(*pSpec);
        return pSpec;
    }

    CGateContainer* addGate(string name) {
        CGate* pGate = new CFalseGate();
        m_pSorter->AddGate(name, 0, *pGate);
        return m_pSorter->FindGate(0);
    }

    void testAddSpectrum_0 () {

        CSpectrum* pSpec = addSpectrum("test");
        EQMSG("Spectrum add should be findable", pSpec, m_pSorter->FindSpectrum("test"));
    }

    void testRemoveSpectrum_0 () {
        addSpectrum("test");

        m_pSorter->RemoveSpectrum("test");

        EQMSG("Spectrum should not exist after remove", (CSpectrum*)0, m_pSorter->FindSpectrum("test"));
    }

    void testAddParameter_0 () {
        m_pSorter->AddParameter("test.1", 1, 1, 0, 100, "arb");

        CParameter* pParam = m_pSorter->FindParameter("test.1");
        EQMSG("Parameter name comparison", string("test.1"), pParam->getName());
        EQMSG("Parameter id comparison", UInt_t(1), pParam->getNumber());
        EQMSG("Parameter scale comparison", UInt_t(1), pParam->getScale());
        EQMSG("Parameter low comparison", Float_t(0), pParam->getLow());
        EQMSG("Parameter high comparison", Float_t(100), pParam->getHigh());
        EQMSG("Parameter high comparison", string("arb"), pParam->getUnits());
    }


    void testRemoveParameter_0 () {
        m_pSorter->AddParameter("test.1", 1, 1, 0, 100, "arb");
        m_pSorter->RemoveParameter("test.1");

        CParameter* pParam = m_pSorter->FindParameter("test.1");
        EQMSG("Removing param should make it not findable", (CParameter*)0, pParam);
    }

    void applyGate_0 () {
        CSpectrum* pSpec = addSpectrum("test");
        CGateContainer* pGate = addGate("gate");

        m_pSorter->ApplyGate("gate", "test");

        std::vector<std::string> record = m_pGatingObserver->getRecord();
        EQ(size_t(1), record.size());
        EQMSG("Applying gate to histogrammer should trigger observer onApply",
              std::string("apply:gate:test"), record.at(0));

        delete pSpec;
    }

    void ungate_0 () {
        CSpectrum* pSpec = addSpectrum("test");
        CGateContainer* pGate = addGate("gate");

        m_pSorter->ApplyGate("gate", "test");

        m_pSorter->UnGate("test");

        std::vector<std::string> record = m_pGatingObserver->getRecord();
        EQ(size_t(2), record.size());
        EQMSG("Ungate calls to histogrammer should trigger observer onApply with -Ungated-",
              std::string("apply:-Ungated-:test"), record.at(1));

        delete pSpec;
    }

};


CPPUNIT_TEST_SUITE_REGISTRATION(TestHistogrammer);
