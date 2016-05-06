

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

#include "TabWorkspace.h"
#include "SpectrumView.h"
#include "QRootCanvas.h"
#include "WinFileWriter.h"
#include "HistogramBundle.h"
#include "HistogramList.h"
#include "TestSpecTclInterface.h"
#include "HistInfo.h"
#include "dispwind.h"
#include "Xamine.h"
#include "dispshare.h"
#include "client.h"

#include <QMutex>
#include <QString>
#include <QDebug>

#include <TH1.h>
#include <TH2.h>

#include <memory>
#include <cstdio>
#include <string>
#include <algorithm>

#include <sys/shm.h>
#include <sys/ipc.h>
#include <sys/wait.h>

using namespace std;

// these are needed for the win file parser
extern spec_shared* xamine_shared;
extern spec_shared* spectra;

class WinFileWriterTest : public CppUnit::TestFixture
{

private:
    std::shared_ptr<Viewer::SpecTclInterface> m_pInterface;
    win_db* m_pDatabase;
    int m_xamine1DSlot;
    int m_xamine2DSlot;

    volatile Xamine_shared* m_pSharedMemory;

public:

    CPPUNIT_TEST_SUITE(WinFileWriterTest);
    CPPUNIT_TEST(canvasDivision_0);
    CPPUNIT_TEST(canvasDivision_1);
    CPPUNIT_TEST(canvasContainsHists_0);
    CPPUNIT_TEST(canvasContainsHists_1);
    CPPUNIT_TEST(canvasTitle_0);
    CPPUNIT_TEST(canvasTitle_1);
    CPPUNIT_TEST(histRendition_0);
    CPPUNIT_TEST(histRendition_1);
    CPPUNIT_TEST_SUITE_END();

public:


    void setUp() {
        using namespace Viewer;

        m_pInterface = shared_ptr<SpecTclInterface>(new TestSpecTclInterface);

        TabWorkspace workspace(m_pInterface);
        workspace.setObjectName("test_workspace");

        HistogramList* pHistList = m_pInterface->getHistogramList();

        pHistList->addHist(createHist1D("asdf"));
        pHistList->addHist(createHist2D("lkjh"));

        // set up the view geometry and draw
        SpectrumView& view = workspace.getView();
        view.setGeometry(1,2);

        view.update();

        QRootCanvas* pCanvas = view.getCanvas(0,0);
        pCanvas->cd();
        pHistList->getHist("asdf")->draw();

        pCanvas = view.getCanvas(0,1);
        pCanvas->cd();
        pHistList->getHist("lkjh")->draw();

        WinFileWriter writer;
        writer.writeTab(workspace, ".test_win.win");

        // we need the shared memory because otherwise this does not work. the
        // win file parser expects the shared_memory to exist and be filled with
        // the appropriate stuff.
        setUpSharedMemory();

        m_pDatabase = new win_db;
        m_pDatabase->read(".test_win.win");

    }

    void tearDown() {
        delete m_pDatabase;
        m_pDatabase = nullptr;
        std::remove(".test_win.win");

        tearDownSharedMemory();

    }

    void setUpSharedMemory()
    {
        // I wanted to avoid doing this, but the win file parser
        // is very tightly coupled to the shared memory.
        Xamine_CreateSharedMemory(1024*1024, &m_pSharedMemory);
        Xamine_ManageMemory();
        Xamine_initspectra();

        // create two spectra with appropriate info so that the parser
        // doesn't bail when looking up spectrum names.
        Xamine_Allocate1d(&m_xamine1DSlot,10, "asdf", 1);
        Xamine_Allocate2d(&m_xamine2DSlot, 10, 10, "lkjh", 2);
    }

    void tearDownSharedMemory()
    {
        Xamine_FreeSpectrum(m_xamine1DSlot);
        Xamine_FreeSpectrum(m_xamine2DSlot);
        m_xamine1DSlot = -1;
        m_xamine2DSlot = -1;

        // Locate the id for our shared mem
        char name[5];
        key_t key = 0;

        Xamine_GetMemoryName(name);
        memcpy(reinterpret_cast<char*>(&key), name, sizeof(name) );
        int id = shmget(key, 1024*1024, 0);

        // detach - reverse the effects of Xamine_CreateSharedMemory
        Xamine_DetachSharedMemory();
        // detach - reverse the effects of Xamine_initspectra
        int stat = shmdt(spectra);

        // wait on the daemon that is trying to clean up the shared memory
        int childStatus = 0;
        wait( &childStatus );
        shmctl(id, IPC_RMID, 0);
    }


    std::unique_ptr<Viewer::HistogramBundle> createHist1D(const string& name) {

        using namespace Viewer;

        std::unique_ptr<TH1> pHist(new TH1F(name.c_str(),"", 10, 0, 10));
        std::unique_ptr<QMutex> pMutex(new QMutex);
        std::unique_ptr<HistogramBundle> pBundle(new HistogramBundle(move(pMutex), move(pHist),
        {name, 1, {"param"}, {{0, 10, 10}}, SpJs::Long}));

        return move(pBundle);
    }

    std::unique_ptr<Viewer::HistogramBundle> createHist2D(const string& name) {

        using namespace Viewer;

        std::unique_ptr<TH1> pHist(new TH2F(name.c_str(),"", 10, 0, 10, 10, 0, 10));
        std::unique_ptr<QMutex> pMutex(new QMutex);
        std::unique_ptr<HistogramBundle> pBundle(new HistogramBundle(move(pMutex), move(pHist),
        {name, 2, {"paramx", "paramy"}, {{0, 10, 10}, {0, 10, 10}}, SpJs::Long}));

    return move(pBundle);
}


protected:

    // BEGIN TESTS ------------------------------------------------------------

    void canvasDivision_0() {
        EQMSG("windb should have x dim = 2", 2, m_pDatabase->nx());
    }

    void canvasDivision_1() {
        EQMSG("windb should have y dim = 1", 1, m_pDatabase->ny());
    }


    void canvasContainsHists_0 () {
        EQMSG("pane (0,0) should contain correct hist",
              std::string("asdf"), m_pDatabase->getdef(0,0)->getSpectrumName());
    }
\
    void canvasContainsHists_1 () {
        EQMSG("pane (0,1) should contain correct hist",
              std::string("lkjh"), m_pDatabase->getdef(1,0)->getSpectrumName());
    }


    void canvasTitle_0 () {
        EQMSG("title of window is set", 1, m_pDatabase->hastitle());
    }

    void canvasTitle_1 () {
        char title[256];
        m_pDatabase->gettitle(title);

        QString qString(title);

        EQMSG("title of window should be same as tab workspace name",
              std::string("test_workspace"), qString.trimmed().toStdString());
    }

    void histRendition_0 () {
        win_1d* pAttr = dynamic_cast<win_1d*>(m_pDatabase->getdef(0,0));
        ASSERTMSG("1d histogram becomes win_1d in win_db", pAttr != nullptr);
        EQMSG("rendition of 1d is always histogram",
              histogram, pAttr->getrend());
    }

    void histRendition_1 () {
        win_2d* pAttr = dynamic_cast<win_2d*>(m_pDatabase->getdef(1,0));
        EQMSG("rendition of 2d is always color",
              color, pAttr->getrend());
    }

};

CPPUNIT_TEST_SUITE_REGISTRATION(WinFileWriterTest);

