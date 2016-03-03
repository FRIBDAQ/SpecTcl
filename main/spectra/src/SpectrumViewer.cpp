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

#include "SpectrumViewer.h"
#include "ui_SpectrumViewer.h"
#include "HistogramList.h"
#include "ContentRequestHandler.h"
#include "GlobalSettings.h"
#include "LockGuard.h"
#include "QRootCanvas.h"
#include "SpecTclInterface.h"

#include <QMessageBox>

#include <TH1.h>
#include <TH2.h>
#include <TCanvas.h>

#include <iostream>
#include <stdexcept>

using namespace std;

namespace Viewer
{

SpectrumViewer::SpectrumViewer(SpecTclInterface* pSpecTcl, QWidget *parent) :
    SpectrumView(parent),
    ui(new Ui::SpectrumViewer),
    m_currentHist(nullptr),
    m_canvas(nullptr),
    m_currentCanvas(nullptr),
    m_pSpecTcl(pSpecTcl),
    m_canvasList()
{

    ui->setupUi(this);

    m_canvas = new QRootCanvas(this);
    m_canvasList.insert(m_canvas);
    m_currentCanvas = m_canvas;

    auto pTab = ui->pTabWidget->currentWidget();
    QGridLayout* pLayout = new QGridLayout;
    pLayout->addWidget(m_canvas);
    pTab->setLayout(pLayout);

    m_canvas->getCanvas()->Resize();
    m_canvas->getCanvas()->Divide(1,1);
    m_canvas->getCanvas()->cd(1);
    m_canvas->show();

    connect(m_pSpecTcl, SIGNAL(gateListChanged()),
            this, SLOT(refreshAll()));

    connect(m_pSpecTcl->getHistogramList(),
            SIGNAL(histogramRemoved(HistogramBundle*)),
            this,
            SLOT(onHistogramRemoved(HistogramBundle*)));

}

SpectrumViewer::~SpectrumViewer()
{
    delete ui;
}

QRootCanvas* SpectrumViewer::getCurrentCanvas()
{
    return m_currentCanvas;
}


void SpectrumViewer::onGeometryChanged(int nRows, int nColumns)
{
  m_currentCanvas->Clear();
  m_currentCanvas->getCanvas()->Divide(nRows, nColumns);
  m_currentCanvas->cd(1);
  if (m_currentHist) {
    m_currentHist->draw();
  }

  m_currentCanvas->Modified();
  m_currentCanvas->Update();


  m_currentNRows = nRows;
  m_currentNColumns = nColumns;
}

void SpectrumViewer::onHistogramRemoved(HistogramBundle *pHistBundle)
{
  if ( pHistBundle == m_currentHist ) {
      // a currently visible histogram is being deleted

      // let's switch to viewing the first histogram in the list
      auto pHistList = m_pSpecTcl->getHistogramList();

      // lock the histogram list
      QMutexLocker lock(pHistList->getMutex());

      if (pHistList->size() != 0) {
          auto itFirstHist = m_pSpecTcl->getHistogramList()->begin();

          update(itFirstHist->second.get());
      } else {
          update(nullptr);
      }

  }
}


void SpectrumViewer::update(HistogramBundle* gHist)
{

    // not really good practice... could block main thread
    // should fix later
    m_currentHist = gHist;
    if ( ! m_currentHist ) {
      return;
    } else {
      // The draw operation can throw, so we need to protect ourselves
      // against that...
      try {

        m_currentHist->synchronizeGates(m_pSpecTcl->getGateList());
        m_currentHist->draw();

      } catch (const exception& exc) {
        QMessageBox::warning(nullptr, "Drawing error", exc.what());
      }

      refreshAll();
    }
  
}

void SpectrumViewer::refreshAll()
{
  TIter it(m_currentCanvas->getCanvas()->GetListOfPrimitives());
  TObject *pObject = nullptr;
  while (( pObject = it.Next() )) {
      if (pObject->InheritsFrom(TPad::Class())) {
          auto pPad = dynamic_cast<TPad*>(pObject);
          pPad->Modified();
        }
    }

  m_currentCanvas->Update();
}

void SpectrumViewer::onError(int errorCode, const QString& reason)
{
    QMessageBox::warning(nullptr,"Error during update",reason);
}

} // end of namespace
