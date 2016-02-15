

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/Asserter.h>

#include "Asserts.h"
#include "XamineShMemDisplayImpl.h"
#include "TestXamineShMem.h"
#include "Spectrum1DL.h"
#include "Parameter.h"
#include "Histogrammer.h"
#include "Cut.h"
#include "GateContainer.h"
#include <memory>
#include <exception>
#include <ostream>
#include <iterator>
#include <algorithm>

using namespace std;

template<class T>
ostream& operator<<(ostream& str, const vector<T>& vec) {

    str << "{";
    copy(vec.begin(), vec.end()-1, ostream_iterator<T>(str, ","));
    if (vec.begin() != vec.end()) {
        str << vec.back();
    }
    str << "}";

    return str;
}

class XamineShMemDisplayImplTests : public CppUnit::TestFixture
{
    CPPUNIT_TEST_SUITE(XamineShMemDisplayImplTests);
    CPPUNIT_TEST(addSpectrum_0);
    CPPUNIT_TEST(addSpectrum_1);
    CPPUNIT_TEST(removeSpectrum_0);
    CPPUNIT_TEST(updateStatistics_0);
    CPPUNIT_TEST(setTitle_0);
    CPPUNIT_TEST(setTitle_1);
    CPPUNIT_TEST(getAssociatedGates_0);

    CPPUNIT_TEST_SUITE_END();


private:
    std::shared_ptr<CTestXamineShMem> m_pShMem;
    std::unique_ptr<CXamineShMemDisplayImpl> m_pImpl;

public:
    void setUp() {
        m_pShMem.reset(new CTestXamineShMem);
        m_pImpl.reset(new CXamineShMemDisplayImpl( m_pShMem ) );
    }

    void tearDown() {

    }

    shared_ptr<CSpectrum> setUpSpectrum(CHistogrammer& sorter) {
        CParameter* pParam = sorter.AddParameter("param", 0, 1., 0, 256, "arb");

        std::shared_ptr<CSpectrum> pSpec(new CSpectrum1DL("testing123", 0, *pParam, 256)); // id = 0
        sorter.AddSpectrum(*pSpec);

        return pSpec;
    }

    void setUpGate(CHistogrammer& sorter, std::string name) {
        CCut cut(0, 1, 0);
        sorter.AddGate(name, 0, cut);
    }


    void addSpectrum_0() {

        // set up the hstigrammer and spectrum
        CHistogrammer sorter;
        auto pSpec = setUpSpectrum(sorter);

        m_pImpl->addSpectrum(*pSpec, sorter);

        const auto& boundSpectra = m_pShMem->boundSpectra();
        auto it = boundSpectra.find(pSpec.get());
        ASSERTMSG("Check that adding a spectrum to the shared memory does work",
                  boundSpectra.end() != it );
        EQMSG("Slot starts at 0", 0, it->second.s_slot);

        EQMSG("Info is set when spectrum added",
              string("testing123"), it->second.s_info );

        EQMSG("No gates are added for a spectrum that has no applied gates",
              size_t(0), m_pShMem->getGates().size());


    }

    void addSpectrum_1() {
        // set up the hstigrammer and spectrum
        CHistogrammer sorter;
        auto pSpec = setUpSpectrum(sorter);
        setUpGate(sorter, "testgate");

        m_pImpl->addSpectrum(*pSpec, sorter);

        const auto& boundSpectra = m_pShMem->boundSpectra();
        auto it = boundSpectra.find(pSpec.get());

        ASSERTMSG("Check that adding a spectrum to the shared memory does work",
                  boundSpectra.end() != it );
        EQMSG("Slot starts at 0", 0, it->second.s_slot);

        EQMSG("Info is set when spectrum added",
              string("testing123"), it->second.s_info );

        EQMSG("One gate added for a spectrum",
              size_t(1), m_pShMem->getGates().size());
    }

    void removeSpectrum_0() {
        // this is a straight delegation to the memory... no need to test
    }

    void addFit_0 () {
        // straight delegation... no logic to test
    }

    void deleteFit_0() {
        // straight delegation... no logic to test
    }

    void updateStatistics_0() {
        CHistogrammer sorter;
        auto pSpec = setUpSpectrum(sorter);

        m_pImpl->addSpectrum(*pSpec, sorter);

        pSpec->logUnderflow(0);

        pSpec->logOverflow(0);
        pSpec->logOverflow(0);

        m_pImpl->updateStatistics();

        auto spectra = m_pShMem->boundSpectra();
        auto it = spectra.find(pSpec.get());

        EQMSG("Underflows should be updated updateStatistics is called",
              vector<unsigned>({1}), it->second.s_underflows);

        EQMSG("Underflows should be updated updateStatistics is called",
          vector<unsigned>({2}), it->second.s_overflows);

    }

    void setTitle_0 () {
        CHistogrammer sorter;

        auto pSpec = setUpSpectrum(sorter);

        m_pImpl->addSpectrum(*pSpec, sorter);
        m_pImpl->setTitle("whatup", 0);

        auto spectra = m_pShMem->boundSpectra();
        auto it = spectra.find(pSpec.get());

        EQMSG("Set title for slot sets the info for correct slot",
              string("whatup"), it->second.s_info);

    }

    void setTitle_1 () {
        CHistogrammer sorter;

        auto pSpec = setUpSpectrum(sorter);

        m_pImpl->addSpectrum(*pSpec, sorter);
        m_pImpl->setTitle(*pSpec, "yeah");

        auto spectra = m_pShMem->boundSpectra();
        auto it = spectra.find(pSpec.get());

        EQMSG("Set title sets the info for correct slot",
              string("yeah"), it->second.s_info);
        EQMSG("Set title sets the info for correct slot",
              0, it->second.s_slot);

    }


    void getAssociatedGates_0 () {
        CHistogrammer sorter;

        auto pSpec = setUpSpectrum(sorter);
        setUpGate(sorter, "testgate");

        CGateContainer* pGate = sorter.FindGate("testgate");

        m_pImpl->addSpectrum(*pSpec, sorter);

        ASSERTMSG("Associated gates works in a simple fashion",
              vector<CGateContainer>({*pGate}) == m_pImpl->getAssociatedGates("testing123", sorter));
    }

};

CPPUNIT_TEST_SUITE_REGISTRATION(XamineShMemDisplayImplTests);
