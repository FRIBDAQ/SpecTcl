#include "ControlPanel.h"
#include "ui_ControlPanel.h"

namespace Viewer
{

ControlPanel::ControlPanel(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ControlPanel)
{
    ui->setupUi(this);

    connect(ui->pUpdateSelected, SIGNAL(clicked()), this, SLOT(onUpdateSelected()));
    connect(ui->pUpdateAll, SIGNAL(clicked()), this, SLOT(onUpdateAll()));
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
