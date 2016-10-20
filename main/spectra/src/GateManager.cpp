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
#include "SpectrumView.h"
#include "SpecTclInterface.h"
#include "QRootCanvas.h"
#include "HistogramList.h"
#include "GSlice.h"
#include "GGate.h"
#include "MasterGateList.h"
#include "HistogramBundle.h"
#include "ControlPanel.h"

#include "SliceTableItem.h"

#include <QTableWidget>
#include <QMessageBox>
#include <QMutexLocker>
#include <QMutex>
#include <QList>
#include <QTimer>

#include <TH1.h>
#include <TH2.h>

#include <iostream>
#include <algorithm>
#include <functional>
#include <stdexcept>

#include "Benchmark.h"

using namespace std;

namespace Viewer
{

GateManager::GateManager(SpectrumView& view,
                         ControlPanel& controls,
                         std::shared_ptr<SpecTclInterface> pSpecTcl,
                         int nDimensions,
                         QWidget *parent) :
    QWidget(parent),
    ui(new Ui::GateManager),
    m_view(view),
    m_controls(controls),
    m_pSpecTcl(pSpecTcl),
    m_nRows(0),
    m_histDim(nDimensions)
{
    ui->setupUi(this);

    ui->gateList->setColumnCount(2);
    ui->gateList->setHorizontalHeaderItem(0, new QTableWidgetItem("Name"));
    ui->gateList->setHorizontalHeaderItem(1, new QTableWidgetItem("Integral"));

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

/*!
 * \brief Set the list of gates in the tablewidget
 *
 * \param gateNames     the names of the new that should be in list
 *
 * To avoid aesthetically unpleasant behavior such as changing the
 * selection as a side effect of an update, we will perform this
 * content update in an intelligent manner. The idea is to not touch
 * or remove any of the list items that are not being changed. The only
 * changes that will be made are to remove stale items and add new ones.
 */
void GateManager::setGateList(std::vector<QString> gateNames)
{
    ui->gateList->setColumnCount(2);

    // This _may_ be faster with a std::list<QString> b/c of O(1) insertion
    // deletion, but I am sticking with the general wisdom of c++ masters to
    // stick with a std::vector unless measurements say to do otherwise.
    std::vector<QString> existingNames;
    QTableWidgetItem* pItem = nullptr;

    existingNames.reserve(ui->gateList->rowCount());
    for (int row=0; row<ui->gateList->rowCount(); ++row) {
        pItem = ui->gateList->item(row, 0);
        if (pItem) {
            existingNames.push_back(pItem->text());
        }
    }

    // sort the vector so that we can use a binary_search to make it faster
    std::sort(existingNames.begin(), existingNames.end());
    std::sort(gateNames.begin(), gateNames.end());


    // add new entries to create the union of the two lists
    for (auto& name : gateNames) {
        if (! std::binary_search(existingNames.begin(), existingNames.end(), name)) {

            int row = ui->gateList->rowCount();
            ui->gateList->setRowCount(row+1);
            auto pItem = new QTableWidgetItem(name);
            ui->gateList->setItem(row, 0, pItem);

            pItem = new QTableWidgetItem(QString(tr("0")));
            ui->gateList->setItem(row, 1, pItem);

            m_nRows++;
        }
    }

    // remove entries that are in list that are not in gateNames
    for (int row=ui->gateList->rowCount(); row>=0; --row) {
        pItem = ui->gateList->item(row,0);
        if (pItem) {
            QString name = pItem->text();
            if (! std::binary_search(gateNames.begin(), gateNames.end(), name)) {
                ui->gateList->removeRow(row);
                m_nRows--;
            }
        }
    }
}


void GateManager::onDeleteButtonClicked()
{
    QList<QTableWidgetItem*> selected = ui->gateList->selectedItems();

    if (selected.size() == 0) {
        QMessageBox::warning(this, "Gate deletion failure",
                             "User must select a gate to delete.");
        return;
    }

    // Figure out the rows to delete without deleting them. This is a bit overkill
    // because at the moment we only support selecting a single row at a time.
    // In case this changes, we will support the general case of having multiple
    // rows. We will store them in a map that has reverse order (biggest row at front
    // and lowest row at back)
    std::map<int, QString, std::greater<int> > rowsToDelete;
    for (int i=0; i<selected.size(); ++i) {
        QTableWidgetItem* pItem = selected.at(i);

        if (! pItem) {
            throw std::runtime_error("GateManager::onDeleteButtonClicked() Selected item is a nullptr.");
        }

        // there are two cells per row. We don't want to concern ourselves with the
        // cell containing the integral. Rather we just want the first column that contains
        // the name of the gate
        if (pItem->column() == 0) {
            rowsToDelete[pItem->row()] = pItem->text();
        }
    }

    // note that this essentially removing rows last first becuase
    // of the definition of the map
    for (auto& rowInfo : rowsToDelete) {
        if (m_pSpecTcl) {
            MasterGateList* pGateList = m_pSpecTcl->getGateList();
            if (m_histDim == 1) {
                pGateList->removeCut1D(std::get<1>(rowInfo));
            } else {
                pGateList->removeCut2D(std::get<1>(rowInfo));
            }

            m_pSpecTcl->deleteGate(std::get<1>(rowInfo));

        }
    }

}

/*! \brief Connect the button signals to appropriate slots in this class */
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

    // Integration is slightly different for 1d and 2d histograms
    // because the TH2s must be integrated by the TCutG:IntregralHist() method.
    // The TH1::Integral takes on a single bin range.
    if (is2D) {
        // 2d
        update2DIntegrals(rHistPkg);
    } else {
        // 1d
        update1DIntegrals(rHistPkg);
    }
}

/*!
 * \brief GateManager::update1DIntegrals
 * \param rHistPkg  - the histogram bundle containing the hist to integrate
 *
 * This is very straight forward. The slice is located and its low and high
 * limits are used to integrate the histogram. The results of the integration
 * are set as the text in the table.
 */
void GateManager::update1DIntegrals(HistogramBundle& rHistPkg)
{
    QMutexLocker guard(rHistPkg.getMutex());

    TH1& hist = rHistPkg.getHist();

    std::map<QString, GSlice*> cuts = rHistPkg.getCut1Ds();
    int nRows = std::min(size_t(m_nRows), cuts.size());
    for (int row=0; row<nRows; ++row) {

        QTableWidgetItem* pNameItem = ui->gateList->item(row, 0);
        QTableWidgetItem* pValueItem = ui->gateList->item(row, 1);

        auto it = cuts.find(pNameItem->text());
        if (it != cuts.end()) {
            GSlice* pSlice = it->second;
            double low = pSlice->getXLow();
            double high = pSlice->getXHigh();

            double integral = hist.Integral(hist.FindBin(low), hist.FindBin(high));

            pValueItem->setText(QString::number(integral));
        }

    }
}

/*!
 * \brief GateManager::update2DIntegrals
 * \param rHistPkg histogram bundle containing the hist to integrate
 *
 * This is very similar to the behavior of the update1DIntegrals except we
 * have to rely on the TCutG::IntegralHist() method.
 */
void GateManager::update2DIntegrals(HistogramBundle& rHistPkg)
{
    QMutexLocker guard(rHistPkg.getMutex());

    // if this is a bad cast, a std::bad_cast is thrown by the dynamic_cast
    // and the system will become very unhappy. This is not going to happen though
    // because we have already used ROOT introspection API to determine that
    // the histogram derives from TH2.
    TH2& hist = dynamic_cast<TH2&>(rHistPkg.getHist());

    std::map<QString, GGate*> cuts = rHistPkg.getCut2Ds();
    int nRows = std::min(size_t(m_nRows), cuts.size());
    for (int row=0; row<nRows; ++row) {
        QTableWidgetItem* pNameItem = ui->gateList->item(row, 0);
        QTableWidgetItem* pValueItem = ui->gateList->item(row, 1);

        auto it = cuts.find(pNameItem->text());
        if (it != cuts.end()) {
            GGate* pGate = it->second;
            MyCutG* pGrObj = pGate->getGraphicObject();

            if (pGrObj) {
                double integral = pGrObj->IntegralHist(&hist);

                pValueItem->setText(QString::number(integral));
            } else {
                pValueItem->setText("--");
            }
        } else {
            pValueItem->setText("--");
        }

    }
}


} // end of namespace
