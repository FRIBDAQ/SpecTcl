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
#include "DockableGateManager.h"
#include "ui_DockableGateManager.h"
#include "GateBuilderDialog.h"
#include "SpectrumViewer.h"
#include "QRootCanvas.h"
#include <QListWidget>
#include <QMessageBox>
#include "TCutG.h"

DockableGateManager::DockableGateManager(const SpectrumViewer& viewer, QWidget *parent) :
    QDockWidget(parent),
    ui(new Ui::DockableGateManager),
    m_view(viewer)
{
    ui->setupUi(this);
    connect(ui->addButton,SIGNAL(clicked()),this, SLOT(launchAddGateDialog()));
    connect(ui->editButton,SIGNAL(clicked()),this, SLOT(launchEditGateDialog()));
}

DockableGateManager::~DockableGateManager()
{
    delete ui;
}

void DockableGateManager::launchAddGateDialog()
{
    auto pCanvas = m_view.getCurrentFocus();
    auto histPkg = m_view.getCurrentHist();
    GateBuilderDialog* dialog = new GateBuilderDialog(*pCanvas, *histPkg);
    dialog->setAttribute(Qt::WA_DeleteOnClose);

    connect(dialog,SIGNAL(completed(TCutG*)),this,SLOT(registerGate(TCutG*)));
    connect(this, SIGNAL(finished(int)), this, SLOT(close()));

    dialog->show();
    dialog->raise();

}

void DockableGateManager::launchEditGateDialog()
{
    auto pCanvas = m_view.getCurrentFocus();
    auto histPkg = m_view.getCurrentHist();

    auto selection = ui->gateList->selectedItems();
    if (selection.size()==1) {
        QVariant cut = selection.at(0)->data(Qt::UserRole);
        TCutG* pCut = reinterpret_cast<TCutG*>(cut.value<void*>());
        GateBuilderDialog* dialog = new GateBuilderDialog(*pCanvas, *histPkg, pCut);
        dialog->setAttribute(Qt::WA_DeleteOnClose);

        connect(this, SIGNAL(finished(int)), this, SLOT(close()));

        dialog->show();
        dialog->raise();
    } else {
        QMessageBox::warning(0,"Invalid selection", "User must select one gate to edit.");
    }
}

void DockableGateManager::registerGate(TCutG* pCut)
{
    Q_ASSERT(pCut!=nullptr);

    QListWidgetItem* pItem = new QListWidgetItem(QString(pCut->GetName()),
                                                 ui->gateList,Qt::UserRole);
    QVariant var = QVariant::fromValue(reinterpret_cast<void*>(pCut));
    pItem->setData(Qt::UserRole, var);

    ui->gateList->addItem(pItem);
}


