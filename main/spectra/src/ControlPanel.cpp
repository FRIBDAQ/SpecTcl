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

#include "ControlPanel.h"
#include "GeometrySelector.h"
#include "SpecTclInterface.h"
#include "SpectrumView.h"
#include "HistogramList.h"
#include "HistogramBundle.h"
#include "SpectrumDrawChooser.h"
#include "MainWindow.h"
#include "ui_ControlPanel.h"
#include <set>

#include <iostream>

using namespace std;

namespace Viewer
{



ControlPanel::ControlPanel(std::shared_ptr<SpecTclInterface> pSpecTcl,
                           SpectrumView *pView, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ControlPanel),
    m_pSpecTcl(pSpecTcl),
    m_pView(pView)
{
    assembleWidgets();

    connectSignalsAndSlots();
}

ControlPanel::~ControlPanel()
{
    delete ui;
}

void ControlPanel::assembleWidgets()
{
    ui->setupUi(this);

}

void ControlPanel::connectSignalsAndSlots()
{

    connect(ui->pUpdateSelected, SIGNAL(clicked()), this, SLOT(onUpdateSelected()));
    connect(ui->pUpdateAll, SIGNAL(clicked()), this, SLOT(onUpdateAll()));
//    connect(ui->pRefresh, SIGNAL(clicked()), this, SLOT(onRefresh()));
    connect(ui->pZoomButton, SIGNAL(clicked()), this, SLOT(onZoom()));

    connect(ui->pStatisticsButton, SIGNAL(clicked()), this, SLOT(onStatisticsButtonClicked()));
    connect(ui->pDisplayButton, SIGNAL(clicked()), this, SLOT(onDisplayButtonClicked()));
    
    connect(ui->pGrobPage, SIGNAL(clicked()), this, SLOT(onGrobPageSelected()));
    connect(ui->pSpecPage, SIGNAL(clicked()), this, SLOT(onSpecPageSelected()));
    connect(ui->pMarker,   SIGNAL(clicked()), this, SLOT(onMarkerClicked()));
    
    
}

void ControlPanel::setSpecTclInterface(std::shared_ptr<SpecTclInterface> pSpecTcl)
{
    m_pSpecTcl = pSpecTcl;
}

void ControlPanel::onUpdateSelected() {
  if (m_pSpecTcl) {
      m_pSpecTcl->requestHistContentUpdate(m_pView->getCurrentCanvas());
  }
}

void ControlPanel::onUpdateAll()
{
  if (m_pSpecTcl) {
      auto canvases = m_pView->getAllCanvases();

      // this needs thread synchronization
      auto it = canvases.begin();
      auto it_end = canvases.end();
      std::set<TH1*> requestHistory;
      while ( it != it_end ) {
          vector<TH1*> hists = m_pView->getAllHists(*it);
          for (auto pHist : hists) {
              // try to insert into the request history.. if the hist already
              // lives in the request history, the second element of the
              // returned pair will be false
              if ( requestHistory.insert(pHist).second == true ) {
                m_pSpecTcl->requestHistContentUpdate(QString(pHist->GetName()));
              }
          }
          ++it;
      }
  }
}


void ControlPanel::onRefresh()
{
  m_pView->refreshAll();
}


void ControlPanel::setZoomed(bool isZoomed) {
    if (isZoomed) {
        ui->pZoomButton->setText("Un&zoom");
    } else {
        ui->pZoomButton->setText("&Zoom");
    }
}

void ControlPanel::onZoom() {

    // if we are zoomed, then the text should say "Unzoom"
    setZoomed((ui->pZoomButton->text() == "Un&zoom"));

    emit zoomButtonClicked();
}


void ControlPanel::onStatisticsButtonClicked()
{
    if (ui->pStatisticsButton->text() == "&Gates >>") {
        ui->pStatisticsButton->setText("&Gates <<");
    } else {
        ui->pStatisticsButton->setText("&Gates >>");
    }
    emit statisticsButtonClicked();
}


void ControlPanel::onDisplayButtonClicked()
{
    if (ui->pDisplayButton->text() == "&Display >>") {
        ui->pDisplayButton->setText("&Display <<");
    } else {
        ui->pDisplayButton->setText("&Display >>");
    }
    emit displayButtonClicked();
}

void ControlPanel::onGrobPageSelected()
{
    ui->pControls->setCurrentIndex(2);
}

void ControlPanel::onSpecPageSelected()
{
    ui->pControls->setCurrentIndex(0);    
}
void ControlPanel::onMarkerClicked()
{
    std::cerr << "Would bring up marker dialog here.\n";
}
} // end of namespace
