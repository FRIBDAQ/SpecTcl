#include "InformationPanel.h"
#include "ui_InformationPanel.h"

#include "HistogramBundle.h"
#include "GateManagerWidget.h"
#include "SpecTclInterface.h"
#include "SpectrumView.h"

#include <QTableWidget>
#include <QTableWidgetItem>

#include "TH1.h"

namespace Viewer
{

InformationPanel::InformationPanel(SpectrumView &rView,
                                   std::shared_ptr<SpecTclInterface> pSpecTcl,
                                   QWidget *parent) :
    QWidget(parent),
    ui(new Ui::InformationPanel)
{
    ui->setupUi(this);

    m_pGateManager = new GateManagerWidget(rView, pSpecTcl, this);

    ui->gridLayout->addWidget(m_pGateManager, 6, 0, 1, 0);
    setUpStatisticsTable();
}

InformationPanel::~InformationPanel()
{
    delete ui;
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

    TH1& bareHist = rHist.getHist();
    Double_t integral = bareHist.Integral();

    QTableWidgetItem* pItem = pTable->item(0, 0);
    pItem->setText(QString("%1").arg(integral));


    Double_t min = bareHist.GetMinimum();
    pItem = pTable->item(1, 0);
    pItem->setText(QString("%1").arg(min));

    Double_t max = bareHist.GetMaximum();
    pItem = pTable->item(2, 0);
    pItem->setText(QString("%1").arg(max));
}

void InformationPanel::setUpStatisticsTable() {
    QTableWidget* pTable = ui->pStatTable;

    pTable->setRowCount(3);
    pTable->setColumnCount(2);

    pTable->setItem(0, 1, new QTableWidgetItem("Integral"));
    pTable->setItem(1, 1, new QTableWidgetItem("Minimum"));
    pTable->setItem(2, 1, new QTableWidgetItem("Maximum"));

    pTable->setItem(0, 0, new QTableWidgetItem(""));
    pTable->setItem(1, 0, new QTableWidgetItem(""));
    pTable->setItem(2, 0, new QTableWidgetItem(""));


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
}

} // end Viewer namespace
