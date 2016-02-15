

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

using namespace std;

class XamineShMemDisplayImplTests : public CppUnit::TestFixture
{
    CPPUNIT_TEST_SUITE(XamineShMemDisplayImplTests);
    CPPUNIT_TEST(addSpectrum_0);
    CPPUNIT_TEST(addSpectrum_1);
    CPPUNIT_TEST(removeSpectrum_0);
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

    void setUpSpectrum(CHistogrammer& sorter) {
        CParameter* pParam = sorter.AddParameter("param", 0, 1., 0, 256, "arb");

        CSpectrum1DL spec("testing123", 0, *pParam, 256); // id = 0
        sorter.AddSpectrum(spec);

    }

    void setUpGate(CHistogrammer& sorter) {
        CCut cut(0, 1, 0);
        sorter.AddGate("testgate", 0, cut);
    }


    void addSpectrum_0() {

        // set up the hstigrammer and spectrum
        CHistogrammer sorter;
        setUpSpectrum(sorter);

        m_pImpl->addSpectrum(spec, sorter);

        const auto& boundSpectra = m_pShMem->boundSpectra();
        auto it = boundSpectra.find(&spec);
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
        setUpSpectrum(sorter);
        setUpGate(sorter);

        m_pImpl->addSpectrum(spec, sorter);

        const auto& boundSpectra = m_pShMem->boundSpectra();
        auto it = boundSpectra.find(&spec);

        ASSERTMSG("Check that adding a spectrum to the shared memory does work",
                  boundSpectra.end() != it );
        EQMSG("Slot starts at 0", 0, it->second.s_slot);

        EQMSG("Info is set when spectrum added",
              string("testing123"), it->second.s_info );

        EQMSG("One gate added for a spectrum",
              size_t(1), m_pShMem->getGates().size());
    }

    void removeSpectrum_0() {

    }

};

CPPUNIT_TEST_SUITE_REGISTRATION(XamineShMemDisplayImplTests);
