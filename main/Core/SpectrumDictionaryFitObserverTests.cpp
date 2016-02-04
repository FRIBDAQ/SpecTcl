
#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/Asserter.h>

#include <config.h>

#include "Histogrammer.h"
#include "SpectrumFactory.h"
#include "CSpectrumFit.h"
#include "CLinearFit.h"
#include "CFitDictionary.h"

#include "SpectrumDictionaryFitObserver.h"
#include "SpecTcl.h"
#include "Globals.h"

#include "Asserts.h"

#include <vector>
#include <string>

using namespace std;

class SpectrumDictionaryFitObserverTests : public CppUnit::TestFixture
{
    CPPUNIT_TEST_SUITE(SpectrumDictionaryFitObserverTests);
    CPPUNIT_TEST(deleteListRemovesFits_0);
    CPPUNIT_TEST_SUITE_END();

private:
    CHistogrammer* m_pSorter;
    SpectrumDictionaryFitObserver* m_pObserver;

public:
    void setUp() {
        m_pSorter = new CHistogrammer;
        gpEventSink = m_pSorter;

        createSpectra();

        m_pObserver = new SpectrumDictionaryFitObserver;

        SpecTcl& api = *(SpecTcl::getInstance());
        api.addSpectrumDictionaryObserver(m_pObserver);

    }

    void createSpectra() {
        SpecTcl& api = *(SpecTcl::getInstance());

        CParameter* pParam = api.AddParameter("test.0", 0, 1, 0, 100, "arb");
        CSpectrum* pSpec = api.Create1D("test1", keLong,
                                         *pParam, 100, 0, 100);
        api.AddSpectrum(*pSpec);

        api.addSpectrumDictionaryObserver(new SpectrumDictionaryFitObserver);

    }

    void tearDown() {
        delete m_pSorter;
        delete m_pObserver;
    }

    void deleteListRemovesFits_0 () {

        SpecTcl& api = *(SpecTcl::getInstance());

        CLinearFit func("linear");
        CSpectrumFit* pFit = new CSpectrumFit("test1", 0, 0, 10, func);
        CFitDictionary& dict = CFitDictionary::getInstance();
        dict.add(*pFit); // dict takes ownership

        api.RemoveSpectrum("test1");

        ASSERT(dict.find("linear") == dict.end());
    }

}; // end class

CPPUNIT_TEST_SUITE_REGISTRATION(SpectrumDictionaryFitObserverTests);
