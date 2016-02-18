
#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/Asserter.h>

#include <config.h>

#include <Spectrum1DL.h>
#include <Parameter.h>
#include "SpectraLocalDisplay.h"
#include "TestXamineShMem.h"
#include "SpecTcl.h"

#include <Asserts.h>
#include <memory>

using namespace std;


class CSpectraTestDisplay : public Spectra::CSpectraLocalDisplay
{
private:
    bool m_alive;
public:

    CSpectraTestDisplay(shared_ptr<CXamineSharedMemory> pMem)
        : CSpectraLocalDisplay(pMem, *SpecTcl::getInstance())
    {}

    void start() {
        m_alive = true;
    }

    void restart() {
        stop();
        start();
    }

    void stop() {
        m_alive = false;
    }


};

class SpectraLocalDisplayTests : public CppUnit::TestFixture
{
    CPPUNIT_TEST_SUITE(SpectraLocalDisplayTests);
    CPPUNIT_TEST(createTitle_0);
    CPPUNIT_TEST_SUITE_END();

private:

    unique_ptr<Spectra::CSpectraLocalDisplay> m_pDisp;

public:
    void setUp() {
        m_pDisp.reset(new CSpectraTestDisplay (
                          shared_ptr<CXamineSharedMemory>(new CTestXamineShMem)));
    }

    void createTitle_0() {

        CSpectrum1DL spectrum("test", 0, CParameter(), 16);

        EQMSG("createTitle returns name of spectrum",
              string("test"),
              m_pDisp->createTitle(spectrum, 0,*(SpecTcl::getInstance()->GetHistogrammer())));

    }

};

CPPUNIT_TEST_SUITE_REGISTRATION(SpectraLocalDisplayTests);


