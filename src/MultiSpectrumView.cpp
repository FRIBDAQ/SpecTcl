#include "MultiSpectrumView.h"
#include "HistogramBundle.h"
#include "QRootCanvas.h"
#include "SpecTclInterface.h"

#include <QGridLayout>
#include <QSpacerItem>
#include <QMouseEvent>
#include <QPainter>
#include <QBrush>
#include <QColor>

#include <iostream>

namespace Viewer
{

MultiSpectrumView::MultiSpectrumView(SpecTclInterface* pSpecTcl, QWidget *parent) :
    SpectrumView(parent),
    m_pLayout(new QGridLayout(this)),
    m_histMap(),
//    m_canvases(),
    m_pSpecTcl(pSpecTcl),
    m_pCurrentCanvas(new QRootCanvas),
    m_currentNRows(1),
    m_currentNColumns(1)
{
    setLayout(m_pLayout.get());

    m_pLayout->addWidget(m_pCurrentCanvas, 0, 0);

    connect(m_pCurrentCanvas, SIGNAL(mousePressed(QRootCanvas*)),
            this, SLOT(setCurrentCanvas(QRootCanvas*)));
}

int MultiSpectrumView::getRowCount() const
{
    return m_currentNRows;
}

int MultiSpectrumView::getColumnCount() const
{
    return m_currentNColumns;
}


void MultiSpectrumView::onGeometryChanged(int nRows, int nCols)
{
    // figure out the current configuation...
    if (nRows == m_pLayout->rowCount() && nCols == m_pLayout->columnCount()) {
        // if the same, do nothing
        return;
    }

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
               }
            } else if ( col >= nCols ) {
                auto pItem = m_pLayout->itemAtPosition(row, col);
                if (pItem) {
                    m_pLayout->removeItem(pItem);
                    delete pItem->widget();
                    delete pItem;
                }
            }

            // add empty spaces
            if (row >= currentNRows) {
                auto pCanvas = new QRootCanvas;
                m_pLayout->addWidget(pCanvas, row, col);
                connect(pCanvas, SIGNAL(mousePressed(QRootCanvas*)),
                        this, SLOT(setCurrentCanvas(QRootCanvas*)));
            } else if (col >= currentNCols) {
               auto pCanvas = new QRootCanvas;
                m_pLayout->addWidget(pCanvas, row, col);
                connect(pCanvas, SIGNAL(mousePressed(QRootCanvas*)),
                        this, SLOT(setCurrentCanvas(QRootCanvas*)));
            }
        }
    }

    m_currentNRows = nRows;
    m_currentNColumns = nCols;

    SpectrumView::update();
}

void MultiSpectrumView::setCurrentCanvas(QRootCanvas *pCanvas)
{
    m_pCurrentCanvas = pCanvas;
    m_pCurrentCanvas->cd();

    QPainter painter;
    QBrush brush;
    brush.setColor(QColor(10, 200, 10));
    painter.setBrush(brush);
    painter.drawRect(pCanvas->frameGeometry());
    QWidget::update();
}

QRootCanvas* MultiSpectrumView::getCurrentCanvas()
{
    return m_pCurrentCanvas;
}

void MultiSpectrumView::update(HistogramBundle* pBundle)
{
    if (pBundle) {
        if (pBundle->hist()) {
            pBundle->draw();
        }
    }
    refreshAll();
}

void MultiSpectrumView::refreshAll()
{
    // if different, then figure out how to change
    int currentNRows = m_pLayout->rowCount();
    int currentNCols = m_pLayout->columnCount();

    for (int row=0; row<currentNRows; ++row) {
        for (int col=0; col<currentNCols; ++col) {

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
}

void MultiSpectrumView::onHistogramRemoved(HistogramBundle *pHistBundle)
{
    std::cout << "Removed hist @ " << static_cast<void*>(pHistBundle) << std::endl;
}

//void MultiSpectrumView::mousePressEvent(QMouseEvent *pEvent)
//{
//    auto pos = pEvent->globalPos();

//    int currentNRows = m_pLayout->rowCount();
//    int currentNCols = m_pLayout->columnCount();

//    for (int row=0; row<currentNRows; ++row) {
//        for (int col=0; col<currentNCols; ++col) {

//            auto pItem = m_pLayout->itemAtPosition(row, col);
//            if (pItem) {
//                if (pItem->geometry().contains(pos)) {
//                    m_currentRow = row;
//                    m_currentColumn = col;
//                    break;
//                }
//            }
//        }
//    }

//    QWidget::mousePressEvent(pEvent);
//}

} // end of namespace
