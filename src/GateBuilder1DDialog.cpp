#include "GateBuilder1DDialog.h"
#include "ui_GateBuilder1DDialog.h"
#include "QRootCanvas.h"
#include "HistogramBundle.h"
#include "GSlice.h"
#include "QTLine.h"

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

GateBuilder1DDialog::GateBuilder1DDialog(QRootCanvas& canvas,
                                         HistogramBundle& hist,
                                         GSlice *pSlice,
                                         QWidget *parent) :
    QDialog(parent),
    ui(new Ui::GateBuilder1DDialog),
    m_canvas(canvas),
    m_histPkg(hist),
    m_editSlice(),
    m_pOldSlice(pSlice),
    m_editFocus(nullptr),
    m_editValidator(new QDoubleValidator(this))
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
        auto xMin = m_histPkg.hist()->GetXaxis()->GetXmin();
        auto xMax = m_histPkg.hist()->GetXaxis()->GetXmax();
        m_histPkg.unlock();

        QString paramName = QString::fromStdString( m_histPkg.getInfo().s_params.at(0) );

        m_editSlice = GSlice(0, "__cut_in_progress__", paramName, xMin, xMax, &m_canvas);
        onNameChanged("");
    }

    // make sure we can edit the slice
    m_editSlice.setEditable(true);
    m_editSlice.draw(&m_canvas);

    m_canvas.Modified();
    m_canvas.Update();

    // Connect
    connect(&m_canvas, SIGNAL(mousePressed(TPad*)),
            this, SLOT(onMousePress(TPad*)));

    connect(&m_canvas, SIGNAL(mouseReleased(TPad*)),
            this, SLOT(onMouseRelease(TPad*)));

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

}




GateBuilder1DDialog::~GateBuilder1DDialog()
{
    delete ui;
}



void GateBuilder1DDialog::accept()
{

    QString name = ui->gateNameEdit->text();
    if (name.isEmpty()) {
        QMessageBox::warning(0,"Invalid name", "User must enter name of gate");
        return;
    }

    // create persistent GSlice to "return"
    if (m_pOldSlice == nullptr) {
        m_pOldSlice = new GSlice;
    }

    // store new values
    *m_pOldSlice = m_editSlice;

    cout << *(m_pOldSlice->getXLowLine()) << endl;
    cout << *(m_pOldSlice->getXHighLine()) << endl;
    cout << *(m_editSlice.getXLowLine()) << endl;
    cout << *(m_editSlice.getXHighLine()) << endl;

    // send the slice to the outside world!
    emit completed( m_pOldSlice );

    // Call the parent's accept() so it accepts as it normally does.
    QDialog::accept();
}

void GateBuilder1DDialog::reject()
{
    // user did not want the changes... get rid of the new ones
    removeOldLines(m_editSlice);

    // redraw the old changes
    if ( m_pOldSlice != nullptr) {
        m_pOldSlice->draw(&m_canvas);
        m_canvas.Modified();
        m_canvas.Update();
    }

    // Call the parent's reject() so it accepts as it normally does.
    QDialog::reject();
}


void GateBuilder1DDialog::onMousePress(TPad *pad)
{
  m_lastMousePressPos = make_pair(pad->GetEventX(), pad->GetEventY());
}

void GateBuilder1DDialog::onMouseRelease(TPad *pad)
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
void GateBuilder1DDialog::onClick(TPad *pad)
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

void GateBuilder1DDialog::updateLow(double x)
{
    updateLowEdit(x);

    m_editSlice.setXLow(x);

    m_canvas.Modified();
    m_canvas.Update();
}

void GateBuilder1DDialog::updateHigh(double x)
{
    updateHighEdit(x);

    m_editSlice.setXHigh(x);

    m_canvas.Modified();
    m_canvas.Update();

}
void GateBuilder1DDialog::updateLowEdit(double x)
{
    ui->lowEdit->setText(QString::number(x,'f',1));

}


void GateBuilder1DDialog::updateHighEdit(double x)
{
    ui->highEdit->setText(QString::number(x,'f',1));
}

bool GateBuilder1DDialog::focusIsLow()
{
    return !(ui->highEdit->hasFocus());
}

void GateBuilder1DDialog::removeOldLines(GSlice &rSlice)
{
    auto pList = m_canvas.getCanvas()->GetListOfPrimitives();
    pList->Remove( const_cast<QTLine*>(rSlice.getXLowLine()) );
    pList->Remove( const_cast<QTLine*>(rSlice.getXHighLine()) );
}

void GateBuilder1DDialog::onNameChanged(QString name)
{

    m_editSlice.setName(name);
    ui->gateNameEdit->setText(name);

    bool empty = name.isEmpty();
    ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled( !empty );
}

void GateBuilder1DDialog::lowEditChanged()
{
    m_editSlice.setXLow(ui->lowEdit->text().toDouble());

    m_canvas.Modified();
    m_canvas.Update();
}

void GateBuilder1DDialog::highEditChanged()
{
    m_editSlice.setXHigh(ui->highEdit->text().toDouble());

    m_canvas.Modified();
    m_canvas.Update();
}

void GateBuilder1DDialog::onLowChanged(double x1, double y1,
                                          double x2, double y2)
{
  updateLow(x1);
}

void GateBuilder1DDialog::onHighChanged(double x1, double y1,
                                        double x2, double y2)
{
  updateHigh(x1);
}

} // end of namespace
