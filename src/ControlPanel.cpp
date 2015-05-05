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

    // at the moment we will deal with these independently... ultimately the user should
    // accept their
    connect(pGeoSelector, SIGNAL(rowCountChanged(int)), this, SLOT(onRowCountChanged(int)));
    connect(pGeoSelector, SIGNAL(columnCountChanged(int)), this, SLOT(onColumnCountChanged(int)));

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

//void ControlPanel::onUpdateGeometry()
//{
//     // no op
//}

void ControlPanel::onRowCountChanged(int nRows)
{
  emit geometryChanged(nRows, pGeoSelector->getColumnCount());
}

void ControlPanel::onColumnCountChanged(int nColumns)
{
  emit geometryChanged(pGeoSelector->getRowCount(), nColumns);
}


} // end of namespace
