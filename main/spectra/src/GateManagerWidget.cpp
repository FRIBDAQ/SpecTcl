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

#include "GateManagerWidget.h"
#include "GateManager.h"
#include "OneDimGateEdit.h"
#include "TwoDimGateEdit.h"
#include "SpectrumView.h"
#include "SpecTclInterface.h"
#include "HistogramList.h"
#include "SliceTableItem.h"
#include "GGate.h"
#include "GSlice.h"
#include "ControlPanel.h"

#include "TH2.h"

#include <QHBoxLayout>
#include <QMessageBox>
#include <QTimer>
#include <QMutexLocker>
#include <QMutex>
#include <QTableWidgetItem>

#include <iostream>

namespace Viewer
{

/*!
 * \brief Constructor
 * \param rView     the view
 * \param rControls the control panel
 * \param pSpecTcl  the spectcl interface
 * \param hName     the name of the histogram
 * \param parent    the parent widget
 *
 * Here we assemble the widgets and the
 */

GateManagerWidget::GateManagerWidget(SpectrumView &rView,
                                     ControlPanel &rControls,
                                     std::shared_ptr<SpecTclInterface> pSpecTcl,
                                     const QString &hName,
                                     QWidget *parent) :
    QWidget(parent),
    m_view(rView),
    m_controls(rControls),
    m_pSpecTcl(pSpecTcl),
    m_histDim(1),
    m_histName(hName)
{
    setUpGUI(pSpecTcl);

    connect(m_pManager, SIGNAL(addGateClicked()), this, SLOT(onAddPressed()));
    connect(m_pManager, SIGNAL(editGateClicked()), this, SLOT(onEditPressed()));
    connect(m_pManager, SIGNAL(deleteGateClicked()), this, SLOT(onDeletePressed()));

    connect(m_pSpecTcl.get(), SIGNAL(gateListChanged()), this, SLOT(updateGateList()));

}

/*!
 * \brief GateManagerWidget::setUpGUI
 *
 * \param pSpecTcl  the spectcl interface
 *
 *  Here we will only create the GateManager and add it to a layout.
 *  Once the user selects that they want to add or edit a widget, the appropriate
 *  widget will be created and used to replace the GateManager.
 */
void GateManagerWidget::setUpGUI(std::shared_ptr<SpecTclInterface> pSpecTcl)
{
    m_pManager = new GateManager(m_view, m_controls, pSpecTcl, m_histDim, this);
    horizontalLayout = new QHBoxLayout();
    horizontalLayout->setContentsMargins(0, 0, 0, 0);

    horizontalLayout->addWidget(m_pManager);
    setLayout(horizontalLayout);

}


void GateManagerWidget::setSpecTclInterface(std::shared_ptr<SpecTclInterface> pSpecTcl)
{
    m_pSpecTcl = pSpecTcl;
    m_pManager->setSpecTclInterface(m_pSpecTcl);
}

/*!
 * \brief Callback for adding a get...dispatches to addGate
 */
void GateManagerWidget::onAddPressed()
{

    auto canvasHist = setUpDialog();

    if (std::get<1>(canvasHist) == nullptr) return;

    addGate(*canvasHist.first, *canvasHist.second);

}

/*!
 * \brief Common code used to initiate add or edit dialogs
 * \returns tuple with content: (current canvas in view, histogram bundle associated with this dialog)
 */
std::pair<QRootCanvas*, HistogramBundle*> GateManagerWidget::setUpDialog()
{
    auto pHistList = m_pSpecTcl->getHistogramList();

    QMutexLocker lock(pHistList->getMutex());

    HistogramBundle* pHistPkg = pHistList->getHist(m_histName);

    if (! pHistPkg) {
        QMessageBox::warning(this, "Failure to add gate",
                             "Cannot add a gate because there is no spectrum "
                             "present to associate the gate with.");
    }

    return std::pair<QRootCanvas*,HistogramBundle*>(m_view.getCurrentCanvas(), pHistPkg);
}

/*!
 * \brief Swap out the GateManager for an editing dialog to create new gate
 *
 * \param rCanvas   canvas that contains the hist and gate
 * \param rHistPkg  histogram bundle the gate will be associate with
 *
 * The main thing that this needs to do is set up a OneDimGateEdit or
 * TwoDimGateEdit widget, depending dimensionality of the gate,
 * to use in place of the GateManager. Once the new widget is created,
 * it is used to replace the GateManager widget in the layout and its
 * signals are connected up to this class' slots.
 */
void GateManagerWidget::addGate(QRootCanvas& rCanvas, HistogramBundle& rHistPkg)
{
    // Turn of processing of updates in the view because if we allow them to be processed,
    // it triggers slots in this widget that really mess things up. We will be
    // sure to tell the view to stop ignoring updates at the end of editing. The issue
    // this addresses is primarily when the auto update feature is on.
    m_view.ignoreUpdates(true);

    // Stop gate polling too
    if (m_pSpecTcl) {
        m_pSpecTcl->enableGatePolling(false);
    }

    // determine whether this is a 1d or 2d hist and
    // open to appropriate dialog
    if (m_histDim == 2) {

        TwoDimGateEdit* pDialog = new TwoDimGateEdit(rCanvas, rHistPkg,
                                                     m_pSpecTcl,
                                                     nullptr,
                                                     this);

        connect(pDialog, SIGNAL(completed(GGate*)),
                this, SLOT(registerGate(GGate*)));

        connect(pDialog, SIGNAL(accepted()),
                this, SLOT(closeDialog()));

        horizontalLayout->takeAt(0);
        m_pManager->hide();
        horizontalLayout->insertWidget(0, pDialog);

    } else {
        OneDimGateEdit* pDialog = new OneDimGateEdit(rCanvas, rHistPkg,
                                                     m_pSpecTcl,
                                                     nullptr, this);

        connect(pDialog, SIGNAL(accepted()),
                this, SLOT(closeDialog()));

        connect(pDialog, SIGNAL(rejected()),
                this, SLOT(closeDialog()));

        horizontalLayout->takeAt(0);
        m_pManager->hide();
        horizontalLayout->insertWidget(0, pDialog);

    }

}

/*!
 * \brief Transition to a state where the edit widget is displayed
 *
 * There is very little different here from GateManagerWidget::addGate()
 * besides the fact that we also have to find the actual gate object
 * to edit and then pass it to the editing widget.
 */
void GateManagerWidget::onEditPressed()
{
    // we don't want the spectrum to update on us and destroy
    // our beautiful gates
    m_view.ignoreUpdates(true);

    auto canvasHist = setUpDialog();

    if (canvasHist.second == nullptr) {
        return;
    }

    QRootCanvas*     pCanvas  = canvasHist.first;
    HistogramBundle* pHistPkg = canvasHist.second;

    auto selection = m_pManager->getSelectedItems();
    if (selection.size()!=2) {
        QMessageBox::warning(0, "Invalid selection",
                             "User must select one gate to edit.");
        return;
    }

    QTableWidgetItem* pItem = selection.at(0);

    MasterGateList* pGateList = m_pSpecTcl->getGateList();

    // determine whether this is a 1d or 2d gate and
    // open to appropriate dialog
    if (m_histDim == 1) {

        auto itCut = pGateList->find1D(pItem->text());
        if (itCut == pGateList->end1d()) {
            QString msg("Unable to edit the requested gate, because \n");
            msg += "the gate was not found in the master gate list.";
            QMessageBox::warning(this, "Missing Gate", msg);
            return;
        }

        GSlice* pCut = itCut->get();

        auto pDialog = new OneDimGateEdit(*pCanvas,
                                          *pHistPkg, m_pSpecTcl,
                                          pCut, this);

        connect(pDialog, SIGNAL(accepted()), this, SLOT(closeDialog()));
        connect(pDialog, SIGNAL(rejected()), this, SLOT(closeDialog()));

        horizontalLayout->takeAt(0);
        m_pManager->hide();
        horizontalLayout->insertWidget(0, pDialog);

    } else {

        auto itCut = pGateList->find2D(pItem->text());
        if (itCut == pGateList->end2d()) {
            QString msg("Unable to edit the requested gate, because \n");
            msg += "the gate was not found in the master gate list.";
            QMessageBox::warning(this, "Missing Gate", msg);
            return;
        }

        GGate* pGate = itCut->get();
        // make sure that state is updated if user moved the cut via the gui
        pGate->synchronize(GGate::GUI);

        auto pDialog = new TwoDimGateEdit(*pCanvas, *pHistPkg, m_pSpecTcl,
                                          pGate);

        connect(pDialog, SIGNAL(accepted()), this, SLOT(closeDialog()));
        connect(pDialog, SIGNAL(rejected()), this, SLOT(closeDialog()));

        horizontalLayout->takeAt(0);
        m_pManager->hide();
        horizontalLayout->insertWidget(0, pDialog);
    }


}

/*! \brief No op ... gate deletion is handled by the GateManager. */
void GateManagerWidget::onDeletePressed()
{
//    std::cout << "Delete pressed" << std::endl;
}


/*!
 * \brief Callback for when editing of gates is completed
 *
 * The point of this is to remove the editing widget and replace it
 * with the GateManager widget again.
 *
 * The editing widget is deleted as well to avoid a memory leak.
 */
void GateManagerWidget::closeDialog()
{
    m_view.ignoreUpdates(false);

    QLayoutItem* pItem = horizontalLayout->takeAt(0);
    delete pItem->widget();

    horizontalLayout->insertWidget(0, m_pManager);
    m_pManager->show();

    // we need to update our canvases in case a new cut was created. Because
    // the SpecTcl interaction must be completed, the request is given a chance
    // to complete before the canvases are updated.
}


/*!
 * \brief Set up the GateManager gate list for 2d histogram/gates
 *
 * \param gateMap   mapping of gates with their names
 *
 * The gate manager does not take a map<QString, GGate*>. Rather, it takes
 * as an argument a vector of gate names. This method simply extracts
 * all of the keys in the map and sends them to the GateManager as a vector.
 */
void GateManagerWidget::setGateList(const std::map<QString, GGate*> &gateMap)
{
    std::vector<QString> gateNames;

    auto it = gateMap.begin();
    auto end = gateMap.end();

    while (it != end) {
        gateNames.push_back(it->first);
        ++it;
    }

    m_pManager->setGateList(gateNames);
}

/*!
 * \brief Set up the GateManager gate list for 1d histograms/gates
 *
 * \param gateMap   mapping gate names and gates
 *
 * This does the same as 2d version of setGateList().
 */
void GateManagerWidget::setGateList(const std::map<QString, GSlice*> &gateMap)
{
    std::vector<QString> gateNames;

    auto it = gateMap.begin();
    auto end = gateMap.end();

    while (it != end) {
        gateNames.push_back(it->first);
        ++it;
    }

    m_pManager->setGateList(gateNames);
}

/*!
 * \brief Conditionally recompute the gate integrals in the Gate Manager
 *
 * \param rHistPkg  the histogrma bundle in question
 *
 * This only does something if the manager is visible. what is the point of
 * doing computations if the gate manager is not even going to display
 * their results?
 */
void GateManagerWidget::updateGateIntegrals(HistogramBundle &rHistPkg)
{
    if (m_pManager->isVisible()) {
        m_pManager->updateGateIntegrals(rHistPkg);
    }
}


/*!
 * \brief Slot called to update the gate list
 *
 * This slot is called when the SpecTclInterface emits a gateListChanged()
 * signal. The histogram gates are resynchronize to ensure they are
 * up to date with whatever changed and then the new list of gates are
 * passed to the GateManager indirectly.
 */
void GateManagerWidget::updateGateList()
{
    if (! m_pManager->isVisible() || ! m_pSpecTcl) {
        return;
    }

    HistogramList* pList = m_pSpecTcl->getHistogramList();

    HistogramBundle* pHistBundle = pList->getHist(m_histName);
    if (pHistBundle) {
        pHistBundle->synchronizeGates(m_pSpecTcl->getGateList());
        if (m_histDim == 1) {
            setGateList(pHistBundle->getCut1Ds());
        } else {
            setGateList(pHistBundle->getCut2Ds());
        }
    }
}

} // end Viewer namespace
