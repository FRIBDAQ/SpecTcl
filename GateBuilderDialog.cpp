#include "GateBuilderDialog.h"
#include "ui_GateBuilderDialog.h"
#include <QRootCanvas.h>
#include <QMessageBox>

#include <TPad.h>
#include <TCutG.h>
#include <iostream>
using namespace std;

GateBuilderDialog::GateBuilderDialog(QRootCanvas& rCanvas, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::GateBuilderDialog),
    m_canvas(rCanvas),
    m_pCut(new TCutG("__cut_in_progress__",0))
{
    ui->setupUi(this);
    QStringList headers;
    headers.push_back("X");
    headers.push_back("Y");

    ui->dataTable->setColumnCount(2);
    ui->dataTable->setHorizontalHeaderLabels(headers);

    connect(&m_canvas, SIGNAL(PadClicked(TPad*)), this, SLOT(newPoint(TPad*)));

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

    // encode the request
   // encodeRequest(m_pCut);

    // relinquish ownership...
    auto pCut = m_pCut.release();

    // this will(or should) transfer ownership to the gate manager
    emit completed(pCut);

    close();
}

void GateBuilderDialog::newPoint(TPad *pad)
{
    cout << "Gatebuilderdialog::newPoint" << endl;
    Q_ASSERT(pad!=nullptr);

    double x, y;
    pad->AbsPixeltoXY(pad->GetEventX(), pad->GetEventY(), x, y);

    auto xEntry = new QTableWidgetItem(QString::number(x,'f',1));
    auto yEntry = new QTableWidgetItem(QString::number(y,'f',1));

    auto& table = *(ui->dataTable);
    table.setRowCount(table.rowCount()+1);
    table.setItem(table.rowCount()-1, 0, xEntry);
    table.setItem(table.rowCount()-1, 1, yEntry);

    int nPoints =m_pCut->GetN();
    m_pCut->Set(nPoints+1);
    m_pCut->SetPoint(nPoints, x, y);

    m_canvas.Modified();
    m_canvas.Update();
}

void GateBuilderDialog::nameChanged(const QString &name)
{
    m_name = name;
}

void GateBuilderDialog::encodeRequest(const TCutG &cut) {

}
