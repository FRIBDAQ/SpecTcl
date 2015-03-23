#include "ParametView.h"
#include "ui_ParameterView.h"

DockWidget::DockWidget(QWidget *parent) :
    QDockWidget(parent),
    ui(new Ui::DockWidget)
{
    ui->setupUi(this);
}

DockWidget::~DockWidget()
{
    delete ui;
}
