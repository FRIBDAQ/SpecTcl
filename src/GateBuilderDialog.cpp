//    This software is Copyright by the Board of Trustees of Michigan
//    State University (c) Copyright 2015.
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

static const char* Copyright = "(C) Copyright Michigan State University 2015, All rights reserved";
#include "GateBuilderDialog.h"
#include "ui_GateBuilderDialog.h"
#include "QRootCanvas.h"
#include "GGate.h"

#include <QMessageBox>
#include <QPushButton>

#include <GateInfo.h>

#include <TList.h>
#include <TCanvas.h>
#include <TCutG.h>
#include <TList.h>
#include <TH1.h>

#include <iostream>
using namespace std;

namespace Viewer
{

GateBuilderDialog::GateBuilderDialog(QRootCanvas& rCanvas,
                                     HistogramBundle& histPkg,
                                     GGate* pCut,
                                     QWidget *parent) :
    QDialog(parent),
    ui(new Ui::GateBuilderDialog),
    m_canvas(rCanvas),
    m_histPkg(histPkg),
    m_pEditCut(new GGate(SpJs::Band())),
    m_pOldCut(pCut),
    m_radioButtons(this),
    m_matchLast(false),
    m_isMoveEvent(false),
    m_lastMousePressPos()
{
    ui->setupUi(this);
    ui->gateNameEdit->setPlaceholderText("Enter name of gate");

    QStringList headers;
    headers.push_back("X");
    headers.push_back("Y");

    // format the table
    ui->dataTable->setColumnCount(2);
    ui->dataTable->setHorizontalHeaderLabels(headers);

    // disable the ok button
    ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(false);

    // Group the type buttons
    m_radioButtons.addButton(ui->bandButton, 0);
    m_radioButtons.addButton(ui->contourButton, 1);

    if (m_pOldCut) {
      // set up the dialog to reflect state of an existing GGate
        hideOldCut(*m_pOldCut);

        *m_pEditCut = *m_pOldCut;

        onNameChanged(QString(m_pEditCut->getName()));
        // users cannot change a name once created
        ui->gateNameEdit->setDisabled(true);

        fillTableWithData(*m_pEditCut);

        // User's cannot change the type once created
        ui->bandButton->setDisabled(true);
        ui->contourButton->setDisabled(true);

    } else {
      // user is starting fresh, use the histogram bundle to access what
      // parameters are to be by the cut... we also default to beginning this
      // as a band
      auto xParam = m_histPkg.getInfo().s_params.at(0);
      auto yParam = m_histPkg.getInfo().s_params.at(1);

        m_pEditCut->setInfo(SpJs::Band("__gate_in_progress", xParam, yParam, {}));
    }

    m_canvas.cd();

    m_pEditCut->setEditable(true);

    m_pEditCut->getGraphicObject()->SetMarkerStyle(20);
    m_pEditCut->draw();

    m_canvas.Modified();
    m_canvas.Update();

    // Connect up the various components
    connect(&m_canvas, SIGNAL(PadMoveEvent(TPad*)), 
            this, SLOT(gateMoved(TPad*)));

    connect(&m_canvas, SIGNAL(mousePressed(TPad*)), 
            this, SLOT(onMousePress(TPad*)));

    connect(&m_canvas, SIGNAL(mouseReleased(TPad*)), 
            this, SLOT(onMouseRelease(TPad*)));

    connect(ui->gateNameEdit, SIGNAL(textChanged(const QString&)),
            this, SLOT(onNameChanged(const QString&)));

    connect(&m_radioButtons, SIGNAL(buttonClicked(int)), 
            this, SLOT(onTypeChanged(int)));

    connect(m_pEditCut->getGraphicObject(),
            SIGNAL(modified(std::vector<std::pair<double, double> >)),
            this,
            SLOT(onValuesChanged(std::vector<std::pair<double, double> >)));

    connect(ui->dataTable, SIGNAL(cellChanged(int,int)),
            this, SLOT(valueChanged(int, int)));
}


GateBuilderDialog::~GateBuilderDialog()
{
  delete ui;
}



// Decorate the accept behavior
void GateBuilderDialog::accept()
{
    if (ui->gateNameEdit->text() == "") {
        QMessageBox::warning(nullptr,"Missing gate name",
                             "User must specify a name for the gate");
        return;
    }

    if (m_pOldCut == nullptr) {
        m_pOldCut = new GGate(SpJs::Band("", "", "", {}));
    }

    // we have edited the MyCutG but not the SpJs::GateInfo2D object...
    // resync those.
    m_pEditCut->synchronize(GGate::GUI);

    *m_pOldCut = *m_pEditCut;
//    m_histPkg.addCut2D(m_pOldCut);

    // this will (or should) transfer ownership to the gate manager
    emit completed(m_pOldCut);

    // honor thy parents and let them have a say
    QDialog::accept();
}



void GateBuilderDialog::reject()
{
    // user did not want the changes... get rid of the new ones
    hideOldCut(*m_pEditCut);

    // redraw the old changes
    if ( m_pOldCut != nullptr) {
        m_pOldCut->draw();
    }

    QDialog::reject();
}

void GateBuilderDialog::setCutName(const QString& name)
{
    ui->gateNameEdit->setText(name);
}

void GateBuilderDialog::onMousePress(TPad *pad) 
{
  m_lastMousePressPos = make_pair(pad->GetEventX(), pad->GetEventY());
}

void GateBuilderDialog::onMouseRelease(TPad *pad) {
  int pXNew = pad->GetEventX(); 
  int pYNew = pad->GetEventY(); 

  int deltaX = pXNew - m_lastMousePressPos.first;
  int deltaY = pYNew - m_lastMousePressPos.second;

  cout << "dX=" << deltaX << "\tdY=" << deltaY << endl;

  if ( (deltaX > -2 || deltaX < 2) || (deltaY > -2 || deltaY < 2) ) {
    newPoint(pad);
  } else {
    // do nothing because the 2d cut will tell us its updated position
  }

}

/// Handle new click
void GateBuilderDialog::newPoint(TPad *pad)
{
  if ( m_isMoveEvent ) {
    m_isMoveEvent = false;
    return;
  } else {
    cout << "click event " << endl;
  }

  // disconnect cell changed signal because we are actually changing it
  disconnect(ui->dataTable, SIGNAL(cellChanged(int,int)),
          this, SLOT(valueChanged(int, int)));

    Q_ASSERT(pad!=nullptr);

    // convert between pixels and actual coordinates for cut
    double x, y;
    pad->AbsPixeltoXY(pad->GetEventX(), pad->GetEventY(), x, y);

    // create table entries
    auto xEntry = new QTableWidgetItem(QString::number(x,'f',1));
    auto yEntry = new QTableWidgetItem(QString::number(y,'f',1));

    // add points to  cut and handle differences between band and contour
    if (m_matchLast) {
        ensureLastPointDiffersFromFirst();
    }

    appendPointToTable(x, y);
    appendPointToCut(x,y);

    if (m_matchLast) {
        ensureLastPointMatchesFirst();
    }

    m_canvas.Modified();
    m_canvas.Update();

    // reconnect the cellChanged signal..
    connect(ui->dataTable, SIGNAL(cellChanged(int,int)),
            this, SLOT(valueChanged(int, int)));
}



void GateBuilderDialog::appendPointToCut(double x, double y)
{
    m_pEditCut->appendPoint(x, y);
}



void GateBuilderDialog::appendPointToTable(double x, double y)
{
    auto xEntry = new QTableWidgetItem(QString::number(x,'f',1));
    auto yEntry = new QTableWidgetItem(QString::number(y,'f',1));

    auto& table = *(ui->dataTable);
    table.setRowCount(table.rowCount()+1);
    table.setItem(table.rowCount()-1, 0, xEntry);
    table.setItem(table.rowCount()-1, 1, yEntry);

}



void GateBuilderDialog::clearTable()
{
  while ( ui->dataTable->rowCount() > 0 ) {
    ui->dataTable->removeRow(0);
  }
}



void GateBuilderDialog::fillTableWithData(GGate& rCut)
{
    auto points = rCut.getPoints();

    fillTableWithData( points );
}



void
GateBuilderDialog::fillTableWithData(const vector<pair<double, double> >& points)
{
  for (auto& point : points) {
      appendPointToTable(point.first, point.second);
  }
}



void GateBuilderDialog::onNameChanged(const QString &name)
{
    ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled( !name.isEmpty() );

    ui->gateNameEdit->setText(name);
    m_pEditCut->setName(name);
}



void GateBuilderDialog::onTypeChanged(int id)
{  
  // disconnect cell changed signal because we are actually changing it
  disconnect(ui->dataTable, SIGNAL(cellChanged(int,int)),
          this, SLOT(valueChanged(int, int)));

    SpJs::GateType type;
    if (id==0) {
        type = SpJs::BandGate;
    } else {
        type = SpJs::ContourGate;
    }

    m_pEditCut->setType(type);
    if (type == SpJs::BandGate) {
        ensureLastPointDiffersFromFirst();
        m_matchLast = false;
    } else {
        // contour
        ensureLastPointMatchesFirst();
        m_matchLast = true;
    }

    m_canvas.Modified();
    m_canvas.Update();

    // disconnect cell changed signal because we are actually changing it
    connect(ui->dataTable, SIGNAL(cellChanged(int,int)),
              this, SLOT(valueChanged(int, int)));
}



void GateBuilderDialog::ensureLastPointDiffersFromFirst()
{

    int nrows = ui->dataTable->rowCount();
    if (nrows == 0 || nrows == 1) {
        return;
    }

    auto firstX = ui->dataTable->item(0,0)->text().toDouble();
    auto firstY = ui->dataTable->item(0,1)->text().toDouble();

    auto lastX = ui->dataTable->item(nrows-1,0)->text().toDouble();
    auto lastY = ui->dataTable->item(nrows-1,1)->text().toDouble();

    if ((firstX == lastX) && (firstY == lastY)) {

        ui->dataTable->removeRow(nrows-1);

        m_pEditCut->popBackPoint();

        m_canvas.Modified();
        m_canvas.Update();
    }

}



void GateBuilderDialog::ensureLastPointMatchesFirst()
{
    int nrows = ui->dataTable->rowCount();
    if (nrows == 0 || nrows == 1) {
        return;
    }

    auto firstX = ui->dataTable->item(0,0)->text().toDouble();
    auto firstY = ui->dataTable->item(0,1)->text().toDouble();

    auto lastX = ui->dataTable->item(nrows-1,0)->text().toDouble();
    auto lastY = ui->dataTable->item(nrows-1,1)->text().toDouble();

    if ((firstX != lastX) || (firstY != lastY)) {

        appendPointToTable(firstX, firstY);
        appendPointToCut(firstX, firstY);

    }
}


void GateBuilderDialog::hideOldCut(GGate& gate)
{
    auto pList = m_canvas.getCanvas()->GetListOfPrimitives();
    pList->Remove(gate.getGraphicObject());
}



void GateBuilderDialog::gateMoved(TPad* pad) 
{
  m_isMoveEvent = true;
  cout << "Move event " << endl;
}



void GateBuilderDialog::onValuesChanged(vector<pair<double, double> > points)
{
  cout << "onValuesChanged" << endl;

  disconnect(ui->dataTable, SIGNAL(cellChanged(int,int)),
          this, SLOT(valueChanged(int, int)));

  clearTable();
  fillTableWithData(points);

  connect(ui->dataTable, SIGNAL(cellChanged(int,int)),
          this, SLOT(valueChanged(int, int)));

}


void GateBuilderDialog::valueChanged(int row, int col)
{
  cout << "changed (" << row << ", " << col << ")" << endl;
  auto pItemX = ui->dataTable->item(row, 0);
  auto pItemY = ui->dataTable->item(row, 1);

  double x = pItemX->text().toDouble();
  double y = pItemY->text().toDouble();

  m_pEditCut->setPoint(row, x, y);

  m_canvas.Modified();
  m_canvas.Update();
}

} // end of namespace
