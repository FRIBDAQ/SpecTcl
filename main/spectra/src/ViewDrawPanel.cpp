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

#include "ListRequestHandler.h"
#include "HistogramList.h"
#include "HistogramBundle.h"
#include "SpecTclRESTInterface.h"
#include "QHistInfo.h"
#include "Benchmark.h"
#include "GeometrySelector.h"

#include <HistFactory.h>

#include <TH1.h>

#include <QTimer>
#include <QList>
#include <QListWidgetItem>

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

    ui->verticalLayout->addWidget(m_pGeoSelector);

    if (! connect(m_pSpecTcl.get(), SIGNAL(histogramListChanged()),
                  this, SLOT(onHistogramListChanged())) ) {
        throw runtime_error("ViewDrawPanel() failed to connect SpecTclInterface::histogramListChanged() to slot");
    }

    if (! connect(ui->histList,SIGNAL(doubleClicked(QModelIndex)),
                  this,SLOT(onDoubleClick(QModelIndex))) ) {
        throw runtime_error("ViewDrawPanel() failed to connect HistogramList::doubleClicked() to slot");
    }

    connect(m_pGeoSelector, SIGNAL(rowCountChanged(int)), this, SLOT(onRowCountChanged(int)));
    connect(m_pGeoSelector, SIGNAL(columnCountChanged(int)), this, SLOT(onColumnCountChanged(int)));

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
  synchronize(pHistList);
}

// Synchronization is a two step process
// 1. Add all entries that are missing in the current local list
// 2. Remove all entries that are in current local list but not in master list
void ViewDrawPanel::synchronize(HistogramList *pHistList)
{
    QMutexLocker lock(pHistList->getMutex());

    auto it    = pHistList->begin();
    auto itend = pHistList->end();

    // add new histograms if they have changed
    while (it!=itend) {

        const QString& name = it->first;
        if (! histExists(name)) {
            appendEntry(name, it);
        } else {
            updateEntry(it, name);
        }

        ++it;
    }

    removeStaleEntries(pHistList);
}

//
//
void ViewDrawPanel::appendEntry(const QString& name, HistogramList::iterator it)
{
    // Histograms are uniquely named, so we can use the name as the key
    auto item = new QListWidgetItem(name, ui->histList,
                                    QListWidgetItem::UserType);

    // store a point to the histogram bundle
    item->setData(Qt::UserRole,
                  QVariant::fromValue<void*>(reinterpret_cast<void*>(it->second.get())));
    setIcon(item);

    QSize geo = ui->histList->size();
    ui->histList->insertItem(geo.height(), item);
}

//
//
void ViewDrawPanel::updateEntry(HistogramList::iterator it, const QString& name)
{
    auto items = ui->histList->findItems(name, Qt::MatchExactly);
    // make sure we found something
    if ( items.size() == 1 ) {
        // get the first and only item found
        auto pItem = items.at(0);

        pItem->setData(Qt::UserRole,
                       QVariant::fromValue<void*>(reinterpret_cast<void*>(it->second.get())));
        setIcon(pItem);
    }
}

//
//
void ViewDrawPanel::removeStaleEntries(HistogramList* pHistList)
{
    int nRows = ui->histList->count();
    for (int row=nRows-1; row>=0; --row) {

        auto pItem = ui->histList->item(row);

        if ( ! pHistList->histExists(pItem->text()) ) {
            delete (ui->histList->takeItem(row));
        }
    }
}


//
//
void ViewDrawPanel::setIcon(QListWidgetItem *pItem)
{
  auto pBundle = reinterpret_cast<HistogramBundle*>(pItem->data(Qt::UserRole).value<void*>());
  if ( pBundle->getInfo().s_type == 1 ) {
    pItem->setIcon(QIcon(":/icons/spectrum1d-icon.png"));
  } else {
    pItem->setIcon(QIcon(":/icons/spectrum2d-icon.png"));
  }
}

//
//
void ViewDrawPanel::setList(std::vector<SpJs::HistInfo> names)
{
    SpJs::HistFactory factory;

    auto iter = names.begin();
    auto end = names.end();

    while (iter!=end) {

        SpJs::HistInfo& info = (*iter);
        QString name = QString::fromStdString(info.s_name);
        if (!histExists(name)) {

            // this should  get a unique ptr, we want to get the raw pointer
            // and strip the unique_ptr of its ownership (i.e. release())
            auto upHist = factory.create(info);

            auto pHistList = m_pSpecTcl->getHistogramList();

            // Synchronization for list
            QMutexLocker listLock(pHistList->getMutex());

            auto pHist = pHistList->addHist(std::move(upHist), info);

            // Synchronization for histogram
            QMutexLocker histLock(pHist->getMutex());

            // a new hist needs to find the necessary
            pHist->synchronizeGates(m_pSpecTcl->getGateList());

            // Histograms are uniquely named, so we can use the name as the key
            QString name = QString::fromStdString((*iter).s_name);

            auto item = new QListWidgetItem(name,
                                            ui->histList,
                                            QListWidgetItem::UserType);

            item->setData(Qt::UserRole,QVariant(name));

            QSize geo = ui->histList->size();
            ui->histList->insertItem(geo.height(), item);

        }

      ++iter;
    }

}

//
//
void ViewDrawPanel::onDoubleClick(QModelIndex index)
{
    auto pHistBundle = reinterpret_cast<HistogramBundle*>(index.data(Qt::UserRole).value<void*>());
    emit histSelected(pHistBundle);
}

void ViewDrawPanel::onDrawClicked()
{
    QList<QListWidgetItem*> selected = ui->histList->selectedItems();

    for (auto& item : selected) {
        std::cout << item->text().toStdString() << std::endl;
    }
}

//
//
bool ViewDrawPanel::histExists(const QString& name)
{
  size_t nRows = ui->histList->count();
  return (binarySearch(0, nRows-1, name) != -1);
}

//
//
int ViewDrawPanel::binarySearch(int min, int max, const QString& name)
{
  int pivot = min + (max-min)/2;
  if ((max < min) || (min < 0)) return -1;

  auto pivotName = ui->histList->item(pivot)->text();
  if (pivotName == name) {
      return pivot;
    } else if (pivotName < name) {
      return binarySearch(pivot+1, max, name);
    } else if (pivotName > name) {
      return binarySearch(min, pivot-1, name);
    } else {
      return -1;
    }

}


//
//
void ViewDrawPanel::deleteHists()
{
    size_t nEntries = ui->histList->count();
    for(size_t  entry=0; entry<nEntries; ++entry) {
        QListWidgetItem* item = ui->histList->item(entry);
        TH1* hist = (TH1*)(item->data(Qt::UserRole).value<void*>());
        delete hist;
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


} // end of namespace
