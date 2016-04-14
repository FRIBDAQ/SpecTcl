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

#include "GateManager.h"
#include "ui_GateManager.h"
#include "GateBuilderDialog.h"
#include "GateBuilder1DDialog.h"
#include "SpectrumView.h"
#include "SpecTclInterface.h"
#include "QRootCanvas.h"
#include "HistogramList.h"
#include "GSlice.h"
#include "GGate.h"
#include "MasterGateList.h"
#include "HistogramBundle.h"

#include "SliceTableItem.h"
#include "GateListItem.h"

#include <QTableWidget>
#include <QMessageBox>
#include <QMutexLocker>
#include <QMutex>

#include <TH1.h>
#include <TH2.h>

#include <iostream>
#include <functional>

#include <chrono>
#include "Benchmark.h"

using namespace std;

namespace Viewer
{

GateManager::GateManager(SpectrumView& view,
                                         std::shared_ptr<SpecTclInterface> pSpecTcl,
                                         QWidget *parent) :
    QWidget(parent),
    ui(new Ui::GateManager),
    m_view(view),
    m_pSpecTcl(pSpecTcl)
{
    ui->setupUi(this);

    ui->gateList->setVerticalHeaderLabels({QString("Name"), QString("Integral")});

    connectSignals();
}

GateManager::~GateManager()
{
    delete ui;
}

void GateManager::setSpecTclInterface(std::shared_ptr<SpecTclInterface> pSpecTcl)
{
    m_pSpecTcl = pSpecTcl;
}


void GateManager::onAddButtonClicked() {
    emit addGateClicked();
}

void GateManager::onEditButtonClicked() {
    emit editGateClicked();
}


void GateManager::setGateList(const std::vector<QString> &gateNames)
{
    ui->gateList->setColumnCount(2);

    QTableWidgetItem* pItem = nullptr;
    while ( ui->gateList->rowCount() > 0) {
        ui->gateList->removeRow(0);
    }

    for (auto& name : gateNames) {

        int row = ui->gateList->rowCount();
        ui->gateList->setRowCount(row+1);
        auto pItem = new QTableWidgetItem(name);
        ui->gateList->setItem(row, 0, pItem);

        pItem = new QTableWidgetItem(QString(tr("0")));
        ui->gateList->setItem(row, 1, pItem);
    }
}


void GateManager::onDeleteButtonClicked()
{
    auto selected = ui->gateList->selectedItems();
    for ( auto pItem : selected ) {

      if (m_pSpecTcl) {
        m_pSpecTcl->deleteGate(pItem->text());
      }

      // Remove the row for now... the next time someone updates,
      // we will see this deletion reflected more properly.
      auto row = ui->gateList->row(pItem);
      ui->gateList->removeRow(row);

    }
}

void GateManager::connectSignals()
{
  connect(ui->addButton, SIGNAL(clicked()),
          this, SLOT(onAddButtonClicked()));

  connect(ui->editButton, SIGNAL(clicked()),
          this, SLOT(onEditButtonClicked()));

  connect(ui->deleteButton, SIGNAL(clicked()),
          this, SLOT(onDeleteButtonClicked()));

}


QList<QTableWidgetItem*> GateManager::getSelectedItems() const
{
    return ui->gateList->selectedItems();
}


void GateManager::updateGateIntegrals(HistogramBundle &rHistPkg)
{

    bool is2D = false;

    rHistPkg.getMutex()->lock();
    is2D = rHistPkg.getHist().InheritsFrom(TH2::Class());
    rHistPkg.getMutex()->unlock();

    if (is2D) {
        // 2d
        update2DIntegrals(rHistPkg);
    } else {
        // 1d
        update1DIntegrals(rHistPkg);
    }

}

void GateManager::update1DIntegrals(HistogramBundle& rHistPkg)
{
    QMutexLocker guard(rHistPkg.getMutex());

    TH1& hist = rHistPkg.getHist();

    std::map<QString, GSlice*> cuts = rHistPkg.getCut1Ds();
    int nRows = ui->gateList->rowCount();
    for (int row=0; row<nRows; ++row) {

        QTableWidgetItem* pNameItem = ui->gateList->item(row, 0);
        QTableWidgetItem* pValueItem = ui->gateList->item(row, 1);

        auto it = cuts.find(pNameItem->text());
        if (it != cuts.end()) {
            GSlice* pSlice = it->second;
            double integral = hist.Integral(pSlice->getXLow(), pSlice->getXHigh());

            pValueItem->setText(QString::number(integral));
        }

    }
}

void GateManager::update2DIntegrals(HistogramBundle& rHistPkg)
{
    QMutexLocker guard(rHistPkg.getMutex());

    TH2& hist = dynamic_cast<TH2&>(rHistPkg.getHist());


    std::map<QString, GGate*> cuts = rHistPkg.getCut2Ds();
    int nRows = ui->gateList->rowCount();
    for (int row=0; row<nRows; ++row) {

        QTableWidgetItem* pNameItem = ui->gateList->item(row, 0);
        QTableWidgetItem* pValueItem = ui->gateList->item(row, 1);

        auto it = cuts.find(pNameItem->text());
        if (it != cuts.end()) {
            GGate* pGate = it->second;

            double integral = pGate->getGraphicObject()->IntegralHist(&hist);

            pValueItem->setText(QString::number(integral));
        } else {
            pValueItem->setText("--");
        }

    }
}


} // end of namespace
