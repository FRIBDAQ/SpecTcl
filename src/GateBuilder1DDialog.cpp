#include "GateBuilder1DDialog.h"
#include "ui_GateBuilder1DDialog.h"
#include "QRootCanvas.h"
#include "HistogramBundle.h"
#include "GSlice.h"
#include <QMessageBox>
#include <TPad.h>
#include <TCanvas.h>
#include <TFrame.h>
#include <TH1.h>
#include <TList.h>
#include <TLine.h>

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
    m_editFocus(nullptr)
{
    ui->setupUi(this);

    ui->gateNameEdit->setPlaceholderText("Enter name of slice");

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

//        ui->gateNameEdit->setText(m_editSlice.getName());
        nameChanged(m_editSlice.getName());

    } else {
        m_histPkg.lock();
        auto xMin = m_histPkg.hist()->GetXaxis()->GetXmin();
        auto xMax = m_histPkg.hist()->GetXaxis()->GetXmax();
        m_histPkg.unlock();

        QString paramName = QString::fromStdString( m_histPkg.getInfo().s_params.at(0) );

        m_editSlice = GSlice(0, "__cut_in_progress__", paramName, xMin, xMax, &m_canvas);
    }
    m_editSlice.draw(&m_canvas);

    // Connect
    connect(&m_canvas, SIGNAL(PadClicked(TPad*)), this, SLOT(onClick(TPad*)));
    connect(ui->gateNameEdit, SIGNAL(textChanged(QString)),
            &m_editSlice, SLOT(nameChanged(QString)));

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

    m_histPkg.addCut1D(m_pOldSlice);

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
    }

    // Call the parent's reject() so it accepts as it normally does.
    QDialog::reject();
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
    pList->Remove(rSlice.getXLowLine());
    pList->Remove(rSlice.getXHighLine());
}
