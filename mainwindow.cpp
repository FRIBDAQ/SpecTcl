#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "QRootCanvas.h"
#include <TH1.h>
#include <TCanvas.h>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
//    setLayout(ui->verticalLayout);

    QGridLayout* gridLayout = new QGridLayout(ui->frame);
    m_canvas = new QRootCanvas(ui->frame);
    gridLayout->addWidget(m_canvas,0,0);

    m_canvas->getCanvas()->Resize();
    m_canvas->getCanvas()->cd();

    m_hist = new TH1D("m_hist","The TEST", 10, 0, 10);
    m_hist->SetBinContent(2,3);
    m_hist->SetBinContent(5,1);
    m_hist->SetBinContent(8,6);
    m_hist->SetDirectory(0);

    m_hist->Draw();
    m_canvas->show();


    connect(ui->pushButton,SIGNAL(pressed()),this,SLOT(update()));
}

void MainWindow::update()
{
    auto bin = ui->spinBox->value();
    auto incr = ui->spinBox_2->value();

    auto content = m_hist->GetBinContent(bin);
    m_hist->SetBinContent(bin, content+incr);
    m_canvas->Modified(1);
    m_canvas->Update();
}

MainWindow::~MainWindow()
{
    delete ui;
    delete m_hist;
}
