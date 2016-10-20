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

/*!
 * \brief InformationPanel::InformationPanel
 * \param rView     tab workspace this associated with
 * \param pSpecTcl  interface to spectcl
 * \param histName  name of histogram this is associated with
 * \param parent    parent widget
 *
 *  Assembles the widget. The gate manager widget is not created by the
 *  InformationPanel.ui file so it is created here and added to the widget.
 *
 */
InformationPanel::InformationPanel(TabWorkspace &rWorkspace,
                                   std::shared_ptr<SpecTclInterface> pSpecTcl,
                                   const QString &histName,
                                   QWidget *parent) :
    QWidget(parent),
    ui(new Ui::InformationPanel),
    m_pView(&rWorkspace.getView())
{
    // assembles the widget given the .ui file
    ui->setupUi(this);

    // create and add the GateManagerWidget
    m_pGateManager = new GateManagerWidget(rWorkspace.getView(), rWorkspace.getControlPanel(),
                                           pSpecTcl, histName, this);

    ui->gridLayout->addWidget(m_pGateManager, 6, 0, 1, 0);
    setUpStatisticsTable();
}

InformationPanel::~InformationPanel()
{
    delete ui;
}

/*!
 * \brief Update cached spectcl interface in case users switch their interface
 * \param pSpecTcl  the new interface
 */
void InformationPanel::setSpecTclInterface(std::shared_ptr<SpecTclInterface> pSpecTcl)
{
    m_pGateManager->setSpecTclInterface(pSpecTcl);
}

/*!
 * \brief Callback to call logic for updating panel when hist changes
 *
 * \param rHist the new histogram
 *
 */
void InformationPanel::onHistogramChanged(HistogramBundle &rHist)
{
    updateParameterList(rHist);
    updateStatistics(rHist);
    updateGates(rHist);
}

/*!
 * \brief Remove all entries from the parameter list
 */
void InformationPanel::clearParameterList()
{
    QListWidgetItem* pItem = nullptr;
    while (( pItem = ui->pParamList->takeItem(0) )) {}
}

/*!
 * \brief Replaces parameter list for the new histogram
 * \param rHist     the new histogram
 *
 * We simply clear out all parameters in the list and the repopulate the list
 * from scratch. This is fine because the user cannot select the elements in the
 * list.
 */
void InformationPanel::updateParameterList(HistogramBundle &rHist)
{
    clearParameterList();
    std::vector<std::string> params = rHist.getInfo().s_params;
    for (size_t i=0; i<params.size(); ++i) {
        ui->pParamList->insertItem(i, QString::fromStdString(params.at(i)));
    }
}

/*!
 * \brief InformationPanel::updateStatistics
 * \param rHist the HistogramBundle
 *
 * We compute all of the statistics associated with the histogram in this
 * method. The maximum, minimum, total integral, and view integral are all
 * computed. The widget is updated to display the new numbers.
 */
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

/*!
 * \brief InformationPanel::setUpStatisticsTable
 *
 * This is called by the constructor to add the tablewidgetitems
 * into the statistics table widget for later editing.
 */
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

/*!
 * \brief InformationPanel::updateGates
 * \param rHist the new histogram
 *
 * Alert the gate manager widget to the updated histogram so that
 * it also can update its displayed data.
 */
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
