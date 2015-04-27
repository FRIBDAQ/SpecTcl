//    This software is Copyright by the Board of Trustees of Michigan
//    State University (c) Copyright 2015.
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

static const char* Copyright = "(C) Copyright Michigan State University 2015, All rights reserved";
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
    QFrame(parent),
    ui(new Ui::SpectrumViewer),
    m_currentHist(nullptr),
    m_canvas(nullptr),
    m_reqHandler(pSpecTcl->getHistogramList()),
    m_currentCanvas(nullptr),
    m_pSpecTcl(pSpecTcl),
    m_canvasList()
{

    ui->setupUi(this);
    setFrameShadow(QFrame::Plain);
    setFrameShape(QFrame::NoFrame);


    m_canvas = new QRootCanvas(this);
    m_canvasList.insert(m_canvas);
    m_currentCanvas = m_canvas;


    ui->gridLayout->addWidget(m_canvas,0,0);

    m_canvas->getCanvas()->Resize();
    m_canvas->getCanvas()->cd();
    m_canvas->show();

    // set up the connections of signals/slots
    connect(ui->updateButton, SIGNAL(pressed()), this, SLOT(requestUpdate()));

    if ( !connect(&m_reqHandler, SIGNAL(parsingComplete(HistogramBundle*)),
                  this, SLOT(update(HistogramBundle*))) ) {
        std::cout << "Failed to connect parsingComplete --> update" << std::endl;
    }

    if ( !connect(&m_reqHandler, SIGNAL(error(int,const QString&)),
                  this, SLOT(onError(int, const QString&))) ) {
        std::cout << "Failed to connect error --> onError" << std::endl;
    }

    if ( !connect(m_pSpecTcl, SIGNAL(gateListChanged()),
                  this, SLOT(refresh())) ) {
        std::cout << "Failed to connect error --> onError" << std::endl;
    }

}

SpectrumViewer::~SpectrumViewer()
{
    delete ui;
}

QRootCanvas* SpectrumViewer::getCurrentFocus() const {
    return m_currentCanvas;
}

HistogramBundle* SpectrumViewer::getCurrentHist() const {
    return m_currentHist;
}

void SpectrumViewer::requestUpdate()
{
    m_reqHandler.get(formUpdateRequest());
}

void SpectrumViewer::update(HistogramBundle* gHist)
{
    m_canvas->cd();


    // not really good practice... could block main thread
    // should fix later
    m_currentHist = gHist;
    if ( ! m_currentHist ) {
      return;
    } else {
      if ( ! m_currentHist->hist() ) return;

      std::cout << *m_currentHist << std::endl;
      // The draw operation can throw, so we need to protect ourselves
      // against that...
      try {

        if (m_currentHist->hist()->InheritsFrom(TH2::Class())) {
          m_currentHist->draw("colz");
        } else {
          m_currentHist->draw();
        }

      } catch (const exception& exc) {
        QMessageBox::warning(nullptr, "Drawing error", exc.what());
      }

      m_canvas->Update();
    }
  
}

void SpectrumViewer::refresh()
{
  update(m_currentHist);
}

QUrl SpectrumViewer::formUpdateRequest()
{
    if (m_currentHist) {
      if (m_currentHist->hist()) {
        QString name = m_currentHist->hist()->GetName();
        auto host = GlobalSettings::getServerHost();
        auto port = GlobalSettings::getServerPort();

        QString reqUrl("http://%1:%2/spectcl/spectrum/contents?name=%3");
        reqUrl = reqUrl.arg(host).arg(port).arg(name);
        return QUrl(reqUrl);
      }
    }

    return QUrl();
}

void SpectrumViewer::onError(int errorCode, const QString& reason)
{
    QMessageBox::warning(nullptr,"Error during update",reason);
}

} // end of namespace
