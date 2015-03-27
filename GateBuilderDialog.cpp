#include "GateBuilderDialog.h"
#include "ui_GateBuilderDialog.h"
#include <QRootCanvas.h>
#include <QMessageBox>
#include <QPushButton>
#include <QRegExp>
#include <QRegExpValidator>

#include <TCanvas.h>
#include <TCutG.h>
#include <TList.h>
#include <TH1.h>
#include <iostream>
using namespace std;

GateBuilderDialog::GateBuilderDialog(QRootCanvas& rCanvas,
                                     HistogramBundle& histPkg,
                                     TCutG* pCut,
                                     QWidget *parent) :
    QDialog(parent),
    ui(new Ui::GateBuilderDialog),
    m_canvas(rCanvas),
    m_histPkg(histPkg),
    m_pCut(pCut),
    m_radioButtons(this),
    m_matchLast(false)
{
    ui->setupUi(this);
    ui->gateNameEdit->setPlaceholderText("Enter name of gate");

    QStringList headers;
    headers.push_back("X");
    headers.push_back("Y");

    // format the table
    ui->dataTable->setColumnCount(2);
    ui->dataTable->setHorizontalHeaderLabels(headers);

    ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(false);

    // Group the type buttons
    m_radioButtons.addButton(ui->bandButton, 0);
    m_radioButtons.addButton(ui->contourButton, 1);

    connect(&m_canvas, SIGNAL(PadClicked(TPad*)), this, SLOT(newPoint(TPad*)));
    connect(ui->gateNameEdit,SIGNAL(textChanged(QString)), this, SLOT(onNameChanged(QString)));

    connect(&m_radioButtons,SIGNAL(buttonClicked(int)),this, SLOT(onTypeChanged(int)));

    if (m_pCut) {
        setCutName(QString(m_pCut->GetName()));
        fillTableWithData(pCut);
    } else {
        m_pCut = new TCutG("__cut_in_progress__",0);
    }

    m_canvas.cd();
    m_pCut->Draw();

    cout << "Gatebuilderdialog" << endl;
}

GateBuilderDialog::~GateBuilderDialog()
{
    disconnect(&m_canvas, SIGNAL(PadClicked(TPad*)), this, SLOT(newPoint(TPad*)));

    delete ui;
}

void GateBuilderDialog::accept()
{
    if (ui->gateNameEdit->text() == "") {
        QMessageBox::warning(nullptr,"Missing gate name",
                             "User must specify a name for the gate");
        return;
    }

    // update the name of the cut
    m_pCut->SetName(ui->gateNameEdit->text().toAscii().constData());


    m_histPkg.addCut2D(m_pCut);

    // encode the request
    // encodeRequest(m_pCut);


    // this will(or should) transfer ownership to the gate manager
    emit completed(m_pCut);

    close();
}

void GateBuilderDialog::setCutName(const QString& name)
{
    ui->gateNameEdit->setText(name);
}

void GateBuilderDialog::newPoint(TPad *pad)
{
    cout << "Gatebuilderdialog::newPoint" << endl;
    Q_ASSERT(pad!=nullptr);

    double x, y;
    pad->AbsPixeltoXY(pad->GetEventX(), pad->GetEventY(), x, y);

    auto xEntry = new QTableWidgetItem(QString::number(x,'f',1));
    auto yEntry = new QTableWidgetItem(QString::number(y,'f',1));

    if (m_matchLast) {
        ensureLastPointDiffersFromFirst();
    }

    insertNewPoint(x, y);
    appendPointToCut(x,y);

    if (m_matchLast) {
        ensureLastPointMatchesFirst();
    }

}

void GateBuilderDialog::appendPointToCut(double x, double y)
{

    int nPoints =m_pCut->GetN();
    m_pCut->Set(nPoints+1);
    m_pCut->SetPoint(nPoints, x, y);

    m_canvas.Modified();
    m_canvas.Update();
}

void GateBuilderDialog::insertNewPoint(double x, double y)
{
    auto xEntry = new QTableWidgetItem(QString::number(x,'f',1));
    auto yEntry = new QTableWidgetItem(QString::number(y,'f',1));

    auto& table = *(ui->dataTable);
    table.setRowCount(table.rowCount()+1);
    table.setItem(table.rowCount()-1, 0, xEntry);
    table.setItem(table.rowCount()-1, 1, yEntry);

}

void GateBuilderDialog::fillTableWithData(TCutG *pCut)
{
    int n = pCut->GetN();
    double* pX = pCut->GetX();
    double* pY = pCut->GetY();

    for (int entry=0; entry<n ; ++entry) {
        insertNewPoint(pX[entry], pY[entry]);
    }
}

void GateBuilderDialog::onNameChanged(const QString &name)
{
    ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled( !name.isEmpty() );
}

void GateBuilderDialog::encodeRequest(const TCutG &cut) {

}

void GateBuilderDialog::onTypeChanged(int id)
{
    if (id==0) {
        // band
        ensureLastPointDiffersFromFirst();
        m_matchLast = false;
    } else {
        // contour
        ensureLastPointMatchesFirst();
        m_matchLast = true;
    }
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

        m_pCut->RemovePoint(nrows-1);

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

        insertNewPoint(firstX, firstY);
        appendPointToCut(firstX, firstY);

    }
}
