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
#include "CanvasOps.h"

#include <TPad.h>
#include <TCanvas.h>
#include <TH1.h>
#include <TObject.h>
#include <TList.h>

#include <QGridLayout>
#include <QSpacerItem>
#include <QMouseEvent>
#include <QPainter>
#include <QBrush>
#include <QColor>
#include <QMutexLocker>
#include <QMessageBox>
#include <QVBoxLayout>
#include <QInputDialog>

#include <iostream>
#include <stdexcept>
#include <tuple>

#include <Utility.h>

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

    connectSignalsToNewCanvas(m_pCurrentCanvas);
}

MultiSpectrumView::~MultiSpectrumView()
{
//    std::cout << "MultiSpectrumView::~MultiSpectrumView" << std::endl;
    for (int col=0; col<m_currentNColumns; col++) {
        for (int row=0; row<m_currentNRows; row++) {
            auto pItem = m_pLayout->itemAtPosition(row,col);
            if (pItem) {
                auto pWidget = dynamic_cast<QRootCanvas*>(pItem->widget());
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

void MultiSpectrumView::connectSignalsToNewCanvas(QRootCanvas* pCanvas)
{
    pCanvas->setShowEventStatus(true);
    connect(pCanvas, SIGNAL(mousePressed(QWidget*)),
            this, SLOT(setCurrentCanvas(QWidget*)));
    connect(pCanvas, SIGNAL(PadDoubleClicked(TPad*)),
            this, SLOT(onPadDoubleClick(TPad*)));
    connect(pCanvas, SIGNAL(CanvasStatusEvent(const char*)),
            m_pStatusBar, SLOT(onCursorMoved(const char*)));
    connect(pCanvas, SIGNAL(CanvasUpdated(QRootCanvas&)),
            this, SLOT(onCanvasUpdated()));
    connect(pCanvas, SIGNAL(MenuCommandExecuted(TObject*,QString)),
            this, SLOT(onMenuCommandExec(TObject*, QString)));
}

void MultiSpectrumView::setGeometry(int nRows, int nCols)
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
                connectSignalsToNewCanvas(pCanvas);

                m_pLayout->addWidget(pCanvas, row, col);
                m_canvases[{row, col}] = pCanvas;
            } else if (col >= currentNCols) {
                auto pCanvas = new QRootCanvas(this);
                m_pLayout->addWidget(pCanvas, row, col);
                connectSignalsToNewCanvas(pCanvas);
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
        bool changed = pBundle->synchronizeGates(m_pSpecTcl->getGateList());

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

void MultiSpectrumView::redrawView()
{
    if (m_ignoreUpdates) return;

    for (auto canvasInfo :  m_canvases) {
        redrawCanvas(*canvasInfo.second);
    }

    m_pCurrentCanvas->cd();
}

void MultiSpectrumView::redrawCanvas(QRootCanvas& canvas)
{
    if (m_ignoreUpdates) return;

    if (!m_pSpecTcl) {
        QMessageBox::warning(this,"Viewer update error", "Viewer cannot redraw canvas without SpecTcl interface");
        throw std::runtime_error("MultiSpectrumView::redrawCanvas() Cannot update canvas without a SpecTclInterface");
    }

    QRootCanvas* pCurrentCanvas = m_pCurrentCanvas;

    HistogramList* pHistList = m_pSpecTcl->getHistogramList();

    std::vector<TH1*> rootHists = SpectrumView::getAllHists(&canvas);

    canvas.cd();

    // redraw the histogram where it need to be drawn
    // This is a little tricky when we are dealing with superimposed histograms and copies.
    // The first histogram that is drawn will clear that canvas and that will delete
    // any superimposed histograms. In order to avoid a segfault, we need to first
    // get all the information we need to draw for the existing hists and store it.
    // After we have all the information, we can proceed to draw histograms.

    // our storage for the histogram and the draw option
    std::vector<std::pair<HistogramBundle*, QString>> drawInfo;

    // locate and store the data
    for (auto pHist : rootHists) {
        HistogramBundle* pBundle = pHistList->getHistFromClone(pHist);

        QString drawOption = CanvasOps::getDrawOption(canvas.getCanvas(), pHist);

        if (pBundle) drawInfo.push_back(make_pair(pBundle, drawOption));
    }

    // do the drawing
    bool first = true;
    bool oned  = true;                    
    for (auto drawable : drawInfo) {
        if (first) {
            first = false;

            // there is the possibility that we did not succeed in finding the
            // first histogram. IN that scenario, we would have no histogram
            // with a option lacking "same". We need to protect against that
            // by removing "same" if it is found.
            QString options = drawable.second;
            if (options.contains("same", Qt::CaseInsensitive)) {
                options.replace("same","",Qt::CaseInsensitive);
                drawable.first->draw(options);
            }
            // we need to figure out if this histogram is 1d or twod as
            // we can't superimpose incompatible spectra.
            // At this point I don't know enough about spectra to know how
            // to remove the incompatible spectra from the list in the pane
            // if it's not compatible so we'll just not render it for now:
            oned = rootHistogramIs1d(drawable.first->getHist());
        }
        // only draw a superposition if the histograms are compatible:
        // >Must< be true for the first one.
        
        if (oned == rootHistogramIs1d(drawable.first->getHist())) {
            drawable.first->draw(drawable.second);
        }
    }

//    // if we redrew the content of the current canvas, emit a signal saying
//    // so
//    if (std::find(m_canvases.begin(), canvases.end(), m_pCurrentCanvas) != canvases.end()) {
//        emit canvasUpdated(*m_pCurrentCanvas);
//    }

    m_pCurrentCanvas->cd();
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

void MultiSpectrumView::drawHistogram(
    HistogramBundle* pBundle, QString option
)
{
    try {   
        if (pBundle) {
            getCurrentCanvas()->cd();
            if (m_pSpecTcl) {
                pBundle->synchronizeGates(m_pSpecTcl->getGateList());
            }
    
            // If the option is "same" this is a superposition attempt.
            // That is only allowed if
            //  - There is a base spectrum.
            //  - The base spectrum has the same dimensionality as this new
            //    spectrum.
            
            if (option.contains("same")) {
                QRootCanvas* pCurrentCanvas = m_pCurrentCanvas;
                std::vector<TH1*> currentHists =
                    SpectrumView::getAllHists(pCurrentCanvas);
                if(currentHists.size()) {         // Could be first.
                    TH1* first = currentHists[0];
                    TH1& superpos(pBundle->getHist());
                    if (rootHistogramIs1d(*first) != rootHistogramIs1d(superpos))
                    {
                        throw std::runtime_error("Incompatible superposition attempted");
                    }
                }
            }
            pBundle->draw(option);
    
            // if the drawn histogram is empty, request content update for
            // all histograms in the pad it was drawn.
            if (m_pSpecTcl && (pBundle->getHist().Integral() == 0)) {
                m_pSpecTcl->requestHistContentUpdate(gPad);
            }
    
            emit canvasContentChanged(*m_pCurrentCanvas);
        }
        setFocus();
        refreshAll();
    } catch (std::exception& e) {
        QMessageBox::warning(
            m_pCurrentCanvas, "Superposition request error: ", e.what());
    }

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
        QString msg("Spectra can only display 100 or fewer canvases per tab\n");
        msg += "and %1 were requested. Requested operation cannot be completed.";
        QMessageBox::warning(this, "Too many spectra",
                             msg.arg(nSpectra));

        spectrumList.clear();
        nSpectra = spectrumList.count();
    }

    // determine the geometry of to display the canvases

    int nRows, nCols;
    std::tie(nRows, nCols) = computeOptimalGeometry(nSpectra);

    HistogramList& histList = *m_pSpecTcl->getHistogramList();

    QRootCanvas* pTopLeftCanvas = nullptr;
    auto pHistName = spectrumList.begin();
    for (int col=0; col<nCols; ++col) {
        for (int row=0; row<nRows; ++row) {

            auto pCanvas = new QRootCanvas(this);
            connectSignalsToNewCanvas(pCanvas);
            if (row==0 && col==0) {
                pTopLeftCanvas = pCanvas;
            }

            m_canvases[{row, col}] = pCanvas;

            m_pLayout->addWidget(pCanvas, row, col);

            if (pHistName != spectrumList.end()) {
                QMutexLocker listLock(histList.getMutex());
                HistogramBundle* pBundle = histList.getHist(*pHistName);
                if (pBundle) {
                    if (m_pSpecTcl) {
                        pBundle->synchronizeGates(m_pSpecTcl->getGateList());
                    }

                    pBundle->draw();

                }
                ++pHistName;
            }
        }
    }


    m_currentNColumns = nCols;
    m_currentNRows = nRows;

    setCurrentCanvas(pTopLeftCanvas);

    if (m_pSpecTcl) {
        for (auto spectrum : spectrumList) {
            m_pSpecTcl->requestHistContentUpdate(spectrum);
        }
    }

    refreshAll();

    emit currentCanvasChanged(*m_pCurrentCanvas);

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
    return (pCanvas->findObject(pHist->getName()) != nullptr);
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

void MultiSpectrumView::onMenuCommandExec(TObject* pObj, QString methodName)
{
    if (methodName == "SetDrawOption") {
		TH1* pHist = dynamic_cast<TH1*>(pObj);
		if (pHist) {
            TVirtualPad* pPad = findPadContaining(pObj);
			HistogramList* pList = m_pSpecTcl->getHistogramList();
            HistogramBundle* pBundle = pList->getHistFromClone(pHist);
            if (pBundle == nullptr) {
                QMessageBox::warning(this, "Failed to locate histogram",
                                     "Unable to locate histogram that the draw option corresponds to");
                return;
            }

            QString option = QInputDialog::getText(this, "Spectrum configuration",
                                                   "Enter draw option");

            if (option.isEmpty()) return; // user cancelled

            // set the specific object's draw option
            TObjLink* pLink = findObjectLink(pPad, pObj);
            pLink->SetOption(option.toUtf8().constData());

            // set the option for future draw options
            pBundle->setDefaultDrawOption(option);
            refreshAll();
		}
	}
}

void MultiSpectrumView::toggleLogx()
{
    if (m_pCurrentCanvas->GetLogx()) {
        m_pCurrentCanvas->SetLogx(0);
    } else {
        m_pCurrentCanvas->SetLogx(1);
    }

    m_pCurrentCanvas->Update();
}

void MultiSpectrumView::toggleLogy()
{
    if (m_pCurrentCanvas->GetLogy()) {
        m_pCurrentCanvas->SetLogy(0);
    } else {
        m_pCurrentCanvas->SetLogy(1);
    }

    m_pCurrentCanvas->Update();
}

void MultiSpectrumView::toggleLogz()
{
    if (m_pCurrentCanvas->GetLogz()) {
        m_pCurrentCanvas->SetLogz(0);
    } else {
        m_pCurrentCanvas->SetLogz(1);
    }

    m_pCurrentCanvas->Update();
}

void MultiSpectrumView::zoomX()
{
    auto hists = SpectrumView::getAllHists(m_pCurrentCanvas);
    if (hists.size()>0) {
        TAxis* pAxis = hists[0]->GetXaxis();
        Int_t x0 = pAxis->GetFirst();
        Int_t x1 = pAxis->GetLast();

        Int_t binOffset = (x1-x0)*0.05;

        pAxis->SetRange(x0+binOffset, x1-binOffset);
    }

    m_pCurrentCanvas->Modified(1);
    m_pCurrentCanvas->Update();
}

void MultiSpectrumView::zeroX()
{
    auto hists = SpectrumView::getAllHists(m_pCurrentCanvas);
    if (hists.size()>0) {
        TAxis* pAxis = hists[0]->GetXaxis();

        pAxis->UnZoom();
    }

    m_pCurrentCanvas->Modified(1);
    m_pCurrentCanvas->Update();
}

void MultiSpectrumView::unzoomX()
{
    auto hists = SpectrumView::getAllHists(m_pCurrentCanvas);
    if (hists.size()>0) {
        TAxis* pAxis = hists[0]->GetXaxis();

        pAxis->ZoomOut();
    }

    m_pCurrentCanvas->Modified(1);
    m_pCurrentCanvas->Update();
}



void MultiSpectrumView::zoomY()
{
    auto hists = SpectrumView::getAllHists(m_pCurrentCanvas);
    if (hists.size()>0) {
        TAxis* pAxis = hists[0]->GetYaxis();
        Int_t x0 = pAxis->GetFirst();
        Int_t x1 = pAxis->GetLast();

        Int_t binOffset = (x1-x0)*0.05;

        pAxis->SetRange(x0+binOffset, x1-binOffset);
    }

    m_pCurrentCanvas->Modified(1);
    m_pCurrentCanvas->Update();
}

void MultiSpectrumView::zeroY()
{
    auto hists = SpectrumView::getAllHists(m_pCurrentCanvas);
    if (hists.size()>0) {
        TAxis* pAxis = hists[0]->GetYaxis();

        pAxis->UnZoom();
    }

    m_pCurrentCanvas->Modified(1);
    m_pCurrentCanvas->Update();
}

void MultiSpectrumView::unzoomY()
{
    auto hists = SpectrumView::getAllHists(m_pCurrentCanvas);
    if (hists.size()>0) {
        TAxis* pAxis = hists[0]->GetYaxis();

        pAxis->ZoomOut();
    }

    m_pCurrentCanvas->Modified(1);
    m_pCurrentCanvas->Update();
}



TVirtualPad* MultiSpectrumView::findPadContaining(TObject* pObj)
{
    auto pads = getAllCanvases();
    TObject* pFoundObj = nullptr;
    TVirtualPad* pFoundPad = nullptr;

    for (auto& pPad : pads) {
        pFoundObj = pPad->getCanvas()->GetListOfPrimitives()->FindObject(pObj);
        if (pFoundObj) {
            pFoundPad = pPad->getCanvas();
            break;
        }
    }

    return pFoundPad;
}

TObjLink* MultiSpectrumView::findObjectLink(TVirtualPad* pPad, TObject* pObj)
{
    TObjLink* pFoundLink = nullptr;
    TObjLink *pLink = pPad->GetListOfPrimitives()->FirstLink();
    while (pLink) {
        if (pLink->GetObject() == pObj) {
            pFoundLink = pLink;
            break;
        }
        pLink = pLink->Next();
    }
    return pFoundLink;
}

} // end of namespace
