
#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/Asserter.h>

#include <config.h>

#include <SpectrumPackage.h>
#include <DisplayInterface.h>
#include <Histogrammer.h>
#include <SpecTcl.h>
#include <TCLResult.h>
#include <TestDisplay.h>
#include <Globals.h>
#include <SpecTclDisplayManager.h>

#include <Asserts.h>

#include <vector>
#include <string>
#include <iterator>

using namespace std;

class SpectrumPackageTests : public CppUnit::TestFixture
{
    CPPUNIT_TEST_SUITE(SpectrumPackageTests);
    CPPUNIT_TEST(create_0);
    CPPUNIT_TEST(bindAll_0);
    CPPUNIT_TEST(bindList_0);
    CPPUNIT_TEST(bindList_1);
    CPPUNIT_TEST(unBindList_0);
    CPPUNIT_TEST(unBindList_1);
    CPPUNIT_TEST_SUITE_END();

private:
    CSpectrumPackage* m_pPkg;
    CHistogrammer*    m_pSorter;
    CTCLInterpreter*  m_pInterp;
    CDisplayInterface*  m_pDM;
    CSpectrum*        m_pSpec1;
    CSpectrum*        m_pSpec2;

public:
    void setUp() {
        m_pInterp = new CTCLInterpreter();
        m_pSorter = new CHistogrammer;
        gpEventSink = m_pSorter;
        m_pDM     = new CSpecTclDisplayInterface;
        m_pDM->getFactory().addCreator("test", *(new CTestDisplayCreator));
        m_pDM->createDisplay("test", "test");
        m_pDM->setCurrentDisplay("test");
        gpDisplayInterface = m_pDM;
        m_pPkg    = new CSpectrumPackage(m_pInterp, m_pSorter, m_pDM);

        createSpectra();

    }

    void createSpectra() {
        CTCLResult result(m_pInterp, true);

        CParameter* pParam = m_pSorter->AddParameter("test.0", 0, 1, 0, 100., "arb");


        vector<string> params(1); params[0] = "test.0";
        UInt_t nbins = 100;
        Float_t low = 0;
        Float_t high = 100;

        SpecTcl* pApi = SpecTcl::getInstance();

        m_pSpec1 = pApi->Create1D("test1", keLong,
                                  *pParam, nbins, low, high);
        m_pSpec2 = pApi->Create1D("test2", keLong,
                                  *pParam, nbins, low, high);
        pApi->AddSpectrum(*m_pSpec1);
        pApi->AddSpectrum(*m_pSpec2);
    }

    void tearDown() {
        delete m_pPkg;
        delete m_pSorter;
        delete m_pInterp;
        delete m_pDM;
        delete m_pSpec1;
        delete m_pSpec2;
    }


    void create_0 () {
        CTCLResult result(m_pInterp, true);

        vector<string> params(1); params[0] = "test.0";
        vector<UInt_t> nbins(1); nbins[0] = 100;
        vector<Float_t> lows(1); lows[0] = 0;
        vector<Float_t> highs(1); highs[0] = 100;

        m_pPkg->CreateSpectrum(result, "unique", "1",
                               params, nbins, lows, highs, "long");
        EQMSG("Spectrum creation succeeded", string("unique"), result.getString());

        SpecTcl*    pApi = SpecTcl::getInstance();
        CSpectrum* pSpec = pApi->FindSpectrum("unique");
        std::vector<UInt_t> resolution;
        pSpec->GetResolutions(resolution);
        EQMSG("N bins correct", nbins[0], resolution[0]);
        EQMSG("Low range correct", lows[0], pSpec->GetLow(0));
        EQMSG("High range correct", highs[0], pSpec->GetHigh(0));

    }

    void bindAll_0 () {
        CTCLResult result(m_pInterp);


        m_pPkg->BindAll(result);

        CDisplay* pDisplay = m_pDM->getCurrentDisplay();

        EQMSG("Bind all should succeed for first spectrum",
              true, pDisplay->spectrumBound(*m_pSpec1));
        EQMSG("Bind all should succeed for second spectrum",
              true, pDisplay->spectrumBound(*m_pSpec2));

    }


    void bindList_0 () {

        CTCLResult result(m_pInterp);

        std::vector<string> names(2);
        names[0] = "test1";
        m_pPkg->BindList(result, names);

        CDisplay* pDisplay = m_pDM->getCurrentDisplay();

        EQMSG("BindList(name) should bind spectra listed",
              true, pDisplay->spectrumBound(*m_pSpec1));
        EQMSG("BindList(name) should not bind spectra that are not listed",
              false, pDisplay->spectrumBound(*m_pSpec2));

    }

    void bindList_1 () {

        CTCLResult result(m_pInterp);

        CSpectrum* pSpec1 = m_pSorter->FindSpectrum("test1");
        if (pSpec1 == NULL) {
            CPPUNIT_FAIL("Unable to find test1 spectrum");
        }
        std::vector<UInt_t> ids(1);
        ids[0] = pSpec1->getNumber();
        m_pPkg->BindList(result, ids);

        CDisplay* pDisplay = m_pDM->getCurrentDisplay();

        EQMSG("BindList(id) should bind spectra listed",
              true, pDisplay->spectrumBound(*m_pSpec1));
        EQMSG("BindList(id) should not bind spectra that are not listed",
              false, pDisplay->spectrumBound(*m_pSpec2));

    }

    void unBindList_0 () {
        CTCLResult result(m_pInterp);

        m_pPkg->BindAll(result);

        std::vector<string> names(1);
        names[0] = "test1";

        m_pPkg->UnbindList(result, names);

        CDisplay* pDisplay = m_pDM->getCurrentDisplay();

        EQMSG("unBindList(name) should unbind spectra listed",
              false, pDisplay->spectrumBound(*m_pSpec1));
        EQMSG("unBindList(name) should not unbind spectra that are not listed",
              true, pDisplay->spectrumBound(*m_pSpec2));
    }

    void unBindList_1 () {
        CTCLResult result(m_pInterp);

        m_pPkg->BindAll(result);

        CSpectrum* pSpec1 = m_pSorter->FindSpectrum("test1");
        std::vector<UInt_t> ids(1);
        ids[0] = pSpec1->getNumber();

        m_pPkg->UnbindList(result, ids);

        CDisplay* pDisplay = m_pDM->getCurrentDisplay();

        EQMSG("unBindList(id) should unbind spectra listed",
              false, pDisplay->spectrumBound(*m_pSpec1));
        EQMSG("unBindList(id) should not unbind spectra that are not listed",
              true, pDisplay->spectrumBound(*m_pSpec2));

    }

};

CPPUNIT_TEST_SUITE_REGISTRATION(SpectrumPackageTests);

