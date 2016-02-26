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
#include "MultiSpectrumView.h"
#include "SpectrumViewer.h"
#include "HistogramView.h"
#include "DockableGateManager.h"
#include "TGo4CreateNewHistogram.h"
#include "SpecTclInterfaceFactory.h"
#include "SpecTclInterface.h"
#include "SpecTclInterfaceObserver.h"
#include "ControlPanel.h"
#include "TabbedMultiSpectrumView.h"
#include "GlobalSettings.h"

#include <QDebug>
#include <QDockWidget>
#include <QMessageBox>

namespace Viewer
{

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    setWindowIcon(QIcon(":/icons/spectra_logo_16x16.png"));
    setWindowIconText("Spectra");

    GlobalSettings::setSessionMode(1);
    auto pInterface = SpecTclInterfaceFactory().create(SpecTclInterfaceFactory::Hybrid);
    m_pSpecTcl = std::shared_ptr<SpecTclInterface>( std::move(pInterface) );

    m_pView = new TabbedMultiSpectrumView(m_pSpecTcl, ui->frame);
    std::unique_ptr<SpecTclInterfaceObserver>
         pViewObs(new GenericSpecTclInterfaceObserver<SpectrumView>(*m_pView));
    addSpecTclInterfaceObserver(std::move(pViewObs));

    m_pControls = new ControlPanel(m_pSpecTcl, m_pView, ui->frame);
    std::unique_ptr<SpecTclInterfaceObserver>
         pControlObs(new GenericSpecTclInterfaceObserver<ControlPanel>(*m_pControls));
    addSpecTclInterfaceObserver(std::move(pControlObs));

    ui->gridLayout->addWidget(m_pView);
    ui->gridLayout->addWidget(m_pControls);

    m_histView = new HistogramView(m_pSpecTcl, this);
    std::unique_ptr<SpecTclInterfaceObserver>
         pHistViewObs(new GenericSpecTclInterfaceObserver<HistogramView>(*m_histView));
    addSpecTclInterfaceObserver(std::move(pHistViewObs));

    m_gateView = new DockableGateManager(*m_pView, m_pSpecTcl, this);
    std::unique_ptr<SpecTclInterfaceObserver>
         pGateViewObs(new GenericSpecTclInterfaceObserver<DockableGateManager>(*m_gateView));
    addSpecTclInterfaceObserver(std::move(pGateViewObs));

    // start polling for  histogram information
    m_pSpecTcl->enableHistogramInfoPolling(true);

    // with everything set up that depends on gates, start gate polling.
    m_pSpecTcl->enableGatePolling(true);

    createDockWindows();

    connect(ui->actionConnect,SIGNAL(activated()),this,SLOT(onConnect()));
    connect(m_histView,SIGNAL(histSelected(HistogramBundle*)),m_pView,SLOT(drawHistogram(HistogramBundle*)));
    connect(ui->actionHIstograms,SIGNAL(triggered()),this,SLOT(dockHistograms()));
    connect(ui->actionNewHistogram,SIGNAL(triggered()),this,SLOT(onNewHistogram()));
    connect(ui->actionGates,SIGNAL(triggered()),this,SLOT(dockGates()));

    connect(m_pControls, SIGNAL(geometryChanged(int, int)), 
            m_pView, SLOT(onGeometryChanged(int, int)));
    connect(m_pSpecTcl.get(), SIGNAL(histogramContentUpdated(HistogramBundle*)),
            m_pView, SLOT(update(HistogramBundle*)));
}

void MainWindow::onConnect() {
    ConnectDialog dialog(*this);
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

void MainWindow::setSpecTclInterface(std::unique_ptr<SpecTclInterface> pInterface)
{
    bool gatePollingEnabled = m_pSpecTcl->gatePollingEnabled();
    bool histPollingEnabled = m_pSpecTcl->histogramInfoPollingEnabled();

    std::shared_ptr<SpecTclInterface> pShared( std::move(pInterface) );
    m_pSpecTcl = pShared;

    m_pSpecTcl->enableGatePolling(gatePollingEnabled);
    m_pSpecTcl->enableHistogramInfoPolling(histPollingEnabled);

    // tell all of the dependent objects that the SpecTcl interface has changed.
    notifyObservers();
}

void MainWindow::addSpecTclInterfaceObserver(std::unique_ptr<SpecTclInterfaceObserver> pObserver)
{
    m_interfaceObservers.insert(m_interfaceObservers.end(), std::move(pObserver));
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
  try {
    TGo4CreateNewHistogram dialog(0);
    dialog.exec();
  } catch (std::exception& exc) {

    QString errmsg(exc.what());
    QMessageBox::warning(this, "Unable to create histograms", errmsg);
  }
}

void MainWindow::notifyObservers() {

    for(auto& pObserver : m_interfaceObservers) {
        pObserver->update(m_pSpecTcl);
    }

}

} // end of namespace
