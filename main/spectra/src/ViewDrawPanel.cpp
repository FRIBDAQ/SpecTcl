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

#include "ViewDrawPanel.h"
#include "ui_ViewDrawPanel.h"

#include "HistogramList.h"
#include "HistogramBundle.h"
#include "SpecTclInterface.h"
#include "GeometrySelector.h"

#include <QTimer>
#include <QList>
#include <QListWidgetItem>
#include <QRegExp>
#include <QMessageBox>
#include <QKeyEvent>
#include <QComboBox>
#include <QLabel>

#include <vector>
#include <iostream>
#include <chrono>
#include <stdexcept>

using namespace std;

namespace Viewer
{

ViewDrawPanel::ViewDrawPanel(std::shared_ptr<SpecTclInterface> pSpecTcl, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ViewDrawPanel),
    m_pSpecTcl(pSpecTcl),
    m_pGeoSelector(new GeometrySelector(this))
{
    ui->setupUi(this);

    QVBoxLayout* pBottomLayout = new QVBoxLayout(this);
    QLabel* pLabel = new QLabel("<h2>Tab Geometry</h2>", this);
    pBottomLayout->addWidget(pLabel);
    pBottomLayout->addWidget(m_pGeoSelector);

    ui->verticalLayout_3->addSpacing(18);
    ui->verticalLayout_3->addLayout(pBottomLayout);

    ui->pDrawOptCombo->setEditable(true);

    connect(m_pSpecTcl.get(), SIGNAL(histogramListChanged()),
            this, SLOT(onHistogramListChanged()));


    connect(ui->histList,SIGNAL(doubleClicked(QModelIndex)),
            this,SLOT(onDoubleClick(QModelIndex)));
    connect(ui->histList,SIGNAL(itemSelectionChanged()),this,SLOT(onSelectionChanged()));

    connect(m_pGeoSelector, SIGNAL(rowCountChanged(int)), this, SLOT(onRowCountChanged(int)));
    connect(m_pGeoSelector, SIGNAL(columnCountChanged(int)), this, SLOT(onColumnCountChanged(int)));

    connect(ui->pFilterEdit, SIGNAL(textEdited(const QString&)),
            this, SLOT(applyFilter(const QString&)));

    connect(ui->pDrawButton,SIGNAL(clicked()), this, SLOT(onDrawClicked()));
}

//
//
ViewDrawPanel::~ViewDrawPanel()
{
}

//
//
void ViewDrawPanel::setSpecTclInterface(std::shared_ptr<SpecTclInterface> pSpecTcl)
{
    if (m_pSpecTcl) {
        if (! disconnect(m_pSpecTcl.get(), SIGNAL(histogramListChanged()),
                   this, SLOT(onHistogramListChanged())) ) {
            std::cout << "Failed to disconnect SpecTclInterface::histogramListChanged() from slot" << std::endl;
        }
    }

    m_pSpecTcl = pSpecTcl;


    if (! connect(m_pSpecTcl.get(), SIGNAL(histogramListChanged()),
            this, SLOT(onHistogramListChanged())) ) {
        throw runtime_error("ViewDrawPanel() failed to connect SpecTclInterface::histogramListChanged() to slot");
    }

}

//
//
void ViewDrawPanel::onHistogramListChanged()
{
  auto pHistList = m_pSpecTcl->getHistogramList();
  setHistogramList(pHistList);
}

// Synchronization is a two step process
// 1. Add all entries that are missing in the current local list
// 2. Remove all entries that are in current local list but not in master list
void ViewDrawPanel::setHistogramList(HistogramList *pHistList)
{

    clearHistogramList();

    QString filter = ui->pFilterEdit->text();

    if (filter.isEmpty() || (filter.at(filter.length()-1) != QChar('*'))) {
        filter += "*";
    }

    QRegExp matcher(filter);
    matcher.setPatternSyntax(QRegExp::Wildcard);

    QMutexLocker lock(pHistList->getMutex());

    auto it    = pHistList->begin();
    auto itend = pHistList->end();

    // add new histograms if they have changed
    while (it!=itend) {

        const QString& name = it->first;
        if ( matcher.exactMatch(name) ) {
            appendHistogramToList(it);
        }
        ++it;
    }

}

void ViewDrawPanel::applyFilter(const QString& filter)
{
    setHistogramList(m_pSpecTcl->getHistogramList());
}

void ViewDrawPanel::clearHistogramList()
{
    QListWidgetItem* pItem = nullptr;
    while (( pItem = ui->histList->takeItem(0) )) {
        delete pItem;
    }
}

void ViewDrawPanel::appendHistogramToList(HistogramList::iterator it)
{
    auto pItem = new QListWidgetItem(it->first, ui->histList);
    setIcon(*pItem, it);
}

//
//
void ViewDrawPanel::setIcon(QListWidgetItem& item, HistogramList::iterator it)
{
    std::unique_ptr<HistogramBundle>& pBundle = it->second;

    QMutexLocker lock(pBundle->getMutex());
    if ( pBundle->getInfo().s_type == "1" ) { // 1d
        item.setIcon(QIcon(":/icons/spectrum1d-icon.png"));
    } else if (pBundle->getInfo().s_type == "2"){ // 2d
        item.setIcon(QIcon(":/icons/spectrum2d-icon.png"));
    } else if (pBundle->getInfo().s_type == "s") { // summary
        item.setIcon(QIcon(":/icons/spectrumsummary-icon.png"));
    } else if (pBundle->getInfo().s_type == "g1") { // gamma 1d
        item.setIcon(QIcon(":/icons/spectrumg1-icon.png"));
    } else if (pBundle->getInfo().s_type == "g2") { // gamma 2d
        item.setIcon(QIcon(":/icons/spectrumg2-icon.png"));
    } else if (pBundle->getInfo().s_type == "gs") { // gamma summary
        item.setIcon(QIcon(":/icons/spectrumgs-icon.png"));
    } else if (pBundle->getInfo().s_type == "S") { // stripchart
        item.setIcon(QIcon(":/icons/spectrumst-icon.png"));
    } else if (pBundle->getInfo().s_type == "gd") { // gamma deluxe
        item.setIcon(QIcon(":/icons/spectrumgd-icon.png"));
    } else if (pBundle->getInfo().s_type == "b") { // bitmap
        item.setIcon(QIcon(":/icons/spectrumbi-icon.png"));
    }
}


//
//
void ViewDrawPanel::onDoubleClick(QModelIndex index)
{

    QListWidgetItem* pItem = ui->histList->item(index.row());

    if (pItem == nullptr) {
        return;
    }

    initiateDraw(*pItem);
}

//
//
void ViewDrawPanel::onDrawClicked()
{
    QList<QListWidgetItem*> items = ui->histList->selectedItems();

    // there should only be a single selection but we will treat it
    // generally here in case something is to change.
    for (auto& pItem : items) {
        if (pItem) {
            initiateDraw(*pItem);
        }
    }

}

/*!
 * \brief Common code for drawing a selected histogram
 * \param item - the selected histogram
 */
void ViewDrawPanel::initiateDraw(QListWidgetItem& item) {
    HistogramList* pHistList = m_pSpecTcl->getHistogramList();

    pHistList->lock();

    auto pHistBundle = pHistList->getHist(item.text());

    if (pHistBundle) {

        QString drawOption = ui->pDrawOptCombo->currentText();
        
        
        if (ui->pSuperimposeSelect->isEnabled() && ui->pSuperimposeSelect->isChecked()) {
            drawOption += " same";
        }
        emit histSelected(pHistBundle, drawOption);
    } else {
        QMessageBox::warning(this, "Missing histogram",
                             "Failed to locate the selected histogram in the master histogram list.");
    }
    pHistList->unlock();

}

void ViewDrawPanel::keyPressEvent(QKeyEvent* pEvent)
{

    if (pEvent->key() == Qt::Key_Enter || pEvent->key() == Qt::Key_Return) {
        if (ui->histList->hasFocus()) {
            HistogramList* pHistList = m_pSpecTcl->getHistogramList();


            QListWidgetItem* pItem = ui->histList->currentItem();

            if (pItem == nullptr) {
                return;
            }

            // protect the histogram list
            pHistList->lock();

            auto pHistBundle = pHistList->getHist(pItem->text());

            if (pHistBundle) {
                QString drawOption = ui->pDrawOptCombo->currentText();
                emit histSelected(pHistBundle, drawOption);
            } else {
                QMessageBox::warning(this, "Missing histogram",
                                     "Failed to locate the selected histogram in the master histogram list.");
            }

            pHistList->unlock();

        }
    }
}

void ViewDrawPanel::onRowCountChanged(int nRows)
{
  emit geometryChanged(nRows, m_pGeoSelector->getColumnCount());
}

void ViewDrawPanel::onColumnCountChanged(int nColumns)
{
  emit geometryChanged(m_pGeoSelector->getRowCount(), nColumns);
}

void ViewDrawPanel::setZoomedState(bool state)
{
    m_pGeoSelector->setDisabled(state);
}

void ViewDrawPanel::onSelectionChanged()
{
    QList<QListWidgetItem*> items = ui->histList->selectedItems();

    // do not continue if we have no selected items
    if (items.count() == 0) return;

    QListWidgetItem* pCurrent = items.at(0);

    HistogramList* pHistList = m_pSpecTcl->getHistogramList();

    pHistList->lock();

    HistogramBundle* pCurrentBundle = pHistList->getHist(pCurrent->text());
    if (pCurrentBundle) {
        pCurrentBundle->lock();
        int dimension = pCurrentBundle->getHist().InheritsFrom(TH2::Class()) ? 2 : 1;
        pCurrentBundle->unlock();

        if (dimension != m_currentDimension) {
            setDrawOptions(dimension);
            m_currentDimension = dimension;
        }

        if (dimension == 1) {
            ui->pSuperimposeSelect->setEnabled(true);
        } else {
            ui->pSuperimposeSelect->setEnabled(false);
        }
    }

    pHistList->unlock();
}

void ViewDrawPanel::setDrawOptions(int dimension)
{
    QComboBox* pBox = ui->pDrawOptCombo;
    pBox->clear();

    if (dimension == 1) {
        pBox->addItems({"hist","histE"});
    } else {
        pBox->addItems({"col2","colz2","colz", "col","lego","lego1", "lego2", "surf", "surf1", "surf2"});
    }

    pBox->setCurrentIndex(0);

}

} // end of namespace
