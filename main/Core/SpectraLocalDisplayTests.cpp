//    This software is Copyright by the Board of Trustees of Michigan
//    State University (c) Copyright 2016.
//
//    This program is free software: you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation, either version 3 of the License, or
//    any later version.
//
//    This program is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License
//    along with this program.  If not, see <http://www.gnu.org/licenses/>.
//
//    Authors:
//    Jeromy Tompkins
//    NSCL
//    Michigan State University
//    East Lansing, MI 48824-1321

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


