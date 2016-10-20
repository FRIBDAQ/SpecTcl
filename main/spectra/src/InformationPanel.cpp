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

#include "InformationPanel.h"
#include "ui_InformationPanel.h"

#include "HistogramBundle.h"
#include "GateManagerWidget.h"
#include "SpecTclInterface.h"
#include "SpectrumView.h"
#include "TabWorkspace.h"
#include "MultiSpectrumView.h"
#include "QRootCanvas.h"
#include "TCanvas.h"

#include <QTableWidget>
#include <QTableWidgetItem>

#include "TH1.h"

namespace Viewer
{

InformationPanel::InformationPanel(TabWorkspace &rView,
                                   std::shared_ptr<SpecTclInterface> pSpecTcl,
                                   const QString &histName,
                                   QWidget *parent) :
    QWidget(parent),
    ui(new Ui::InformationPanel),
    m_pView(&rView.getView())
{
    ui->setupUi(this);

    m_pGateManager = new GateManagerWidget(rView.getView(), rView.getControlPanel(),
                                           pSpecTcl, histName, this);

    ui->gridLayout->addWidget(m_pGateManager, 6, 0, 1, 0);
    setUpStatisticsTable();
}

InformationPanel::~InformationPanel()
{
    delete ui;
}


void InformationPanel::setSpecTclInterface(std::shared_ptr<SpecTclInterface> pSpecTcl)
{
    m_pGateManager->setSpecTclInterface(pSpecTcl);
}

void InformationPanel::onHistogramChanged(HistogramBundle &rHist)
{
    updateParameterList(rHist);
    updateStatistics(rHist);
    updateGates(rHist);

}


void InformationPanel::clearParameterList()
{
    QListWidgetItem* pItem = nullptr;
    while (( pItem = ui->pParamList->takeItem(0) )) {}
}

void InformationPanel::updateParameterList(HistogramBundle &rHist)
{
    clearParameterList();
    std::vector<std::string> params = rHist.getInfo().s_params;
    for (size_t i=0; i<params.size(); ++i) {
        ui->pParamList->insertItem(i, QString::fromStdString(params.at(i)));
    }
}

void InformationPanel::updateStatistics(HistogramBundle& rHist)
{
    QTableWidget* pTable = ui->pStatTable;

    TVirtualPad* pPad = m_pView->getCurrentCanvas()->getCanvas();
    TH1& bareHist = *(rHist.getClone(*pPad));

    TAxis* pX = bareHist.GetXaxis();
    TAxis* pY = bareHist.GetYaxis();

    int xbinLow = pX->GetFirst();
    int xbinHigh = pX->GetLast();

    int ybinLow = pY->GetFirst();
    int ybinHigh = pY->GetLast();

    QTableWidgetItem* pItem = nullptr;
    if (bareHist.InheritsFrom(TH2::Class())) {

        auto& hist2d = dynamic_cast<TH2&>(bareHist);

        Double_t integral = hist2d.Integral(1, pX->GetNbins(),
                                            1, pY->GetNbins());

        pItem = pTable->item(0, 1);
        pItem->setText(QString("%1").arg(integral));

        integral = hist2d.Integral(xbinLow, xbinHigh, ybinLow, ybinHigh);
        pItem = pTable->item(1, 1);
        pItem->setText(QString("%1").arg(integral));

    } else {
        Double_t integral = bareHist.Integral(1, bareHist.GetNbinsX());

        pItem = pTable->item(0, 1);
        pItem->setText(QString("%1").arg(integral));

        integral = bareHist.Integral(xbinLow, xbinHigh);
        pItem = pTable->item(1, 1);
        pItem->setText(QString("%1").arg(integral));
    }

    Double_t min = bareHist.GetMinimum();
    pItem = pTable->item(2, 1);
    pItem->setText(QString("%1").arg(min));

    Double_t max = bareHist.GetMaximum();
    pItem = pTable->item(3, 1);
    pItem->setText(QString("%1").arg(max));
}

void InformationPanel::setUpStatisticsTable() {
    QTableWidget* pTable = ui->pStatTable;

    pTable->setRowCount(4);
    pTable->setColumnCount(2);

    pTable->setHorizontalHeaderItem(0, new QTableWidgetItem("Statistic"));
    pTable->setHorizontalHeaderItem(1, new QTableWidgetItem("Value"));

    pTable->setItem(0, 0, new QTableWidgetItem("Total Integral"));
    pTable->setItem(1, 0, new QTableWidgetItem("View Integral"));
    pTable->setItem(2, 0, new QTableWidgetItem("Minimum"));
    pTable->setItem(3, 0, new QTableWidgetItem("Maximum"));

    pTable->setItem(0, 1, new QTableWidgetItem(""));
    pTable->setItem(1, 1, new QTableWidgetItem(""));
    pTable->setItem(2, 1, new QTableWidgetItem(""));
    pTable->setItem(3, 1, new QTableWidgetItem(""));


}

void InformationPanel::updateGates(HistogramBundle &rHist)
{
    if (rHist.getHist().InheritsFrom("TH2")) {
        m_pGateManager->setHistogramDimension(2);
        m_pGateManager->setGateList(rHist.getCut2Ds());
    } else {
        m_pGateManager->setHistogramDimension(1);
        m_pGateManager->setGateList(rHist.getCut1Ds());
    }

    m_pGateManager->updateGateIntegrals(rHist);
}

} // end Viewer namespace
