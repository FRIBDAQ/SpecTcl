//    This software is Copyright by the Board of Trustees of Michigan
//    State University (c) Copyright 2016.
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

#include "MainWindow.h"
#include "ui_MainWindow.h"
#include "ConnectServer.h"
#include "MultiSpectrumView.h"
#include "TGo4CreateNewHistogram.h"
#include "SpecTclInterfaceFactory.h"
#include "SpecTclInterface.h"
#include "SpecTclInterfaceObserver.h"
#include "SpecTclInterfaceControl.h"
#include "ControlPanel.h"
#include "TabbedMultiSpectrumView.h"
#include "GlobalSettings.h"
#include "AutoUpdateDialog.h"
#include "TabWorkspace.h"
#include "SaveToRootDialog.h"
#include "ConfigCopySelector.h"


#include <QDebug>
#include <QDockWidget>
#include <QMessageBox>
#include <QSplitter>
#include <QVBoxLayout>
#include <QFrame>

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

    // Create the SpecTcl interface as a shared_ptr (it will live on outside of
    // this scope
    constructSpecTclInterface();

    assembleWidgets();

    // Register the SpecTcl interface observers
    addInterfaceObservers();

    // start polling for  histogram information
    m_specTclControl.getInterface()->enableHistogramInfoPolling(true);

    // with everything set up that depends on gates, start gate polling.
    m_specTclControl.getInterface()->enableGatePolling(true);

    // set up connections
    connectSignalsAndSlots();

    createShortcuts();
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
    SpecTclInterfaceFactory factory;
    int mode = GlobalSettings::getSessionMode();

    if (mode == 0) {
        m_specTclControl.setInterface(factory.create(SpecTclInterfaceFactory::REST));
    } else {
        m_specTclControl.setInterface(factory.create(SpecTclInterfaceFactory::Hybrid));
    }
}

//
//
void MainWindow::assembleWidgets()
{

    // Create the main layout
    m_pView = new TabbedMultiSpectrumView(m_specTclControl.getInterface(), pUI->frame);
    m_pMainWidget = m_pView;

    pUI->gridLayout->addWidget(m_pView, 0, 0);
}

//
//
void MainWindow::addInterfaceObservers()
{
    m_specTclControl.addGenericSpecTclInterfaceObserver(*this);
    m_specTclControl.addGenericSpecTclInterfaceObserver(*m_pView);
}


//
//
void MainWindow::connectSignalsAndSlots()
{
    connect(pUI->actionConfigure,SIGNAL(activated()),this,SLOT(onConfigure()));

    connect(pUI->actionNewHistogram,SIGNAL(triggered()),this,SLOT(onNewHistogram()));

    connect(pUI->pAutoUpdateAction,SIGNAL(triggered()),this,SLOT(launchAutoUpdateDialog()));

    connect(pUI->pSaveAsAction, SIGNAL(triggered()), this, SLOT(onSaveAs()));

    connect(pUI->actionCopySpecAttributes, SIGNAL(triggered()), this,
            SLOT(onCopySpectrumAttributes()));
}


//
//
void MainWindow::onConfigure() {
    ConnectDialog dialog(m_specTclControl);
    dialog.exec();
}

//
//
void MainWindow::createDockWindows()
{
}

//
//
void MainWindow::setSpecTclInterface(std::shared_ptr<SpecTclInterface> pInterface)
{
    // connect the new signal-slots
    connect(pInterface.get(), SIGNAL(histogramContentUpdated(HistogramBundle*)),
            m_pView, SLOT(update(HistogramBundle*)));

}

void MainWindow::launchAutoUpdateDialog()
{
    TabWorkspace& workspace = m_pView->getCurrentWorkspace();

    AutoUpdateDialog dialog(workspace.getUpdater(), this);
    dialog.exec();
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

void MainWindow::onCopySpectrumAttributes()
{
    ConfigCopySelector selector(m_pView->getCurrentWorkspace().getView());
    selector.exec();
}

void MainWindow::createShortcuts()
{
}

void MainWindow::onSaveAs() {

    qDebug() << "MainWindow::onSaveAs()";
    auto pDialog = new SaveToRootDialog(*m_pView, m_specTclControl.getInterface(), this);

    connect(pDialog, SIGNAL(accepted()), this, SLOT(closeDialog()));
    connect(pDialog, SIGNAL(rejected()), this, SLOT(closeDialog()));

    pUI->gridLayout->addWidget(pDialog, 0, 0);

    m_pMainWidget->hide();
    pDialog->show();
    m_pMainWidget = pDialog;

}

void MainWindow::closeDialog()
{
    if (m_pMainWidget != m_pView) {
        m_pMainWidget->hide();

        delete m_pMainWidget;

        m_pView->show();
        m_pMainWidget = m_pView;
    }

}

} // end of namespace
