
#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/Asserter.h>

#include <config.h>

#include "CParameterDictionarySingleton.h"
#include <SpectrumPackage.h>
#include <DisplayInterface.h>
#include <Histogrammer.h>
#include <SpecTcl.h>
#include <TCLResult.h>
#include <TCLInterpreter.h>
#include <TCLObject.h>

#include <TestDisplay.h>
#include <Globals.h>
#include <SpecTclDisplayManager.h>

#include <Asserts.h>

#include <vector>
#include <string>
#include <iterator>
#include <sstream>

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
    CPPUNIT_TEST(deleteAll_0);
    CPPUNIT_TEST(deleteAll_1);
    CPPUNIT_TEST(deleteList_0);
    CPPUNIT_TEST(getChannel_0);
    CPPUNIT_TEST(setChannel_0);
    CPPUNIT_TEST(list_0);
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
        UInt_t bin[1] = {0};
        m_pSpec1->set(bin, 23);
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

        // Clean the dictionaries:

        CParameterDictionarySingleton::getInstance()->clear();
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
        
        // nbins +2 because root has its two bins (under/overflows)
        
        EQMSG("N bins correct", nbins[0]+2, resolution[0]);
        EQMSG("Low range correct", lows[0], pSpec->GetLow(0));
        EQMSG("High range correct", highs[0], pSpec->GetHigh(0));

    }

    void bindAll_0 () {
        CTCLResult result(m_pInterp);


        m_pPkg->BindAll(result);

        CDisplay* pDisplay = m_pDM->getCurrentDisplay();

        EQMSG("Bind all should succeed for first spectrum",
              true, pDisplay->spectrumBound(m_pSpec1));
        EQMSG("Bind all should succeed for second spectrum",
              true, pDisplay->spectrumBound(m_pSpec2));

    }


    void bindList_0 () {

        CTCLResult result(m_pInterp);

        std::vector<string> names(1);
        names[0] = "test1";
        m_pPkg->BindList(result, names);

        CDisplay* pDisplay = m_pDM->getCurrentDisplay();

        EQMSG("BindList(name) should bind spectra listed",
              true, pDisplay->spectrumBound(m_pSpec1));
        EQMSG("BindList(name) should not bind spectra that are not listed",
              false, pDisplay->spectrumBound(m_pSpec2));

    }

    void bindList_1 () {

        CTCLResult result(m_pInterp);

        std::vector<string> names(1);
        names[0] = "spectrumDoesNotExist";
        auto status = m_pPkg->BindList(result, names);

        EQMSG("BindList(name) should return TCL_ERROR when spectrum doesn't exist",
              TCL_ERROR, status);

    }

//    void bindList_1 () {

//        CTCLResult result(m_pInterp);

//        CSpectrum* pSpec1 = m_pSorter->FindSpectrum("test1");
//        if (pSpec1 == NULL) {
//            CPPUNIT_FAIL("Unable to find test1 spectrum");
//        }
//        std::vector<UInt_t> ids(1);
//        ids[0] = pSpec1->getNumber();
//        m_pPkg->BindList(result, ids);

//        CDisplay* pDisplay = m_pDM->getCurrentDisplay();

//        EQMSG("BindList(id) should bind spectra listed",
//              true, pDisplay->spectrumBound(*m_pSpec1));
//        EQMSG("BindList(id) should not bind spectra that are not listed",
//              false, pDisplay->spectrumBound(*m_pSpec2));

//    }

    void unBindList_0 () {
        CTCLResult result(m_pInterp);

        m_pPkg->BindAll(result);

        std::vector<string> names(1);
        names[0] = "test1";

        m_pPkg->UnbindList(result, names);

        CDisplay* pDisplay = m_pDM->getCurrentDisplay();

        EQMSG("unBindList(name) should unbind spectra listed",
              false, pDisplay->spectrumBound(m_pSpec1));
        EQMSG("unBindList(name) should not unbind spectra that are not listed",
              true, pDisplay->spectrumBound(m_pSpec2));
    }

    void unBindList_1 () {
        CTCLResult result(m_pInterp);

        m_pPkg->BindAll(result);

        std::vector<string> names(1);
        names[0] = "doesn't exist";

        auto status = m_pPkg->UnbindList(result, names);

        EQMSG("unBindList(name) should not return TCL_ERROR if spectrum doesn't exist",
              TCL_ERROR, status);
    }



    void deleteAll_0 () {

        CTCLResult result(m_pInterp);

        m_pPkg->BindAll(result);

        m_pPkg->DeleteAll();

        CDisplay* pDisplay = m_pDM->getCurrentDisplay();

        EQMSG("Deleting all should result in removal of first spectrum from display",
              false, pDisplay->spectrumBound(m_pSpec1));
        EQMSG("Deleting all should result in removal of second spectrum from display",
              false, pDisplay->spectrumBound(m_pSpec2));

        m_pSpec1 = nullptr;
        m_pSpec2 = nullptr;
    }



    void deleteAll_1 () {

        CTCLResult result(m_pInterp);

        m_pPkg->BindAll(result);

        m_pPkg->DeleteAll();

        CDisplay* pDisplay = m_pDM->getCurrentDisplay();

        EQMSG("Delete all should remove first spectrum from dictionary",
              (CSpectrum*)0, m_pSorter->FindSpectrum("test1"));
        EQMSG("Delete all should remove second spectrum from dictionary",
              (CSpectrum*)0, m_pSorter->FindSpectrum("test2"));

        // so that the tearDown method does not double free
        m_pSpec1 = nullptr;
        m_pSpec2 = nullptr;
    }

    void deleteList_0 () {

        CTCLResult result(m_pInterp);

        m_pPkg->BindAll(result);

        std::vector<string> names(1);
        names[0] = "test1";

        m_pPkg->DeleteList(result, names);

        CDisplay* pDisplay = m_pDM->getCurrentDisplay();

        EQMSG("DeleteList(name) should unbind spectra listed",
              false, pDisplay->spectrumBound(m_pSpec1));
        EQMSG("DeleteList(name) should remove the ",
             (CSpectrum*)0, m_pSorter->FindSpectrum("test1"));

        // to prevent tearDown() from double freeing
        m_pSpec1 = nullptr;
    }

    void getChannel_0() {

        CTCLResult result(m_pInterp);

        vector<UInt_t> indices = {0};
        m_pPkg->GetChannel(result, "test1", indices);

        EQMSG("GetChannel returns the correct value for a bin",
              int(23), int(result));

    }

    void setChannel_0() {
        CTCLResult result(m_pInterp);

        vector<UInt_t> indices = {0};
        ULong_t value = 25;
        Bool_t success = m_pPkg->SetChannel(result, "test1", indices, value);

        EQMSG("SetChannel must succeed for valid input", kfTRUE, success);
        EQMSG("SetChannel sets the correct value for specified bin",
              value, (*m_pSpec1)[indices.data()]);

    }
    void list_0() {
        // Demonstrator for Issue #87 - Create a gamma spectrum and
        // be sure the axis specifications are listed properly.

        // Make some parameters:

        auto api = SpecTcl::getInstance();
        for (int i = 0; i < 10; i++) {
            std::stringstream sname;
            sname << "p." << i;
            auto p = api->AddParameter(sname.str(), api->AssignParameterId(), std::string("mm"));
        }
        // Make the parameter array:

        std::vector<std::vector<std::string>> params = {
            {std::string("p.0")},
            {std::string("p.1"), std::string("p.2")},
            {std::string("p.3"), std::string("p.4"), std::string("p.5")},
            {std::string("p.6"), std::string("p.7"), std::string("p.8"), std::string("p.9")}
        };
        std::vector<Float_t> l = {0.0};
        std::vector<Float_t> h = {512.0};
        auto pSpec = api->CreateGammaSummary(std::string("gs"), keLong, params, UInt_t(512), &l, &h);
        ASSERT(pSpec);
        CPPUNIT_ASSERT_NO_THROW(api->AddSpectrum(*pSpec));

        std::vector<std::string> descriptions;
        CPPUNIT_ASSERT_NO_THROW(m_pPkg->ListSpectra(descriptions, "gs", false));
        EQ(size_t(1), descriptions.size());

        // Now parse the list, we need the 4'th element of the description list and to pick that apart.

        CTCLInterpreter interp;
        CTCLObject      d; d.Bind(interp);
        d = descriptions[0];
        EQ(6, d.llength());
        CTCLObject axes = d.lindex(4);  axes.Bind(interp);
        EQ(1, axes.llength());
        CTCLObject axis = axes.lindex(0); axis.Bind(interp);
        EQ(3, axis.llength());
        CTCLObject low = axis.lindex(0);  low.Bind(interp);
        CTCLObject hi = axis.lindex(1);   hi.Bind(interp);
        CTCLObject bins = axis.lindex(2);  bins.Bind(interp);


        EQ(0.0, (double)(low));
        EQ(512.0, (double(hi)));
        EQ(512, (int)bins);

    }


};

CPPUNIT_TEST_SUITE_REGISTRATION(SpectrumPackageTests);

