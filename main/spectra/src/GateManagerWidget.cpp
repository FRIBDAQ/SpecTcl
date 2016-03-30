#include "GateManagerWidget.h"
#include "GateManager.h"
#include "OneDimGateEdit.h"
#include "SpectrumView.h"
#include "SpecTclInterface.h"
#include "HistogramList.h"
#include "TH2.h"
#include "GateBuilderDialog.h"
#include "SliceTableItem.h"
#include "GateListItem.h"

#include <QHBoxLayout>
#include <QMessageBox>

#include <QMutexLocker>
#include <QMutex>

#include <iostream>

namespace Viewer
{

GateManagerWidget::GateManagerWidget(SpectrumView &rView,
                                     std::shared_ptr<SpecTclInterface> pSpecTcl,
                                     QWidget *parent) :
    QWidget(parent),
    m_view(rView),
    m_pSpecTcl(pSpecTcl)
{

    m_pManager = new GateManager(m_view, pSpecTcl, this);
    horizontalLayout = new QHBoxLayout();

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

    auto pCanvas = m_view.getCurrentCanvas();
    auto hists = SpectrumView::getAllHists(pCanvas);


    auto pHistList = m_pSpecTcl->getHistogramList();
    HistogramBundle* pHistPkg = nullptr;
    {
        QMutexLocker lock(pHistList->getMutex());

        // Ensure that there is a spectrum in the canvas
        if (hists.size() > 0) {
            pHistPkg = pHistList->getHist(hists.at(0));
        } else {
            QMessageBox::warning(this, "Failure to add gate",
                                 "Cannot add a gate because there is no spectrum "
                                 "present to associate the gate with.");
        }
    }

    return std::pair<QRootCanvas*,HistogramBundle*>(pCanvas, pHistPkg);
}

void GateManagerWidget::addGate(QRootCanvas& rCanvas, HistogramBundle& rHistPkg)
{

    if (m_pSpecTcl) {
        m_pSpecTcl->enableGatePolling(false);
    }

    bool isTH2 = false;
    {
      QMutexLocker lock(rHistPkg.getMutex());
      isTH2 = rHistPkg.getHist().InheritsFrom(TH2::Class());
    }
    // determine whether this is a 1d or 2d hist and
    // open to appropriate dialog
    if (isTH2) {

        GateBuilderDialog* dialog = new GateBuilderDialog(rCanvas, rHistPkg, nullptr);
        dialog->setAttribute(Qt::WA_DeleteOnClose);

        connect(dialog, SIGNAL(completed(GGate*)),
                this, SLOT(registerGate(GGate*)));

        dialog->show();
        dialog->raise();

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

    auto canvasHist = setUpDialog();

    if (canvasHist.second == nullptr) {
        return;
    }

    QRootCanvas*     pCanvas  = canvasHist.first;
    HistogramBundle* pHistPkg = canvasHist.second;

    auto selection = m_pManager->getSelectedItems();
    if (selection.size()!=1) {
        QMessageBox::warning(0, "Invalid selection", "User must select one gate to edit.");
        return;
    }

    auto pItem = selection.at(0);

    // determine whether this is a 1d or 2d gate and
    // open to appropriate dialog
    if (auto pSlItem = dynamic_cast<SliceTableItem*>(pItem)) {
        auto pCut = pSlItem->getSlice();
        auto pDialog = new OneDimGateEdit(*pCanvas,
                                          *pHistPkg, m_pSpecTcl,
                                          pCut, this);

        connect(pDialog, SIGNAL(accepted()), this, SLOT(closeDialog()));
        connect(pDialog, SIGNAL(rejected()), this, SLOT(closeDialog()));

        horizontalLayout->takeAt(0);
        m_pManager->hide();
        horizontalLayout->insertWidget(0, pDialog);

    } else {
        auto pGateItem = dynamic_cast<GateListItem*>(pItem);
        auto pGate = pGateItem->getGate();

        // make sure that state is updated if user moved the cut via the gui
        pGate->synchronize(GGate::GUI);

        GateBuilderDialog* dialog = new GateBuilderDialog(*pCanvas, *pHistPkg, pGate);
        dialog->setAttribute(Qt::WA_DeleteOnClose);
        connect(dialog, SIGNAL(completed(GGate*)),
                this, SLOT(editGate(GGate*)));

        dialog->show();
        dialog->raise();
    }


}

void GateManagerWidget::onDeletePressed()
{
    std::cout << "Delete pressed" << std::endl;
}

void GateManagerWidget::closeDialog()
{
    QLayoutItem* pItem = horizontalLayout->takeAt(0);
    delete pItem->widget();

    horizontalLayout->insertWidget(0, m_pManager);
    m_pManager->show();
}

} // end Viewer namespace
