//    This software is Copyright by the Board of Trustees of Michigan
//    State University (c) Copyright 2015.
//
//    This program is free software: you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation, either version 3 of the License, or
//    any later version.
//
//    This program is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License
//    along with this program.  If not, see <http://www.gnu.org/licenses/>.
//
//    Authors:
//    Jeromy Tompkins 
//    NSCL
//    Michigan State University
//    East Lansing, MI 48824-1321

static const char* Copyright = "(C) Copyright Michigan State University 2015, All rights reserved";
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "ConnectServer.h"
#include "SpectrumViewer.h"
#include <QDebug>
#include <QDockWidget>
#include <HistogramView.h>
#include <DockableGateManager.h>
#include "TGo4CreateNewHistogram.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    m_view = new SpectrumViewer(ui->frame);
    ui->gridLayout->addWidget(m_view);

    m_histView = new HistogramView(this);
    m_gateView = new DockableGateManager(*m_view, this);

    createDockWindows();

    connect(ui->actionConnect,SIGNAL(activated()),this,SLOT(onConnect()));
    connect(m_histView,SIGNAL(histSelected(HistogramBundle*)),m_view,SLOT(update(HistogramBundle*)));
    connect(ui->actionHIstograms,SIGNAL(triggered()),this,SLOT(dockHistograms()));
    connect(ui->actionNewHistogram,SIGNAL(triggered()),this,SLOT(onNewHistogram()));
    connect(ui->actionGates,SIGNAL(triggered()),this,SLOT(dockGates()));
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
    m_gateView->setAllowedAreas(Qt::LeftDockWidgetArea|Qt::RightDockWidgetArea);

    addDockWidget(Qt::LeftDockWidgetArea,m_histView);
    addDockWidget(Qt::LeftDockWidgetArea,m_gateView);
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

void MainWindow::dockGates()
{
    if (m_gateView->isVisible()) {
        return;
    } else {
        addDockWidget(Qt::LeftDockWidgetArea,m_gateView);
        m_gateView->show();
    }
}

void MainWindow::onNewHistogram()
{
    TGo4CreateNewHistogram dialog(0);
    dialog.exec();
}
