#include "GeometrySelector.h"
#include "ui_GeometrySelector.h"


namespace Viewer
{

GeometrySelector::GeometrySelector(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::GeometrySelector)
{
    ui->setupUi(this);

    connect(ui->rowSpinBox, SIGNAL(valueChanged(int)), this, SLOT(onRowCountChanged(int)));
    connect(ui->colSpinBox, SIGNAL(valueChanged(int)), this, SLOT(onColumnCountChanged(int)));
}

GeometrySelector::~GeometrySelector()
{
    delete ui;
}

int GeometrySelector::getRowCount() const
{
  return ui->rowSpinBox->value();
}

int GeometrySelector::getColumnCount() const
{
  return ui->colSpinBox->value();
}

void GeometrySelector::onRowCountChanged(int nRows)
{
  emit rowCountChanged(nRows);
}

void GeometrySelector::onColumnCountChanged(int nColumns)
{
  emit columnCountChanged(nColumns);
}

} // end of namespace
