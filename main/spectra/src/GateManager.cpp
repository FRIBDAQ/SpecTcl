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

#include "SliceTableItem.h"
#include "GateListItem.h"

#include <QListWidget>
#include <QMessageBox>

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


void GateManager::setGateList(const std::vector<QString> &gates)
{
    QListWidgetItem* pItem = nullptr;
    while (( pItem = ui->gateList->takeItem(0) )) {
        delete pItem;
    }

    for (auto& name : gates) {
        ui->gateList->addItem(name);
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
      ui->gateList->takeItem(row);

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


QList<QListWidgetItem*> GateManager::getSelectedItems() const
{
    return ui->gateList->selectedItems();
}

} // end of namespace
