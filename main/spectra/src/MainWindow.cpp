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
#include "SaveAsDialog.h"
#include "ConfigCopySelector.h"
#include "HistogramBundle.h"
#include "HistogramList.h"
#include "SpectraSpectrumInterface.h"
#include "SpectrumQueryInterface.h"
#include "PrintDialog.h"
#include "CanvasOps.h"
#include "QRootCanvas.h"

#include <QDebug>
#include <QDockWidget>
#include <QMessageBox>
#include <QSplitter>
#include <QVBoxLayout>
#include <QFrame>

#include <TH1.h>
#include <TCanvas.h>
#include <TAxis.h>
#include <QString>
#include <QStringList>


Win::SpectrumQueryInterface gSpectrumInterface;

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

    std::shared_ptr<Win::SpectrumQuerier> pQuerier(
                new SpectraSpectrumInterface(m_specTclControl.getInterface()));
    gSpectrumInterface.setQueryEntity(pQuerier);
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
//    m_specTclControl.addGenericSpecTclInterfaceObserver(*this);
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

    connect(pUI->pPrintAction, SIGNAL(triggered()), this, SLOT(onPrint()));
}


//
//
void MainWindow::onConfigure() {
    ConnectDialog dialog(m_specTclControl);
    dialog.exec();
}

//
//
void MainWindow::setSpecTclInterface(std::shared_ptr<SpecTclInterface> pInterface)
{
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
    ConfigCopySelector selector(m_pView->getCurrentWorkspace().getView(),
                                m_specTclControl.getInterface());
    selector.exec();

    ConfigCopySelection selection = selector.getSelection();

    ConfigCopyTarget sourceTarget = selection.s_sourceTarget;

    // User cancelled operation or did not select a dest
    if (selection.s_destTargets.size() == 0) return;

    // we now have to be careful about how we apply these attributes. They
    // correspond only the displayed histograms and NOT others.


    for (int i=0; i<selection.s_destTargets.size(); ++i) {

        ConfigCopyTarget& destTarget = selection.s_destTargets[i];

        TH1& pDestHist = *(destTarget.s_pHist);
        TH1& pSourceHist = *(sourceTarget.s_pHist);

    	if (selection.s_copyXAxis) {
            // copy x axis range
            TAxis* pDestAxis = pDestHist.GetXaxis();
            TAxis* pSrcAxis = pSourceHist.GetXaxis();

            int lowerLimit = pSrcAxis->GetFirst();
            int upperLimit = pSrcAxis->GetLast();

            pDestAxis->SetRange(lowerLimit, upperLimit);
    	}

    	if (selection.s_copyYAxis) {
    		// copy x axis range
            TAxis* pDestAxis = pDestHist.GetYaxis();
            TAxis* pSrcAxis = pSourceHist.GetYaxis();

            int lowerLimit = pSrcAxis->GetFirst();
            int upperLimit = pSrcAxis->GetLast();
            pDestAxis->SetRange(lowerLimit, upperLimit);
    	}

        if (selection.s_copyMinimum) {
            pDestHist.SetMinimum(pSourceHist.GetMinimum());
        }

        if (selection.s_copyMaximum) {
            pDestHist.SetMaximum(pSourceHist.GetMaximum());
        }

        SpectrumView& view = m_pView->getCurrentWorkspace().getView();
        QRootCanvas* pSourceCanvas = view.getCanvas(sourceTarget.s_row, sourceTarget.s_col);
        QRootCanvas* pDestCanvas = view.getCanvas(destTarget.s_row, destTarget.s_col);

    	if (selection.s_copyDrawOption) {

            if (pDestCanvas && pSourceCanvas) {
                QString sourceOpt = CanvasOps::getDrawOption(pSourceCanvas->getCanvas(), &pSourceHist);
                CanvasOps::setDrawOption(pDestCanvas->getCanvas(), &pDestHist, sourceOpt);
            }
    	}

        if (selection.s_copyLogx) {
            if (pDestCanvas && pSourceCanvas) {
                pDestCanvas->SetLogx(pSourceCanvas->GetLogx());
            }
        }

        if (selection.s_copyLogy) {
            if (pDestCanvas && pSourceCanvas) {
                pDestCanvas->SetLogy(pSourceCanvas->GetLogy());
            }
        }

        if (selection.s_copyLogz) {
            if (pDestCanvas && pSourceCanvas) {
                pDestCanvas->SetLogz(pSourceCanvas->GetLogz());
            }
        }

    }

    m_pView->getCurrentWorkspace().getView().refreshAll();
}

void MainWindow::createShortcuts()
{
}

void MainWindow::onSaveAs() {

    SaveAsDialog dialog(*m_pView, m_specTclControl.getInterface(), this);
    dialog.setModal(true);
    dialog.exec();
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

void MainWindow::onPrint()
{
    PrintDialog dialog(m_specTclControl.getInterface(), *m_pView, this);
    dialog.exec();
}

} // end of namespace
