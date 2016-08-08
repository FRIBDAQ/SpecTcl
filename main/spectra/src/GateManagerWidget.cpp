#include "GateManagerWidget.h"
#include "GateManager.h"
#include "OneDimGateEdit.h"
#include "TwoDimGateEdit.h"
#include "SpectrumView.h"
#include "SpecTclInterface.h"
#include "HistogramList.h"
#include "SliceTableItem.h"
#include "GateListItem.h"
#include "GGate.h"
#include "GSlice.h"

#include "TH2.h"

#include <QHBoxLayout>
#include <QMessageBox>

#include <QMutexLocker>
#include <QMutex>
#include <QTableWidgetItem>

#include <iostream>

namespace Viewer
{

GateManagerWidget::GateManagerWidget(SpectrumView &rView,
                                     std::shared_ptr<SpecTclInterface> pSpecTcl,
                                     const QString &hName,
                                     QWidget *parent) :
    QWidget(parent),
    m_view(rView),
    m_pSpecTcl(pSpecTcl),
    m_histDim(1),
    m_histName(hName)
{
    m_pManager = new GateManager(m_view, pSpecTcl, this);
    horizontalLayout = new QHBoxLayout();
    horizontalLayout->setContentsMargins(0, 0, 0, 0);

    horizontalLayout->addWidget(m_pManager);
    setLayout(horizontalLayout);

    show();

    connect(m_pManager, SIGNAL(addGateClicked()), this, SLOT(onAddPressed()));
    connect(m_pManager, SIGNAL(editGateClicked()), this, SLOT(onEditPressed()));
    connect(m_pManager, SIGNAL(deleteGateClicked()), this, SLOT(onDeletePressed()));

}


void GateManagerWidget::onAddPressed()
{

    auto canvasHist = setUpDialog();

    if (std::get<1>(canvasHist) == nullptr) return;

    addGate(*canvasHist.first, *canvasHist.second);

}

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

void GateManagerWidget::addGate(QRootCanvas& rCanvas, HistogramBundle& rHistPkg)
{
    m_view.ignoreUpdates(true);

    if (m_pSpecTcl) {
        m_pSpecTcl->enableGatePolling(false);
    }

    // determine whether this is a 1d or 2d hist and
    // open to appropriate dialog
    if (m_histDim == 2) {

        TwoDimGateEdit* pDialog = new TwoDimGateEdit(rCanvas, rHistPkg,
                                                     m_pSpecTcl,
                                                     nullptr);

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

void GateManagerWidget::onDeletePressed()
{
    std::cout << "Delete pressed" << std::endl;
}

void GateManagerWidget::closeDialog()
{
    m_view.ignoreUpdates(false);

    QLayoutItem* pItem = horizontalLayout->takeAt(0);
    delete pItem->widget();

    horizontalLayout->insertWidget(0, m_pManager);
    m_pManager->show();
}


// 2d version
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

// 1d version
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

void GateManagerWidget::updateGateIntegrals(HistogramBundle &rHistPkg)
{
    m_pManager->updateGateIntegrals(rHistPkg);
}


} // end Viewer namespace
