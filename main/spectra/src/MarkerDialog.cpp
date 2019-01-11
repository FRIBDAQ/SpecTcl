#include "MarkerDialog.h"
#include "ui_MarkerDialog.h"

MarkerDialog::MarkerDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::MarkerDialog)
{
    ui->setupUi(this);
}

MarkerDialog::~MarkerDialog()
{
    delete ui;
}

void MarkerDialog::on_pMarkerDialogButtonBox_accepted()
{
    QString markerName = ui->lineEdit->text();
    emit onAccepted(markerName, 0, 0);    // @todo get coordinates.
}

void MarkerDialog::on_pMarkerDialogButtonBox_rejected()
{
    
    emit onRejected();
}

void MarkerDialog::on_pMarkerDialogButtonBox_destroyed()
{
    emit onDeleted();
}
