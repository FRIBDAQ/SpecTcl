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
        
        // If the histogram has more parameters they must all be put in
        // (that'll make this a gamma gate):
        
        int np = m_histPkg.getInfo().s_params.size();
        if (np > 1) {
            for (int i =1; i < np; i++) {
                m_editSlice.addParameter(QString::fromStdString(m_histPkg.getInfo().s_params[i]));
            }
        }
        
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

//    cout << *m_pOldSlice << endl;

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

  // In most cases, all we need to do is enter the right slice type.
  // for 
    pSlice->setEditable(false);

    // That for the 2dm projection spectra, we need to make
    // a bunch of slices, one for each of the x parameters and
    // an OR gate tying them together.
    
    if (m_pSpecTcl) {
        SpJs::HistInfo hInfo = m_histPkg.getInfo();
        
        if (hInfo.s_type != "2dmproj") {
            m_pSpecTcl->addGate(*pSlice);
        } else {
            createProjectionGate(hInfo, pSlice);
        }
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
        SpJs::HistInfo hInfo = m_histPkg.getInfo();
        if (hInfo.s_type != "2dmproj") {
            m_pSpecTcl->editGate(*pSlice);
        } else {
            editProjectionGate(hInfo, pSlice);
        }
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
/*--------------------------------------------------------------------------
 *  Special gate handling for projection gates.  These methods assume
 *  that the SpecTcl interface has been created and so on.
 */

 /**
  * duplicateSlice
  *    Create a copy of a slice with different parameters and names.
  *    The low/high values of the slice, the parent.  The canvas is our
  *    canvas and there's no parent given.
  *
  * @param newName - name of the object slice
  * @param newParam -name of the object slice's xparameter
  * @param source   - Slice that's getting copied.
  * @return GSlice* - Dynamically created slice...must be deleted by caller.
  */
 GSlice*
 OneDimGateEdit::duplicateSlice(
    const std::string& newName, const std::string& newParam,
    const GSlice& source
 )
 {
    QString name(newName.c_str());
    QString param(newParam.c_str());
    
    return  new GSlice(
        nullptr, name, param, source.getXLow(), source.getXHigh(), &m_canvas
    );
    
 }
 /**
  * createProjectionComponents
  *    projection spectrum slices get turned into a number of slices
  *    and an Or gate of all of those slices.  This method produces
  *    the component gates that are ore-d together to create the final gate.
  *
  * @param hInfo - for convenience, this is the histogram information
  *                extracted from our histogram bundle.
  * @param pSource - pointer to the slice that was accepted on the
  *                display.  This is used to supply the base names for the components
  *                and the low/high cut values.
  * @return std::vector<std::pair<std::string, GSlice> -
  *         Vector of the resulting gate names and slices
  */
 std::vector<std::pair<std::string, GSlice*> >
 OneDimGateEdit::createProjectionComponents(
        SpJs::HistInfo& hInfo, GSlice* pSource
)
{
    std::vector<std::pair<std::string, GSlice*> > result;
    std::string baseName = pSource->getName().toStdString();
    
    int start = hInfo.s_direction ? 0 : 1;   // X or Y parameter projection.
    
    for (int  i =start; i < hInfo.s_params.size(); i += 2) {
        std::string paramName = hInfo.s_params[i];
        std::string gateName(baseName);
        gateName += ".";
        gateName += paramName;
        
        GSlice* s = duplicateSlice(gateName, paramName, *pSource);
        result.push_back(std::make_pair(gateName, s));
    }
    return result;
}   
/**
 * createProjectionGate
 *    Creates the component gates, the OR gate and enters it into SpecTcl.
 *    m_pSpecTcl must be non null to call this method.
 *
 *  @param hInfo   - Histogram bundle on which the gate was drawn.
 *  @param pSource - source slice for the component gates.
 */
void
OneDimGateEdit::createProjectionGate(SpJs::HistInfo& hInfo, GSlice* pSource)
{
    std::vector<std::pair<std::string, GSlice*> > components =
        createProjectionComponents(hInfo, pSource);
    
    // First create the component gates...
    
    std::vector<std::string> componentNames;;
    for (int i =0; i < components.size(); i++) {
        m_pSpecTcl->addGate(*components[i].second);
        delete components[i].second;
        componentNames.push_back(components[i].first);
    }
    // Then create the OR gate:
    
    std::string gateName =  pSource->getName().toStdString();
    
    m_pSpecTcl->addOrGate(gateName, componentNames);
}

/**
 * editProjectionGate
 *    This is the same as createProjection gate, but existing gates are presumed
 *    to exist and are edited.  An astute reader of code will note that at present,
 *    the REST interface does not differentiate between editing and creation.
 *    An even more astute reader will note that if it ever did, this method
 *    would become a bit more complicated as some of the components might need
 *    creation, while others might only need to be edited -- for now we'll
 *    ignore that potential complexity, since a the level of SpecTcl gate
 *    creation and modification are not differentiated either.
 *
 *  @param hInfo   - Histogram bundle on which the gate was drawn.
 *  @param pSource - The gate that was drawn.
 */
void
OneDimGateEdit::editProjectionGate(SpJs::HistInfo& hInfo, GSlice* pSource)
{
    std::vector<std::pair<std::string, GSlice*> > components =
        createProjectionComponents(hInfo, pSource);
    
    // First create the component gates...
    
    std::vector<std::string> componentNames;;
    for (int i =0; i < components.size(); i++) {
        m_pSpecTcl->editGate(*components[i].second);
        delete components[i].second;
        componentNames.push_back(components[i].first);
    }
    // Then create the OR gate:
    
    std::string gateName =  pSource->getName().toStdString();
    
    m_pSpecTcl->editOrGate(gateName, componentNames);    
}

} // end of namespace
