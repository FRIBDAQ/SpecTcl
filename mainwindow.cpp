#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "ConnectServer.h"
#include "SpectrumViewer.h"
#include <QDebug>
#include <QDockWidget>
#include <HistogramView.h>
#include "TGo4CreateNewHistogram.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    m_view = new SpectrumViewer(ui->frame);
    ui->gridLayout->addWidget(m_view);

    m_histView = new HistogramView(this);

    createDockWindows();

    connect(ui->actionConnect,SIGNAL(activated()),this,SLOT(onConnect()));
    connect(m_histView,SIGNAL(histSelected(TH1*)),m_view,SLOT(update(TH1*)));
    connect(ui->actionHIstograms,SIGNAL(triggered()),this,SLOT(dockHistograms()));
    connect(ui->actionNewHistogram,SIGNAL(triggered()),this,SLOT(onNewHistogram()));
}

void MainWindow::onConnect() {
    ConnectDialog dialog;
    dialog.exec();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::createDockWindows()
{

    m_histView->setAllowedAreas(Qt::LeftDockWidgetArea|Qt::RightDockWidgetArea);

    addDockWidget(Qt::LeftDockWidgetArea,m_histView);
}

void MainWindow::dockHistograms()
{
    if (m_histView->isVisible()) {
        return;
    } else {
        addDockWidget(Qt::LeftDockWidgetArea,m_histView);
        m_histView->show();
    }
}


void MainWindow::onNewHistogram()
{
    TGo4CreateNewHistogram dialog(0);
    dialog.exec();
}
