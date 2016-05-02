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

#include "MultiSpectrumView.h"
#include "HistogramBundle.h"
#include "QRootCanvas.h"
#include "SpecTclInterface.h"
#include "HistogramList.h"
#include "StatusBar.h"

#include <TPad.h>
#include <TCanvas.h>
#include <TH1.h>

#include <QGridLayout>
#include <QSpacerItem>
#include <QMouseEvent>
#include <QPainter>
#include <QBrush>
#include <QColor>
#include <QMutexLocker>
#include <QMessageBox>
#include <QVBoxLayout>

#include <iostream>
#include <stdexcept>
#include <tuple>

using namespace std;

namespace Viewer
{
MultiSpectrumView::MultiSpectrumView(std::shared_ptr<SpecTclInterface> pSpecTcl,
                                     QWidget *parent) :
    SpectrumView(parent),
    m_pLayout(),
    m_histMap(),
    m_pSpecTcl(pSpecTcl),
    m_pCurrentCanvas(new QRootCanvas(this)),
    m_currentNRows(1),
    m_currentNColumns(1),
    m_canvases(),
    m_isZoomed(false),
    m_pStatusBar(),
    m_ignoreUpdates(false)
{

    auto pVLayout = new QVBoxLayout(this);
    setLayout(pVLayout);

    m_pLayout = new QGridLayout(this);
    pVLayout->addLayout(m_pLayout);

    m_pStatusBar = new StatusBar(this);
    pVLayout->addWidget(m_pStatusBar);


    m_pLayout->addWidget(m_pCurrentCanvas, 0, 0);
    m_pCurrentCanvas->setShowEventStatus(true);
    m_pCurrentCanvas->cd();
    m_canvases[{0,0}] = m_pCurrentCanvas;


    connect(m_pCurrentCanvas, SIGNAL(mousePressed(QWidget*)),
            this, SLOT(setCurrentCanvas(QWidget*)));

    connect(m_pCurrentCanvas, SIGNAL(PadDoubleClicked(TPad*)),
            this, SLOT(onPadDoubleClick(TPad*)));

    connect(m_pCurrentCanvas, SIGNAL(CanvasStatusEvent(const char*)),
            m_pStatusBar, SLOT(onCursorMoved(const char*)));

    connect(m_pCurrentCanvas, SIGNAL(CanvasUpdated()), this, SLOT(onCanvasUpdated()));

}

MultiSpectrumView::~MultiSpectrumView()
{
    for (int col=0; col<m_currentNColumns; col++) {
        for (int row=0; row<m_currentNRows; row++) {
            auto pItem = m_pLayout->itemAtPosition(row,col);
            if (pItem) {
                auto pWidget = pItem->widget();
                delete pWidget;
            }
        }
    }
}

int MultiSpectrumView::getRowCount() const
{
    return m_currentNRows;
}

int MultiSpectrumView::getColumnCount() const
{
    return m_currentNColumns;
}

vector<QRootCanvas*> MultiSpectrumView::getAllCanvases()
{
  vector<QRootCanvas*> canvases;

  for (int col=0; col<m_currentNColumns; col++) {
      for (int row=0; row<m_currentNRows; row++) {
          auto pItem = m_pLayout->itemAtPosition(row, col);
          if (pItem) {
              auto pCanvas = dynamic_cast<QRootCanvas*>(pItem->widget());
              if (pCanvas) {
                  canvases.push_back(pCanvas);
              } else {
                  throw runtime_error("MultiSpectrumView::getAllCanvases() Failed to upcast widget to QRootCanvas*");
              }
          }
       }
    }

  return canvases;
}


void MultiSpectrumView::setSpecTclInterface(std::shared_ptr<SpecTclInterface> pSpecTcl)
{
    m_pSpecTcl = pSpecTcl;
}

void MultiSpectrumView::onGeometryChanged(int nRows, int nCols)
{

    // figure out the current configuation...
    if (nRows == m_currentNRows && nCols == m_currentNColumns) {
        // if the same, do nothing
        return;
    }

    auto prevCanvasLoc = findLocation(m_pCurrentCanvas);

    // if different, then figure out how to change
    int currentNRows = m_currentNRows;
    int currentNCols = m_currentNColumns;

    int maxRows = std::max(nRows, currentNRows);
    int maxCols = std::max(nCols, currentNCols);
    for (int row=0; row<maxRows; ++row) {
        for (int col=0; col<maxCols; ++col) {

            // get rid of current rows that are marked for destruction
            if ( row >= nRows ) {
               auto pItem = m_pLayout->itemAtPosition(row, col);
               if (pItem) {
                   m_pLayout->removeItem(pItem);
                   delete pItem->widget();
                   delete pItem;
                   m_canvases.erase({row, col});
               }
            } else if ( col >= nCols ) {
                auto pItem = m_pLayout->itemAtPosition(row, col);
                if (pItem) {
                    m_pLayout->removeItem(pItem);
                    delete pItem->widget();
                    delete pItem;
                    m_canvases.erase({row, col});
                }
            }

            // add empty spaces
            if (row >= currentNRows) {
                auto pCanvas = new QRootCanvas(this);
                pCanvas->setShowEventStatus(true);
                m_pLayout->addWidget(pCanvas, row, col);
                connect(pCanvas, SIGNAL(mousePressed(QWidget*)),
                        this, SLOT(setCurrentCanvas(QWidget*)));
                connect(pCanvas, SIGNAL(PadDoubleClicked(TPad*)),
                        this, SLOT(onPadDoubleClick(TPad*)));
                connect(pCanvas, SIGNAL(CanvasStatusEvent(const char*)),
                        m_pStatusBar, SLOT(onCursorMoved(const char*)));
                connect(pCanvas, SIGNAL(CanvasUpdated()),
                        this, SLOT(onCanvasUpdated()));
                m_canvases[{row, col}] = pCanvas;
            } else if (col >= currentNCols) {
                auto pCanvas = new QRootCanvas(this);
                m_pLayout->addWidget(pCanvas, row, col);
                pCanvas->setShowEventStatus(true);
                connect(pCanvas, SIGNAL(mousePressed(QWidget*)),
                        this, SLOT(setCurrentCanvas(QWidget*)));
                connect(pCanvas, SIGNAL(PadDoubleClicked(TPad*)),
                        this, SLOT(onPadDoubleClick(TPad*)));
                connect(pCanvas, SIGNAL(CanvasStatusEvent(const char*)),
                        m_pStatusBar, SLOT(onCursorMoved(const char*)));
                connect(pCanvas, SIGNAL(CanvasUpdated()),
                        this, SLOT(onCanvasUpdated()));

                m_canvases[{row, col}] = pCanvas;
            }
        }
    }

    m_currentNRows = nRows;
    m_currentNColumns = nCols;

    // update our current canvas
    int newCurrentCanvasRow = prevCanvasLoc.first;
    int newCurrentCanvasCol = prevCanvasLoc.second;

    if (prevCanvasLoc.first >= m_currentNRows) {
        newCurrentCanvasRow = m_currentNRows-1;
    }
    if (prevCanvasLoc.second >= m_currentNColumns) {
        newCurrentCanvasCol = m_currentNColumns-1;
    }

    auto pItem = m_pLayout->itemAtPosition(newCurrentCanvasRow, newCurrentCanvasCol);
    setCurrentCanvas(pItem->widget());

    update();

}

void MultiSpectrumView::setCurrentCanvas(QWidget *pWidget)
{
  if (auto pCanvas = dynamic_cast<QRootCanvas*>(pWidget)) {

      // don't do anything if we just clicked on the thing
      if (m_pCurrentCanvas == pCanvas) return;

      m_pCurrentCanvas = pCanvas;
      m_pCurrentCanvas->cd();

      setFocus();
      QWidget::update();

      emit currentCanvasChanged(*m_pCurrentCanvas);
  }
}

void MultiSpectrumView::keyPressEvent(QKeyEvent *key)
{
    if (m_isZoomed) return;

  auto location = findLocation(m_pCurrentCanvas);
  if (location == std::pair<int,int>(-1,-1))
    return;

    int newRow = location.first;
    int newCol = location.second;

    int keyId = key->key();
    if (keyId == Qt::Key_Up) {
        if (location.first == 0) {
            newRow = m_currentNRows-1;
            if (location.second == 0) {
              newCol = m_currentNColumns-1;
            } else {
              newCol = location.second - 1;
            }
        } else {
            newRow = location.first-1;
        }
    } else if (keyId == Qt::Key_Right) {
        if (location.second == m_currentNColumns-1) {
            newCol = 0;
            if (location.first == m_currentNRows-1) {
                newRow = 0;
              } else {
                newRow = location.first + 1;
              }
        } else {
            newCol = location.second+1;
        }
    } else if (keyId == Qt::Key_Down) {
        if (location.first == m_currentNRows-1) {
            newRow = 0;
            if (location.second == m_currentNColumns-1) {
              newCol = 0;
            } else {
              newCol = location.second + 1;
            }
        } else {
            newRow = location.first+1;
        }
    } else if (keyId == Qt::Key_Left) {
        if (location.second == 0) {
            newCol = m_currentNColumns-1;
            if (location.first == 0) {
              newRow = m_currentNRows-1;
            } else {
              newRow = location.first - 1;
            }    
        } else {
            newCol = location.second-1;
        }
    }

    auto pNewCanvas = m_pLayout->itemAtPosition(newRow, newCol)->widget();
    setCurrentCanvas(pNewCanvas);

}

std::pair<int,int> MultiSpectrumView::findLocation(QWidget *pWidget)
{
  for (int col=0; col<m_currentNColumns; col++) {
      for (int row=0; row<m_currentNRows; row++) {
          auto pItem = m_pLayout->itemAtPosition(row,col);
          if (pItem) {
              if (pItem->widget() == pWidget) {
                  return std::make_pair(row,col);
                }
            }
        }
    }
  return std::make_pair(-1, -1);
}



void MultiSpectrumView::paintEvent(QPaintEvent *evt)
{
  QPainter painter(this);
  QPen pen(Qt::DotLine);
  pen.setColor(Qt::red);
  pen.setWidth(2);
  painter.setPen(pen);
  auto frame = m_pCurrentCanvas->geometry();

  painter.drawRect(frame.x()-2, frame.y()-2,
                   frame.width()+3, frame.height()+3);

}


QRootCanvas* MultiSpectrumView::getCurrentCanvas()
{
    return m_pCurrentCanvas;
}


QRootCanvas* MultiSpectrumView::getCanvas(int row, int col)
{
    QRootCanvas* pCanvas = nullptr;

    auto itResult = m_canvases.find({row, col});
    if (itResult != m_canvases.end()) {
        pCanvas = itResult->second;
    }
    return pCanvas;
}

void MultiSpectrumView::updateView(HistogramBundle* pBundle)
{
    // sometimes we don't want to update any canvas... especially if we
    // are in the middle of creating a gate
    if (m_ignoreUpdates) return;

    if (pBundle && m_pSpecTcl) {

        QMutex* pMutex = pBundle->getMutex();
        pMutex->lock();

        // there is one histogram to synchronize, synchronize it once
        pBundle->synchronizeGates(m_pSpecTcl->getGateList());

        std::vector<QRootCanvas*> canvases = locateCanvasesWithHist(*pBundle);

        // redraw the histogram where it need to be drawn
        for (auto pCanvas : canvases) {

            pCanvas->cd();
            pBundle->draw();

        }
        // some things that get triggered by canvasUpdated might try to lock
        // the mutex, so we will let it go before moving on from here
        pMutex->unlock();

        // make sure that we reset our state to make current canvas drawable
        m_pCurrentCanvas->cd();

        // if we redrew the content of the current canvas, emit a signal saying
        // so
        if (std::find(canvases.begin(), canvases.end(), m_pCurrentCanvas) != canvases.end()) {
            emit canvasUpdated(*m_pCurrentCanvas);
        }
    }

    if (isVisible()) {
        setFocus();
        refreshAll();
    }
}


std::vector<QRootCanvas*>
MultiSpectrumView::locateCanvasesWithHist(HistogramBundle &rHistPkg)
{
    std::vector<QRootCanvas*> canvasesWithHist;

    for (auto& pane : m_canvases ) {
        QRootCanvas* pCanvas = pane.second;
        if (histogramInCanvas(&rHistPkg, pCanvas)) {
            canvasesWithHist.push_back(pCanvas);
        }
    }

    return canvasesWithHist;

}

void MultiSpectrumView::drawHistogram(HistogramBundle* pBundle)
{
    if (pBundle) {
        getCurrentCanvas()->cd();
        if (m_pSpecTcl) {
            pBundle->synchronizeGates(m_pSpecTcl->getGateList());
        }
        pBundle->draw();

        emit canvasContentChanged(*m_pCurrentCanvas);
    }
    setFocus();
    refreshAll();

}




void MultiSpectrumView::toggleZoom(QWidget& rWidget)
{
    if (! m_isZoomed) {

        QLayoutItem* pChild;
        for (auto& canvasInfo : m_canvases) {
            if (canvasInfo.second != &rWidget) {
                canvasInfo.second->hide();
            }
        }

        setCurrentCanvas(&rWidget);
        m_isZoomed = true;

    } else {

        for (auto& canvasInfo : m_canvases) {
            canvasInfo.second->show();
        }

        m_isZoomed = false;
    }

    emit zoomChanged(m_isZoomed);
}


void MultiSpectrumView::toggleZoom()
{
    if (m_pCurrentCanvas) {
        toggleZoom(*m_pCurrentCanvas);
    }
}

void MultiSpectrumView::clearLayout()
{
    QLayoutItem* pItem;
    while (( pItem = m_pLayout->takeAt(0) )) {
        if (pItem->widget()) {
            delete pItem->widget();
        }
        delete pItem;
    }
}



std::tuple<int, int> MultiSpectrumView::computeOptimalGeometry(int nSpectra)
{
    int nRows = 1;
    int nCols = 1;

    for (int cols=1; cols<11; ++cols) {
        if (nRows*cols >= nSpectra) {
            nCols = cols;
            break;
        }

        nRows++;

        if (nRows*cols >= nSpectra) {
            nCols = cols;
            break;
        }
    }

    return std::make_tuple(nRows, nCols);
}

void MultiSpectrumView::layoutSpectra(QStringList spectrumList)
{
    int nSpectra = spectrumList.count();

    if (nSpectra > 100) {
        QString msg("Spectra can only display 100 or fewer spectra per tab\n");
        msg += "and the user specified %1. Requested operation cannot be completed.";
        QMessageBox::warning(this, "Too many spectra",
                             msg.arg(nSpectra));
        return;
    }

    // determine the geometry of to display the canvases

    int nRows, nCols;
    std::tie(nRows, nCols) = computeOptimalGeometry(nSpectra);

    HistogramList& histList = *m_pSpecTcl->getHistogramList();

    QRootCanvas* pTopLeftCanvas = nullptr;
    auto pHistName = spectrumList.begin();
    for (int col=0; col<nCols; ++col) {
        for (int row=0; row<nRows; ++row) {

            QRootCanvas* pCanvas = new QRootCanvas(this);
            pCanvas->setShowEventStatus(true);
            if (row==0 && col==0) {
                pTopLeftCanvas = pCanvas;
            }
            connect(pCanvas, SIGNAL(mousePressed(QWidget*)),
                    this, SLOT(setCurrentCanvas(QWidget*)));
            connect(pCanvas, SIGNAL(PadDoubleClicked(TPad*)),
                    this, SLOT(onPadDoubleClick(TPad*)));
            connect(pCanvas, SIGNAL(CanvasStatusEvent(const char*)),
                    m_pStatusBar, SLOT(onCursorMoved(const char*)));
            connect(pCanvas, SIGNAL(CanvasUpdated()),
                    this, SLOT(onCanvasUpdated()));

            m_canvases[{row, col}] = pCanvas;

            m_pLayout->addWidget(pCanvas, row, col);

            if (pHistName != spectrumList.end()) {
                QMutexLocker listLock(histList.getMutex());
                HistogramBundle* pBundle = histList.getHist(*pHistName);
                if (pBundle) {
                    QMutexLocker histLock(pBundle->getMutex());
                    pBundle->draw();
                }
                ++pHistName;
            }

        }
    }

    m_currentNColumns = nCols;
    m_currentNRows = nRows;

    setCurrentCanvas(pTopLeftCanvas);

    refreshAll();

}

void MultiSpectrumView::mouseDoubleClickEvent(QMouseEvent* evt)
{
    auto pWidget = childAt(evt->x(), evt->y());
    Q_ASSERT( pWidget );

    auto location = findLocation(pWidget);
    toggleZoom(*pWidget);

    QWidget::update();
}

void MultiSpectrumView::onPadDoubleClick(TPad *pPad)
{
    QRootCanvas* pTargetCanvas = nullptr;
    auto canvases = getAllCanvases();
    for (auto& pCanvas : canvases) {
        if (pPad == pCanvas->getCanvas()) {
            pTargetCanvas = pCanvas;
            break;
        }
    }

    toggleZoom(*pTargetCanvas);
}

void MultiSpectrumView::refreshAll()
{
    // if different, then figure out how to change
    for (int row=0; row<m_currentNRows; ++row) {
        for (int col=0; col<m_currentNColumns; ++col) {

            auto pItem = m_pLayout->itemAtPosition(row, col);
            if (pItem) {
                auto pCanvas = dynamic_cast<QRootCanvas*>(pItem->widget());
                if (pCanvas) {
                    pCanvas->Modified();
                    pCanvas->Update();
                }
            }
        }
    }
    m_pCurrentCanvas->cd();
}


void MultiSpectrumView::onHistogramRemoved(HistogramBundle *pHistBundle)
{
}


bool MultiSpectrumView::histogramVisible(HistogramBundle *pHist)
{
  for (int row = 0; row<m_currentNRows; ++row) {
      for (int col = 0; col<m_currentNColumns; ++col) {

          auto pItem = m_pLayout->itemAtPosition(row,col);

          if (histogramInCanvas(pHist, dynamic_cast<QRootCanvas*>(pItem->widget()))) {
              return true;
          }
      }
  }

  return false;
}


bool MultiSpectrumView::histogramInCanvas(HistogramBundle* pHist, QRootCanvas* pCanvas)
{
  return (pCanvas->findObject(&pHist->getHist()) != nullptr);
}

void MultiSpectrumView::onCanvasUpdated()
{
    emit canvasUpdated(*m_pCurrentCanvas);
}

bool MultiSpectrumView::isIgnoringUpdates() const
{
    return m_ignoreUpdates;
}

void MultiSpectrumView::ignoreUpdates(bool state)
{
    m_ignoreUpdates = state;
}

} // end of namespace
