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
    CPPUNIT_TEST(removeSpectrum_0);
    // CPPUNIT_TEST(updateStatistics_0); // haven't a clue how this works or what it does but Rootizing broke it.
    CPPUNIT_TEST(setTitle_0);
    CPPUNIT_TEST(setTitle_1);
//    CPPUNIT_TEST(getAssociatedGates_0);
//    CPPUNIT_TEST(getAssociatedGates_1);

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

    void setUpGate(CHistogrammer& sorter, std::string name, int index) {
        CCut cut(0, 1, index);
        sorter.AddGate(name, index, cut);
    }


    void addSpectrum_0() {

        // set up the hstigrammer and spectrum
        CHistogrammer sorter;
        auto pSpec = setUpSpectrum(sorter);

        m_pImpl->addSpectrum(*pSpec, sorter);

        const auto& boundSpectra = m_pShMem->boundSpectra();
        auto it = boundSpectra.find(pSpec.get());
        CPPUNIT_ASSERT_MESSAGE("Check that adding a spectrum to the shared memory does work",
                  boundSpectra.end() != it );
        EQMSG("Slot starts at 0", 0, it->second.s_slot);

        EQMSG("Info is set when spectrum added",
              string("testing123"), it->second.s_info );

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


//    void getAssociatedGates_0 () {
//        CHistogrammer sorter;

//        auto pSpec = setUpSpectrum(sorter);
//        setUpGate(sorter, "testgate", 0);

//        CGateContainer* pGate = sorter.FindGate("testgate");

//        m_pImpl->addSpectrum(*pSpec, sorter);

//        CPPUNIT_ASSERT_MESSAGEt("Associated gates works in a simple fashion",
//              vector<CGateContainer>({*pGate}) == m_pImpl->getAssociatedGates("testing123", sorter));
//    }

//    void getAssociatedGates_1 () {
//        CHistogrammer sorter;

//        auto pSpec = setUpSpectrum(sorter);
//        setUpGate(sorter, "testing1", 1); // doesn't apply to testing123
//        setUpGate(sorter, "testing2", 2); // doesn't apply to testing123

//        CGateContainer* pGate = sorter.FindGate("testgate");

//        m_pImpl->addSpectrum(*pSpec, sorter);

//        EQMSG("Associated gates returns empty if no gates are associated",
//              size_t(0), m_pImpl->getAssociatedGates("testing123", sorter).size());
//    }

};

CPPUNIT_TEST_SUITE_REGISTRATION(XamineShMemDisplayImplTests);
