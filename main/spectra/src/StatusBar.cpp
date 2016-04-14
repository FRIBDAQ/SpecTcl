#include "StatusBar.h"
#include "ui_StatusBar.h"

#include <iostream>

namespace Viewer
{

StatusBar::StatusBar(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::StatusBar)
{
    ui->setupUi(this);
    ui->pLeftLabel->setText(QString("--"));
    ui->pMiddleLabel->setText(QString("--"));
    ui->pRightLabel->setText(QString("--"));
}

StatusBar::~StatusBar()
{
    delete ui;
}

void StatusBar::onCursorMoved(const char *pStatus)
{
    QString msg(pStatus);

    ui->pRightLabel->setText(msg);
}



} // end Viewer namespace
