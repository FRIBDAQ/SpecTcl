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

#include "OneDimGateEdit.h"
#include "ui_OneDimGateEdit.h"
#include "QRootCanvas.h"
#include "HistogramBundle.h"
#include "GSlice.h"
#include "QTLine.h"
#include "SpecTclInterface.h"

#include <QMessageBox>
#include <QPushButton>
#include <QDoubleValidator>

#include <TPad.h>
#include <TCanvas.h>
#include <TFrame.h>
#include <TH1.h>
#include <TList.h>

using namespace std;

namespace Viewer
{

OneDimGateEdit::OneDimGateEdit(QRootCanvas& canvas,
                               HistogramBundle& hist,
                               std::shared_ptr<SpecTclInterface> pSpecTcl,
                               GSlice *pSlice,
                               QWidget *parent) :
    QWidget(parent),
    ui(new Ui::OneDimGateEdit),
    m_canvas(canvas),
    m_histPkg(hist),
    m_editSlice(),
    m_pOldSlice(pSlice),
    m_editFocus(nullptr),
    m_editValidator(new QDoubleValidator(this)),
    m_pSpecTcl(pSpecTcl)
{
    ui->setupUi(this);

    ui->gateNameEdit->setPlaceholderText("Enter name of slice");

    // only let the user enter doubles into the line edit
    ui->lowEdit->setValidator(m_editValidator);
    ui->highEdit->setValidator(m_editValidator);

    // Set the low entry to focus
    m_editFocus = ui->lowEdit;

    // Handle if the user is editing an existing slice
    if (m_pOldSlice) {

        // We want the user to see the lines we are editing
        removeOldLines(*m_pOldSlice);

        // Copy the state of the old slice to the editable slice
        m_editSlice = *m_pOldSlice;

        // Update
        double xLow = m_editSlice.getXLowLine()->GetX1();
        double xHigh = m_editSlice.getXHighLine()->GetX1();

        updateLow(xLow);
        updateHigh(xHigh);

        onNameChanged(m_editSlice.getName());

        // at the moment users cant change the name of a gate once created
        ui->gateNameEdit->setDisabled(true);
    } else {
        m_histPkg.lock();
        auto xMin = m_histPkg.getHist().GetXaxis()->GetXmin();
        auto xMax = m_histPkg.getHist().GetXaxis()->GetXmax();
        m_histPkg.unlock();

        double xWidth = xMax - xMin;

        QString paramName = QString::fromStdString( m_histPkg.getInfo().s_params.at(0) );

        m_editSlice = GSlice(0, "__cut_in_progress__", paramName,
                             xMin + 0.1*xWidth,
                             xMax - 0.1*xWidth, &m_canvas);
        onNameChanged("");
    }

    // make sure we can edit the slice
    m_editSlice.setEditable(true);
    m_editSlice.draw(&m_canvas);

    m_canvas.Modified();
    m_canvas.Update();

    connectSignals();
}




OneDimGateEdit::~OneDimGateEdit()
{
    delete ui;
}



void OneDimGateEdit::accept()
{

    QString name = ui->gateNameEdit->text();
    if (name.isEmpty()) {
        QMessageBox::warning(0,"Invalid name", "User must enter name of gate");
        return;
    }

    bool isNewSlice = false;
    // create persistent GSlice to "return"
    if (m_pOldSlice == nullptr) {
        m_pOldSlice = new GSlice;
        isNewSlice = true;
    }

    // store new values
    *m_pOldSlice = m_editSlice;

    cout << *m_pOldSlice << endl;

    // get rid of the editable version of the slice
    removeOldLines(m_editSlice);

//    // send the slice to the outside world!
//    emit completed( m_pOldSlice );

    if (isNewSlice) {
        registerSlice(m_pOldSlice);
        delete m_pOldSlice;
    } else {
        editSlice(m_pOldSlice);
    }

    emit accepted();
}

void OneDimGateEdit::reject()
{
    // user did not want the changes... get rid of the new ones
    removeOldLines(m_editSlice);

    // redraw the old changes
    if ( m_pOldSlice != nullptr) {
        m_pOldSlice->draw(&m_canvas);
    }
    m_canvas.Modified();
    m_canvas.Update();

    emit rejected();
}

/*!
 * \brief OneDimGateEdit::registerSlice
 *
 * The slice is registered to SPecTcl. It is not drawn because the next
 * update should cause it to be drawn.
 *
 * \param pSlice    the slice to register
 */
void OneDimGateEdit::registerSlice(GSlice *pSlice)
{
  Q_ASSERT( pSlice != nullptr );

    pSlice->setEditable(false);

    if (m_pSpecTcl) {
        m_pSpecTcl->addGate(*pSlice);
        m_pSpecTcl->enableGatePolling(true);
    }

}

/*!
 * \brief OneDimGateEdit::editSlice
 *
 *  The new version of the slice is sent to SpecTcl to replace the old
 * version of it. The original graphical object is redrawn.
 *
 * \param pSlice
 */
void OneDimGateEdit::editSlice(GSlice *pSlice)
{
    Q_ASSERT(pSlice != nullptr);

    // Update the
    if (m_pSpecTcl) {
        m_pSpecTcl->editGate(*pSlice);
        m_pSpecTcl->enableGatePolling(true);
    }

    pSlice->draw();
    pSlice->setEditable(false);

}

void OneDimGateEdit::onMousePress(QRootCanvas *pad)
{
  m_lastMousePressPos = make_pair(pad->GetEventX(), pad->GetEventY());
}

void OneDimGateEdit::onMouseRelease(QRootCanvas *pad)
{
  int newXPos = pad->GetEventX();
  int newYPos = pad->GetEventY();

  int deltaX = newXPos - m_lastMousePressPos.first;
  int deltaY = newYPos - m_lastMousePressPos.second;

  // only treat this as a click if the user did not move the mouse with the button
  // held down
  if ( ( deltaX > -2 && deltaX < 2 ) && ( deltaY > -2 && deltaY < 2 ) ) {
      onClick(pad);
  }
}


// Handle user's click
void OneDimGateEdit::onClick(QRootCanvas *pad)
{
    int px = pad->GetEventX();

    // convert pixel position to the appropriate gate value
    double x = pad->AbsPixeltoX(px);

    if (m_editFocus == ui->lowEdit) {
        updateLow(x);

        // make sure that the next time we click it ends up in the next
        m_editFocus = ui->highEdit;
    } else {
        updateHigh(x);

        m_editFocus = ui->lowEdit;
    }

}

void OneDimGateEdit::updateLow(double x)
{
    updateLowEdit(x);

    m_editSlice.setXLow(x);

    m_canvas.Modified();
    m_canvas.Update();
}

void OneDimGateEdit::updateHigh(double x)
{
    updateHighEdit(x);

    m_editSlice.setXHigh(x);

    m_canvas.Modified();
    m_canvas.Update();

}
void OneDimGateEdit::updateLowEdit(double x)
{
    ui->lowEdit->setText(QString::number(x,'f',1));

}


void OneDimGateEdit::updateHighEdit(double x)
{
    ui->highEdit->setText(QString::number(x,'f',1));
}

bool OneDimGateEdit::focusIsLow()
{
    return !(ui->highEdit->hasFocus());
}

void OneDimGateEdit::removeOldLines(GSlice &rSlice)
{
    auto pList = m_canvas.getCanvas()->GetListOfPrimitives();
    pList->Remove( const_cast<QTLine*>(rSlice.getXLowLine()) );
    pList->Remove( const_cast<QTLine*>(rSlice.getXHighLine()) );
    m_canvas.Modified();
    m_canvas.Update();
}

void OneDimGateEdit::onNameChanged(QString name)
{

    m_editSlice.setName(name);
    ui->gateNameEdit->setText(name);

    bool empty = name.isEmpty();
    ui->pApplyButton->setEnabled( !empty );
}

void OneDimGateEdit::lowEditChanged()
{
    m_editSlice.setXLow(ui->lowEdit->text().toDouble());

    m_canvas.Modified();
    m_canvas.Update();
}

void OneDimGateEdit::highEditChanged()
{
    m_editSlice.setXHigh(ui->highEdit->text().toDouble());

    m_canvas.Modified();
    m_canvas.Update();
}

void OneDimGateEdit::onLowChanged(double x1, double y1,
                                          double x2, double y2)
{
  updateLow(x1);
}

void OneDimGateEdit::onHighChanged(double x1, double y1,
                                        double x2, double y2)
{
  updateHigh(x1);
}

void OneDimGateEdit::connectSignals()
{

  // Connect
  connect(&m_canvas, SIGNAL(mousePressed(QRootCanvas*)),
          this, SLOT(onMousePress(QRootCanvas*)));

  connect(&m_canvas, SIGNAL(mouseReleased(QRootCanvas*)),
          this, SLOT(onMouseRelease(QRootCanvas*)));

  connect(ui->gateNameEdit, SIGNAL(textChanged(QString)),
          this, SLOT(onNameChanged(QString)));

  connect(ui->lowEdit, SIGNAL(editingFinished()),
          this, SLOT(lowEditChanged()));

  connect(ui->highEdit, SIGNAL(editingFinished()),
          this, SLOT(highEditChanged()));

  connect(m_editSlice.getXLowLine(), SIGNAL(valuesChanged(double,double,double,double)),
          this, SLOT(onLowChanged(double,double,double,double)));

  connect(m_editSlice.getXHighLine(), SIGNAL(valuesChanged(double,double,double,double)),
          this, SLOT(onHighChanged(double,double,double,double)));

  connect(ui->pApplyButton, SIGNAL(clicked()), this, SLOT(accept()));
  connect(ui->pCancelButton, SIGNAL(clicked()), this, SLOT(reject()));
}

} // end of namespace
