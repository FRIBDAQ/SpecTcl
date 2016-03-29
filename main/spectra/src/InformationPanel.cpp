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


void InformationPanel::onHistogramChanged(HistogramBundle &rHist)
{
}

} // end Viewer namespace
