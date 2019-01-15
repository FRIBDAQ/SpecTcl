#include "canvascontrol.h"
#include "ui_canvascontrol.h"
#include <iostream>

CanvasControl::CanvasControl(QWidget *parent) :
    QFrame(parent),
    ui(new Ui::CanvasControl)
{
    ui->setupUi(this);
}

CanvasControl::~CanvasControl()
{
    delete ui;
}

void CanvasControl::on_pClearSelected_clicked()
{
  std::cerr << "Clearing selected spectrum\n";
}

void CanvasControl::on_pClearAll_clicked()
{
    std::cerr << "Clearing all spectra\n";
}

void CanvasControl::on_pClearVisible_clicked()
{
    std::cerr << "Clearing spectra in this layout\n";
}

void CanvasControl::on_pEmptySelected_clicked()
{
    std::cerr << "Emptying the selected canvas\n";
}

void CanvasControl::on_pEmptyVisible_clicked()
{
    std::cerr << "Emptying the visible canvases\n";
}
