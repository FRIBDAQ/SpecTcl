
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
#include "RootFileWriter.h"
#include "HistogramBundle.h"
#include "HistogramList.h"
#include "TestSpecTclInterface.h"
#include "HistInfo.h"

#include <QMutex>
#include <QString>

#include <TFile.h>
#include <TCanvas.h>

#include <memory>
#include <cstdio>
#include <string>

using namespace std;

class RootFileWriterTest : public CppUnit::TestFixture
{

private:
    std::shared_ptr<Viewer::SpecTclInterface> m_pInterface;
    TFile* m_pFile;

public:

    CPPUNIT_TEST_SUITE(RootFileWriterTest);
    CPPUNIT_TEST(canvasName_0);
    CPPUNIT_TEST_SUITE_END();

public:
    void setUp() {
        using namespace Viewer;

        m_pInterface = shared_ptr<SpecTclInterface>(new TestSpecTclInterface);

        TabWorkspace workspace(m_pInterface);
        workspace.setObjectName("test_workspace");

        HistogramList* pHistList = m_pInterface->getHistogramList();

        pHistList->addHist(createHist("asdf"));
        pHistList->addHist(createHist("lkjh"));

        // set up the view geometry and draw
        SpectrumView& view = workspace.getView();
        view.setGeometry(1,2);

        view.setCurrentCanvas(view.getCanvas(0,0));
        pHistList->getHist("asdf")->draw();

        view.setCurrentCanvas(view.getCanvas(0,1));
        pHistList->getHist("lkjh")->draw();


        RootFileWriter writer;
        writer.openFile(".testFile.root", "RECREATE");
        writer.writeTab(workspace, true);
        writer.closeFile();

        m_pFile = new TFile(".testFile.root", "READ");

    }

    void tearDown() {

        //m_pFile->Close();

        remove(".testFile.root");
    }

    std::unique_ptr<Viewer::HistogramBundle> createHist(const string& name) {

        using namespace Viewer;

        std::unique_ptr<TH1> pHist(new TH1F(name.c_str(),"", 10, 0, 10));
        std::unique_ptr<QMutex> pMutex(new QMutex);
        std::unique_ptr<HistogramBundle> pBundle(new HistogramBundle(move(pMutex), move(pHist),
        {name, 1, {"param"}, {{0, 10, 10}}, SpJs::Long}));

        return move(pBundle);
    }


protected:

    void canvasName_0() {
        TCanvas* pCanvas;

        m_pFile->GetObject("test_workspace", pCanvas);
        ASSERTMSG("Canvas should have name of tab and be findable", pCanvas != nullptr);

    }

};

CPPUNIT_TEST_SUITE_REGISTRATION(RootFileWriterTest);

