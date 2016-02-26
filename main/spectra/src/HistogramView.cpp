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
#include "HistogramView.h"
#include "ui_HistogramView.h"
#include "ListRequestHandler.h"
#include "HistogramList.h"
#include "HistogramBundle.h"
#include "SpecTclRESTInterface.h"
#include "QHistInfo.h"
#include "Benchmark.h"

#include <HistFactory.h>

#include <TH1.h>

#include <QTimer>
#include <QList>
#include <QListWidgetItem>

#include <vector>
#include <iostream>
#include <chrono>

using namespace std;

namespace Viewer
{

HistogramView::HistogramView(std::shared_ptr<SpecTclInterface> pSpecTcl, QWidget *parent) :
    QDockWidget(tr("Histograms"),parent),
    ui(new Ui::HistogramView),
    m_req(new ListRequestHandler(this)),
    m_pSpecTcl(pSpecTcl)
{
    ui->setupUi(this);


    connect(m_pSpecTcl.get(), SIGNAL(histogramListChanged()),
            this, SLOT(onHistogramListChanged()));

    connect(ui->histList,SIGNAL(doubleClicked(QModelIndex)),
            this,SLOT(onDoubleClick(QModelIndex)));
}

HistogramView::~HistogramView()
{
  delete ui;
}

void HistogramView::setSpecTclInterface(std::shared_ptr<SpecTclInterface> pSpecTcl)
{
    m_pSpecTcl = pSpecTcl;
}

void HistogramView::onHistogramListChanged()
{

  auto pHistList = m_pSpecTcl->getHistogramList();

  // Synchronize access to the histogram list
  QMutexLocker lock(pHistList->getMutex());

  auto it = pHistList->begin();
  auto itend = pHistList->end();

  // add new histograms if they have changed
  while (it!=itend) {

      const QString& name = it->first;
      if (! histExists(name)) {

          // Histograms are uniquely named, so we can use the name as the key
          auto item = new QListWidgetItem(name, ui->histList,
                                          QListWidgetItem::UserType);

          // store a point to the histogram bundle
          item->setData(Qt::UserRole,
                        QVariant::fromValue<void*>(reinterpret_cast<void*>(it->second.get())));
          setIcon(item);

          QSize geo = ui->histList->size();
          ui->histList->insertItem(geo.height(), item);
      } else {

          // the value already exists...get the ListWidgetItem associated with it
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

    ++it;
  }

  // now remove stale items

  int nRows = ui->histList->count();
  for (int row=nRows-1; row>=0; --row) {

      auto pItem = ui->histList->item(row);

      if ( ! pHistList->histExists(pItem->text()) ) {
          delete (ui->histList->takeItem(row));
      }
  }

}

void HistogramView::setIcon(QListWidgetItem *pItem)
{
  auto pBundle = reinterpret_cast<HistogramBundle*>(pItem->data(Qt::UserRole).value<void*>());
  if ( pBundle->getInfo().s_type == 1 ) {
    pItem->setIcon(QIcon(":/icons/spectrum1d-icon.png"));
  } else {
    pItem->setIcon(QIcon(":/icons/spectrum2d-icon.png"));
  }
}

void HistogramView::setList(std::vector<SpJs::HistInfo> names)
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

void HistogramView::onDoubleClick(QModelIndex index)
{
    auto pHistBundle = reinterpret_cast<HistogramBundle*>(index.data(Qt::UserRole).value<void*>());
    emit histSelected(pHistBundle);
}


bool HistogramView::histExists(const QString& name)
{
  size_t nRows = ui->histList->count();
  return (binarySearch(0, nRows-1, name) != -1);
}

int HistogramView::binarySearch(int min, int max, const QString& name)
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


void HistogramView::deleteHists()
{
    size_t nEntries = ui->histList->count();
    for(size_t  entry=0; entry<nEntries; ++entry) {
        QListWidgetItem* item = ui->histList->item(entry);
        TH1* hist = (TH1*)(item->data(Qt::UserRole).value<void*>());
        delete hist;
    }
}

} // end of namespace
