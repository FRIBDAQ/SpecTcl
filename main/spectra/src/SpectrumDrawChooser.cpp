#include "SpectrumDrawChooser.h"
#include "ui_SpectrumDrawChooser.h"

#include "ui_SpectrumDrawChooser.h"
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
#include <stdexcept>

using namespace std;

namespace Viewer
{

SpectrumDrawChooser::SpectrumDrawChooser(std::shared_ptr<SpecTclInterface> pSpecTcl, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SpectrumDrawChooser),
    m_pSpecTcl(pSpecTcl)
{
    ui->setupUi(this);

    if (! connect(m_pSpecTcl.get(), SIGNAL(histogramListChanged()),
                  this, SLOT(onHistogramListChanged())) ) {
        throw runtime_error("SpectrumDrawChooser() failed to connect SpecTclInterface::histogramListChanged() to slot");
    }

    if (! connect(ui->pHistList,SIGNAL(doubleClicked(QModelIndex)),
                  this,SLOT(onDoubleClick(QModelIndex))) ) {
        throw runtime_error("SpectrumDrawChooser() failed to connect HistogramList::doubleClicked() to slot");
    }

    if (! connect(ui->pDrawButton,SIGNAL(clicked()),
                  this,SLOT(onDrawClicked())) ) {
        throw runtime_error("SpectrumDrawChooser() failed to connect HistogramList::onDrawClicked() to slot");
    }

}

//
//
SpectrumDrawChooser::~SpectrumDrawChooser()
{
}

//
//
void SpectrumDrawChooser::setSpecTclInterface(std::shared_ptr<SpecTclInterface> pSpecTcl)
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
        throw runtime_error("SpectrumDrawChooser() failed to connect SpecTclInterface::histogramListChanged() to slot");
    }

}

//
//
void SpectrumDrawChooser::onHistogramListChanged()
{
  auto pHistList = m_pSpecTcl->getHistogramList();
  synchronize(pHistList);
}

// Synchronization is a two step process
// 1. Add all entries that are missing in the current local list
// 2. Remove all entries that are in current local list but not in master list
void SpectrumDrawChooser::synchronize(HistogramList *pHistList)
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
void SpectrumDrawChooser::appendEntry(const QString& name, HistogramList::iterator it)
{
    // Histograms are uniquely named, so we can use the name as the key
    auto item = new QListWidgetItem(name, ui->pHistList,
                                    QListWidgetItem::UserType);

    // store a point to the histogram bundle
    item->setData(Qt::UserRole,
                  QVariant::fromValue<void*>(reinterpret_cast<void*>(it->second.get())));
    setIcon(item);

    QSize geo = ui->pHistList->size();
    ui->pHistList->insertItem(geo.height(), item);
}

//
//
void SpectrumDrawChooser::updateEntry(HistogramList::iterator it, const QString& name)
{
    auto items = ui->pHistList->findItems(name, Qt::MatchExactly);
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
void SpectrumDrawChooser::removeStaleEntries(HistogramList* pHistList)
{
    int nRows = ui->pHistList->count();
    for (int row=nRows-1; row>=0; --row) {

        auto pItem = ui->pHistList->item(row);

        if ( ! pHistList->histExists(pItem->text()) ) {
            delete (ui->pHistList->takeItem(row));
        }
    }
}


//
//
void SpectrumDrawChooser::setIcon(QListWidgetItem *pItem)
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
void SpectrumDrawChooser::setList(std::vector<SpJs::HistInfo> names)
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
                                            ui->pHistList,
                                            QListWidgetItem::UserType);

            item->setData(Qt::UserRole,QVariant(name));

            QSize geo = ui->pHistList->size();
            ui->pHistList->insertItem(geo.height(), item);

        }

      ++iter;
    }

}

//
//
void SpectrumDrawChooser::onDoubleClick(QModelIndex index)
{
    auto pHistBundle = reinterpret_cast<HistogramBundle*>(index.data(Qt::UserRole).value<void*>());
    emit histSelected(pHistBundle);
}

void SpectrumDrawChooser::onDrawClicked()
{
    QList<QListWidgetItem*> selected = ui->pHistList->selectedItems();

    for (auto& item : selected) {
        std::cout << item->text().toStdString() << std::endl;
    }
}

//
//
bool SpectrumDrawChooser::histExists(const QString& name)
{
  size_t nRows = ui->pHistList->count();
  return (binarySearch(0, nRows-1, name) != -1);
}

//
//
int SpectrumDrawChooser::binarySearch(int min, int max, const QString& name)
{
  int pivot = min + (max-min)/2;
  if ((max < min) || (min < 0)) return -1;

  auto pivotName = ui->pHistList->item(pivot)->text();
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
void SpectrumDrawChooser::deleteHists()
{
    size_t nEntries = ui->pHistList->count();
    for(size_t  entry=0; entry<nEntries; ++entry) {
        QListWidgetItem* item = ui->pHistList->item(entry);
        TH1* hist = (TH1*)(item->data(Qt::UserRole).value<void*>());
        delete hist;
    }
}

} // end of namespace
