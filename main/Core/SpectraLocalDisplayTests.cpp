
#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/Asserter.h>

#include <config.h>

#include <TestProcess.h>
#include "SpectraLocalDisplay.h"

#include <Asserts.h>

using namespace std;


class CSpectraTestDisplay : public Spectra::CSpectraLocalDisplay
{
private:
    bool m_alive;
public:

    CSpectraTestDisplay() : CSpectraLocalDisplay()
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
    CPPUNIT_TEST_SUITE_END();

private:

public:
    void setUp() {

    }

    void tearDown() {

    }


    void addSpectrum_0() {

    }

    void removeSpectrum_0() {

    }

    void spectrumBound_0() {

    }

    // we don't use spectcl side facilities for fit
    void addFit_0() {}

    // we don't use spectcl side facilities for fit
    void deleteFit_0() {}

    void addGate_0 () {

    }

    void removeGate_0() {

    }

    void getAssociatedGates_0() {

    }

    void createTitle_0() {

    }
};

CPPUNIT_TEST_SUITE_REGISTRATION(SpectraLocalDisplayTests);


