

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
#include "RootFileReader.h"
#include "HistogramBundle.h"
#include "HistogramList.h"
#include "TestSpecTclInterface.h"
#include "HistInfo.h"
#include "ParameterDb.h"
#include "Parameters.h"

#include <QMutex>
#include <QString>
#include <QDebug>

#include <TFile.h>
#include <TCanvas.h>
#include <TH1.h>
#include <TH2.h>
#include <TCutG.h>

#include <memory>
#include <cstdio>
#include <string>

using namespace std;

class RootFileReaderTest : public CppUnit::TestFixture
{

  private:
    std::shared_ptr<Viewer::SpecTclInterface> m_pInterface;
    std::vector<Viewer::TabWorkspace*>        m_workspaces;

  public:

    CPPUNIT_TEST_SUITE(RootFileReaderTest);
//    CPPUNIT_TEST(numberOfTabs_0);
//    CPPUNIT_TEST(numberOfColumns_0);
//    CPPUNIT_TEST(numberOfRows_0);
//    CPPUNIT_TEST(histCreation_0);
//    CPPUNIT_TEST(location_0);
//    CPPUNIT_TEST(location_1);
//    CPPUNIT_TEST(location_2);
//    CPPUNIT_TEST(location_3);
//    CPPUNIT_TEST(location_4);
    CPPUNIT_TEST_SUITE_END();

  public:
    void setUp() {
      using namespace Viewer;

      // create the file and its content
      createTFile(".testInputFile.root");

      // open the new file
      TFile file(".testInputFile.root", "READ");

      // create a reader and parse it
      RootFileReader reader;
      m_workspaces = reader.readFile(file);

    }

    void tearDown() {
      std::remove(".testInputFile.root");

    }

    void createTFile(const string& name) {
      TFile file(name.c_str(), "RECREATE");

      TCanvas* pCanvas = createCanvas("tester");
      pCanvas->Write();

      using namespace Viewer;
      RootEmbed::ParameterDb db;
      db.setObject("hist0", RootEmbed::Parameters("h0"));
      db.setObject("hist1", RootEmbed::Parameters("h1"));
      db.setObject("hist2", RootEmbed::Parameters("h2"));
      db.setObject("hist3", RootEmbed::Parameters("h3x", "h3y"));
      db.setObject("testCut", RootEmbed::Parameters("h3x", "h3y"));

      db.Write("paramDb");
    }

    TH1* createTH1(const std::string& name) {
      return new TH1F(name.c_str(), name.c_str(), 10, 0, 10);
    }

    TCutG* createCut() {
      auto pCut = new TCutG("testCut",4);

      pCut->SetName("testCut");
      pCut->SetPoint(0, 4, 5);
      pCut->SetPoint(1, 5, 7);
      pCut->SetPoint(2, 7, 2);
      pCut->SetPoint(3, 2, 1);

      return pCut;
    }

    TH2* createTH2(const std::string& name) {
      auto pHist = new TH2F(name.c_str(), name.c_str(), 10, 0, 10, 10, 0, 10);
      pHist->Fill(3,3);
      pHist->Fill(4,4);
      pHist->Fill(5,5);
      pHist->Fill(4,4);
      pHist->Fill(5,6);

      return pHist;
    }

    TCanvas* createCanvas(const std::string& name) {

      auto pCanvas= new TCanvas(name.c_str(), name.c_str(), 800);
      pCanvas->Divide(2,2);
      pCanvas->cd(1);
      createTH1("hist0")->Draw();

      pCanvas->cd(2);
      createTH1("hist1")->Draw();

      pCanvas->cd(3);
      createTH1("hist2")->Draw();

      pCanvas->cd(4);
      createTH2("hist3")->Draw();
      createCut()->Draw();

      return pCanvas;
    }

  protected:

    void numberOfTabs_0() {
      EQMSG("1 canvas becomes 1 tab", size_t(1), m_workspaces.size());
    }

    void numberOfColumns_0() {
      EQMSG("tab workspace has 2 columns", 2, m_workspaces.at(0)->getView().getColumnCount());
    }

    void numberOfRows_0() {
      EQMSG("tab workspace has 2 rows", 2, m_workspaces.at(0)->getView().getRowCount());
    }

    void histCreation_0 () {
      Viewer::HistogramList* pList = m_pInterface->getHistogramList();
      ASSERTMSG("histogram 0 must be added to HistogramList", pList->getHist("hist0") != nullptr);
      ASSERTMSG("histogram 1 must be added to HistogramList", pList->getHist("hist1") != nullptr);
      ASSERTMSG("histogram 2 must be added to HistogramList", pList->getHist("hist2") != nullptr);
      ASSERTMSG("histogram 3 must be added to HistogramList", pList->getHist("hist3") != nullptr);
    }

    void location_0() {
      Viewer::TabWorkspace* pWorkspace = m_workspaces.at(0);
      Viewer::QRootCanvas* pCanvas = pWorkspace->getView().getCanvas(0,0);
      ASSERTMSG("Histogram 0 is located in 1st quadrant", pCanvas->getCanvas()->FindObject("hist0") != nullptr);
    }

    void location_1() {
      Viewer::TabWorkspace* pWorkspace = m_workspaces.at(0);
      Viewer::QRootCanvas* pCanvas = pWorkspace->getView().getCanvas(0,1);
      ASSERTMSG("Histogram 1 is located in 2nd quadrant", pCanvas->getCanvas()->FindObject("hist1") != nullptr);
    }

    void location_2() {
      Viewer::TabWorkspace* pWorkspace = m_workspaces.at(0);
      Viewer::QRootCanvas* pCanvas = pWorkspace->getView().getCanvas(1,0);
      ASSERTMSG("Histogram 2 is located in 2nd quadrant", pCanvas->getCanvas()->FindObject("hist2") != nullptr);
    }

    void location_3() {
      Viewer::TabWorkspace* pWorkspace = m_workspaces.at(0);
      Viewer::QRootCanvas* pCanvas = pWorkspace->getView().getCanvas(1,1);
      ASSERTMSG("Histogram 3 is located in 3rd quadrant", pCanvas->getCanvas()->FindObject("hist3") != nullptr);
    }

    void location_4() {
      Viewer::TabWorkspace* pWorkspace = m_workspaces.at(0);
      Viewer::QRootCanvas* pCanvas = pWorkspace->getView().getCanvas(1,1);
      ASSERTMSG("2d cut is located in 3rd quadrant", pCanvas->getCanvas()->FindObject("testCut") != nullptr);
    }
};

CPPUNIT_TEST_SUITE_REGISTRATION(RootFileReaderTest);

