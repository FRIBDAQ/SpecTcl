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

    ui->gridLayout->addWidget(m_pGeoSelector, 4, 0, 1, 2);

    connect(m_pSpecTcl.get(), SIGNAL(histogramListChanged()),
            this, SLOT(onHistogramListChanged()));


    connect(ui->histList,SIGNAL(doubleClicked(QModelIndex)),
            this,SLOT(onDoubleClick(QModelIndex)));

    connect(m_pGeoSelector, SIGNAL(rowCountChanged(int)), this, SLOT(onRowCountChanged(int)));
    connect(m_pGeoSelector, SIGNAL(columnCountChanged(int)), this, SLOT(onColumnCountChanged(int)));

    connect(ui->pFilterEdit, SIGNAL(textEdited(const QString&)),
            this, SLOT(applyFilter(const QString&)));
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
    if ( pBundle->getInfo().s_type == 1 ) {
        item.setIcon(QIcon(":/icons/spectrum1d-icon.png"));
    } else {
        item.setIcon(QIcon(":/icons/spectrum2d-icon.png"));
    }
}


//
//
void ViewDrawPanel::onDoubleClick(QModelIndex index)
{
    HistogramList* pHistList = m_pSpecTcl->getHistogramList();


    QListWidgetItem* pItem = ui->histList->item(index.row());

    if (pItem == nullptr) {
        return;
    }

    auto pHistBundle = pHistList->getHist(pItem->text());

    if (pHistBundle) {
        QString drawOption = ui->pDrawOptionEdit->text();
        if (! drawOption.isEmpty()) {
            pHistBundle->setDefaultDrawOption(drawOption);
        }
        emit histSelected(pHistBundle);
    } else {
        QMessageBox::warning(this, "Missing histogram",
                             "Failed to locate the selected histogram in the master histogram list.");
        return;
    }
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

            auto pHistBundle = pHistList->getHist(pItem->text());
            if (pHistBundle) {
                QString drawOption = ui->pDrawOptionEdit->text();
                if (! drawOption.isEmpty()) {
                    pHistBundle->setDefaultDrawOption(drawOption);
                }
                emit histSelected(pHistBundle);
            } else {
                QMessageBox::warning(this, "Missing histogram",
                                     "Failed to locate the selected histogram in the master histogram list.");
                return;
            }
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

} // end of namespace
