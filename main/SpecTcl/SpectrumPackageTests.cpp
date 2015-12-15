
#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/Asserter.h>

#include <config.h>

#include <SpectrumPackage.h>
#include <DisplayManager.h>
#include <Histogrammer.h>
#include <SpecTcl.h>
#include <TCLResult.h>

#include <Asserts.h>

#include <vector>
#include <string>

using namespace std;

extern CHistogrammer* gpEventSink;

class SpectrumPackageTests : public CppUnit::TestFixture
{
    CPPUNIT_TEST_SUITE(SpectrumPackageTests);
    CPPUNIT_TEST(create_0);
//    CPPUNIT_TEST(bindAll_0);
    CPPUNIT_TEST_SUITE_END();

private:
    CSpectrumPackage* m_pPkg;
    CHistogrammer*    m_pSorter;
    CTCLInterpreter*  m_pInterp;
    CDisplayManager*  m_pDM;

public:
    void setUp() {
        m_pInterp = new CTCLInterpreter();
        m_pSorter = new CHistogrammer;
        gpEventSink = m_pSorter;
        m_pDM     = new CDisplayManager;
        m_pPkg    = new CSpectrumPackage(m_pInterp, m_pSorter, m_pDM);
    }

    void tearDown() {
        delete m_pPkg;
        delete m_pSorter;
        delete m_pInterp;
        delete m_pDM;
    }


    void create_0 () {
        CTCLResult result(m_pInterp, true);

        m_pSorter->AddParameter("test.0", 0, 1, 0, 100., "arb");

        vector<string> params(1); params[0] = "test.0";
        vector<UInt_t> nbins(1); nbins[0] = 100;
        vector<Float_t> lows(1); lows[0] = 0;
        vector<Float_t> highs(1); highs[0] = 100;

        m_pPkg->CreateSpectrum(result, "test", "1",
                               params, nbins, lows, highs, "long");
        EQMSG("Spectrum creation succeeded", string("test"), result.getString());

        SpecTcl*    pApi = SpecTcl::getInstance();
        CSpectrum* pSpec = pApi->FindSpectrum("test");
        std::vector<UInt_t> resolution;
        pSpec->GetResolutions(resolution);
        EQMSG("N bins correct", nbins[0], resolution[0]);
        EQMSG("Low range correct", lows[0], pSpec->GetLow(0));
        EQMSG("High range correct", highs[0], pSpec->GetHigh(0));

    }

    void bindAll_0 () {
       CTCLResult result(m_pInterp, true);
    }
};

CPPUNIT_TEST_SUITE_REGISTRATION(SpectrumPackageTests);

