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
#include "SpecTclInterfaceControl.h"
#include "ControlPanel.h"
#include "TabbedMultiSpectrumView.h"
#include "GlobalSettings.h"

#include <QDebug>
#include <QDockWidget>
#include <QMessageBox>

namespace Viewer
{


//
//
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    pUI(new Ui::MainWindow),
    m_specTclControl(std::shared_ptr<SpecTclInterface>())
{
    pUI->setupUi(this);

    setWindowIcon(QIcon(":/icons/spectra_logo_16x16.png"));
    setWindowIconText("Spectra");

    assembleWidgets();

    // Create the SpecTcl interface as a shared_ptr (it will live on outside of
    // this scope
    constructSpecTclInterface();

    // Register the SpecTcl interface observers
    addInterfaceObservers();

    // start polling for  histogram information
    m_specTclControl.getInterface()->enableHistogramInfoPolling(true);

    // with everything set up that depends on gates, start gate polling.
    m_specTclControl.getInterface()->enableGatePolling(true);

    // set up connections
    connectSignalsAndSlots();
}

//
//
MainWindow::~MainWindow()
{
}

//
//
void MainWindow::constructSpecTclInterface()
{
    GlobalSettings::setSessionMode(1);
    SpecTclInterfaceFactory factory;
    m_specTclControl.setInterface(factory.create(SpecTclInterfaceFactory::Hybrid));
}

//
//
void assembleWidgets()
{

    // Create the main layout
    m_pView = new TabbedMultiSpectrumView(m_specTclControl.getInterface(), pUI->frame);
    m_pControls = new ControlPanel(m_specTclControl.getInterface(), m_pView, pUI->frame);

    pUI->gridLayout->addWidget(m_pView);
    pUI->gridLayout->addWidget(m_pControls);

    createDockWindows();
}

//
//
void MainWindow::addInterfaceObservers()
{
    m_specTclControl.addGenericSpecTclInterfaceObserver(*m_pView);
    m_specTclControl.addGenericSpecTclInterfaceObserver(*m_pControls);
    m_specTclControl.addGenericSpecTclInterfaceObserver(*m_histView);
    m_specTclControl.addGenericSpecTclInterfaceObserver(*m_gateView);
}


//
//
void MainWindow::connectSignalsAndSlots()
{
    connect(pUI->actionConfigure,SIGNAL(activated()),this,SLOT(onConfigure()));

    connect(m_histView,SIGNAL(histSelected(HistogramBundle*)),
            m_pView,SLOT(drawHistogram(HistogramBundle*)));

    connect(pUI->actionHIstograms,SIGNAL(triggered()),this,SLOT(dockHistograms()));

    connect(pUI->actionNewHistogram,SIGNAL(triggered()),this,SLOT(onNewHistogram()));

    connect(pUI->actionGates,SIGNAL(triggered()),this,SLOT(dockGates()));

    connect(m_pControls, SIGNAL(geometryChanged(int, int)),
            m_pView, SLOT(onGeometryChanged(int, int)));

    connect(m_specTclControl.getInterface().get(),
            SIGNAL(histogramContentUpdated(HistogramBundle*)),
            m_pView, SLOT(update(HistogramBundle*)));
}


//
//
void MainWindow::onConfigure() {
    ConnectDialog dialog(*this);
    dialog.exec();
}

//
//
void MainWindow::createDockWindows()
{
    // Create the dockable widgets
    m_histView = new HistogramView(m_specTclControl.getInterface(), this);
    m_gateView = new DockableGateManager(*m_pView,
                                         m_specTclControl.getInterface(), this);

    m_histView->setAllowedAreas(Qt::LeftDockWidgetArea|Qt::RightDockWidgetArea);
    m_gateView->setAllowedAreas(Qt::LeftDockWidgetArea|Qt::RightDockWidgetArea);

    addDockWidget(Qt::LeftDockWidgetArea,m_histView);
    addDockWidget(Qt::LeftDockWidgetArea,m_gateView);
}

//
//
void MainWindow::setSpecTclInterface(std::unique_ptr<SpecTclInterface> pInterface)
{
    m_specTclControl.setInterface( std::move(pInterface) );

    // connect the new signal-slots
    connect(pInterface.get(), SIGNAL(histogramContentUpdated(HistogramBundle*)),
            m_pView, SLOT(update(HistogramBundle*)));

}

//
//
void MainWindow::dockHistograms()
{
    if (m_histView->isVisible()) {
        return;
    } else {
        addDockWidget(Qt::LeftDockWidgetArea, m_histView);
        m_histView->show();
    }
}

//
//
void MainWindow::dockGates()
{
    if (m_gateView->isVisible()) {
        return;
    } else {
        addDockWidget(Qt::LeftDockWidgetArea, m_gateView);
        m_gateView->show();
    }
}

//
//
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

} // end of namespace
