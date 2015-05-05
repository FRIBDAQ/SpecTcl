#include "ControlPanel.h"
#include "GeometrySelector.h"
#include "ui_ControlPanel.h"

namespace Viewer
{

ControlPanel::ControlPanel(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ControlPanel),
    pGeoSelector(new GeometrySelector(this))
{
    ui->setupUi(this);

    connect(ui->pUpdateSelected, SIGNAL(clicked()), this, SLOT(onUpdateSelected()));
    connect(ui->pUpdateAll, SIGNAL(clicked()), this, SLOT(onUpdateAll()));

    ui->horizontalLayout->insertWidget(0, pGeoSelector, 0, Qt::AlignLeft);

}

ControlPanel::~ControlPanel()
{
    delete ui;
}


void ControlPanel::onUpdateSelected()
{
    emit updateSelected();
}

void ControlPanel::onUpdateAll()
{
    emit updateAll();
}

void ControlPanel::onUpdateGeometry()
{
     // no op
}


}
 // end of namespace
