#include "LayoutDialogChooser.h"
#include "ui_LayoutDialogChooser.h"

namespace Viewer {

LayoutDialogChooser::LayoutDialogChooser(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::LayoutDialogChooser)
{
    ui->setupUi(this);

    ui->pTabNameEdit->setFocus();

    connect(ui->pBulkButton, SIGNAL(clicked()), this, SLOT(onBulkClicked()));
    connect(ui->pSequentialButton, SIGNAL(clicked()), this, SLOT(onSequentialClicked()));

}

LayoutDialogChooser::~LayoutDialogChooser()
{
    delete ui;
}

void LayoutDialogChooser::onBulkClicked()
{

    emit bulkClicked();
}

void LayoutDialogChooser::onSequentialClicked()
{
    emit sequentialClicked();
}

QString LayoutDialogChooser::getTabName() const
{
    return ui->pTabNameEdit->text();
}

void LayoutDialogChooser::showEvent(QShowEvent*)
{
    ui->pTabNameEdit->grabKeyboard();
}

void LayoutDialogChooser::hideEvent(QHideEvent*)
{
    ui->pTabNameEdit->releaseKeyboard();
}


} // end Viewer namespace
