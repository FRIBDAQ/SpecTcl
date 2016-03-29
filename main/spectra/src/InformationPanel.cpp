#include "InformationPanel.h"
#include "ui_InformationPanel.h"

#include "HistogramBundle.h"

namespace Viewer
{

InformationPanel::InformationPanel(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::InformationPanel)
{
    ui->setupUi(this);
}

InformationPanel::~InformationPanel()
{
    delete ui;
}


void InformationPanel::clearParameterList()
{
    QListWidgetItem* pItem = nullptr;
    while (( pItem = ui->pParamList->takeItem(0) )) {}
}

void InformationPanel::onHistogramChanged(HistogramBundle &rHist)
{
    clearParameterList();
    std::vector<std::string> params = rHist.getInfo().s_params;
    for (size_t i=0; i<params.size(); ++i) {
        ui->pParamList->insertItem(i, QString::fromStdString(params.at(i)));
    }

}



} // end Viewer namespace
