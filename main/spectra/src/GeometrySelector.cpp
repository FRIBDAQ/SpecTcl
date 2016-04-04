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

#include "GeometrySelector.h"

namespace Viewer
{

//
//
GeometrySelector::GeometrySelector(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::GeometrySelector)
{
    ui->setupUi(this);

    connect(ui->rowSpinBox, SIGNAL(valueChanged(int)), this, SLOT(onRowCountChanged(int)));
    connect(ui->colSpinBox, SIGNAL(valueChanged(int)), this, SLOT(onColumnCountChanged(int)));
}

//
//
GeometrySelector::~GeometrySelector()
{
}

//
//
int GeometrySelector::getRowCount() const
{
  return ui->rowSpinBox->value();
}

//
//
int GeometrySelector::getColumnCount() const
{
  return ui->colSpinBox->value();
}

//
//
void GeometrySelector::onRowCountChanged(int nRows)
{
  emit rowCountChanged(nRows);
}

void GeometrySelector::setGeometry(int nRows, int nCols)
{
    disconnect(ui->rowSpinBox, SIGNAL(valueChanged(int)), this, SLOT(onRowCountChanged(int)));
    disconnect(ui->colSpinBox, SIGNAL(valueChanged(int)), this, SLOT(onColumnCountChanged(int)));

    ui->rowSpinBox->setValue(nRows);
    ui->colSpinBox->setValue(nCols);

    connect(ui->rowSpinBox, SIGNAL(valueChanged(int)), this, SLOT(onRowCountChanged(int)));
    connect(ui->colSpinBox, SIGNAL(valueChanged(int)), this, SLOT(onColumnCountChanged(int)));

}

//
//
void GeometrySelector::onColumnCountChanged(int nColumns)
{
  emit columnCountChanged(nColumns);
}

} // end of namespace
