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

#include "TabbedMultiSpectrumView.h"
#include "ui_TabbedMultiSpectrumView.h"
#include "MultiSpectrumView.h"
#include "SpecTclInterface.h"
#include "SpectrumLayoutDialog.h"
#include "TabWorkspace.h"
#include "TabFromWinFileCompositor.h"

#include <QPushButton>
#include <QWidget>
#include <QSize>

#include <iostream>

using namespace std;

namespace Viewer
{

TabbedMultiSpectrumView::TabbedMultiSpectrumView(shared_ptr<SpecTclInterface> pSpecTcl,
                                                 QWidget *parent) :
    ui(new Ui::TabbedMultiSpectrumView),
    m_pCurrentView(nullptr),
    m_pSpecTcl(pSpecTcl),
    m_pAddButton(new QPushButton(this))
{
    ui->setupUi(this);

    m_pAddButton->setText(tr("&New Tab"));
    ui->pTabWidget->setCornerWidget(m_pAddButton);

    addTab("");

    connect(m_pAddButton, SIGNAL(clicked()), this, SLOT(onAddTab()));
    connect(ui->pTabWidget, SIGNAL(currentChanged(int)), this, SLOT(onCurrentChanged(int)));
    connect(ui->pTabWidget, SIGNAL(tabCloseRequested(int)), this, SLOT(onTabCloseRequested(int)));
 }

TabbedMultiSpectrumView::~TabbedMultiSpectrumView()
{
    delete ui;
}

void TabbedMultiSpectrumView::addTab(const QString &title)
{

  auto pSpecLayoutDialog = new SpectrumLayoutDialog(m_pSpecTcl, this);
  int tabIndex = ui->pTabWidget->addTab(pSpecLayoutDialog, title);

  connect(pSpecLayoutDialog, SIGNAL(spectraChosenToDraw(QStringList)),
          this, SLOT(onNewTabContentsSelected(QStringList)));
  connect(pSpecLayoutDialog, SIGNAL(loadFileChosen(QString)),
          this, SLOT(onNewTabContentsFromFile(QString)));

  ui->pTabWidget->setCurrentIndex(tabIndex);
}



void TabbedMultiSpectrumView::onNewTabContentsSelected(QStringList selection)
{

    auto pSetupWidget = dynamic_cast<SpectrumLayoutDialog*>(ui->pTabWidget->currentWidget());

    QString tabName = pSetupWidget->getTabName();

    int index = ui->pTabWidget->currentIndex();
    ui->pTabWidget->removeTab(index);

    auto pWorkspace = new TabWorkspace(m_pSpecTcl, this);
    pWorkspace->layoutSpectra(selection);

    int newTabIndex = ui->pTabWidget->insertTab(index, pWorkspace, tabName);

    // we will use the returned index because it may be different than the original
    // index.
    ui->pTabWidget->setCurrentIndex(newTabIndex);
}

void TabbedMultiSpectrumView::onNewTabContentsFromFile(QString fileName)
{

    auto pSetupWidget = dynamic_cast<SpectrumLayoutDialog*>(ui->pTabWidget->currentWidget());

    QString tabName = pSetupWidget->getTabName();

    int index = ui->pTabWidget->currentIndex();
    ui->pTabWidget->removeTab(index);

    auto pWorkspace = new TabWorkspace(m_pSpecTcl, this);

    std::cout << fileName.toStdString() << std::endl;
    if (fileName.endsWith(".win")) {
        TabFromWinFileCompositor compositor(m_pSpecTcl);

        compositor.compose(*pWorkspace, fileName);
    }

    int newTabIndex = ui->pTabWidget->insertTab(index, pWorkspace, tabName);

    // we will use the returned index because it may be different than the original
    // index.
    ui->pTabWidget->setCurrentIndex(newTabIndex);
}



void TabbedMultiSpectrumView::setSpecTclInterface(std::shared_ptr<SpecTclInterface> pSpecTcl)
{
    m_pSpecTcl = pSpecTcl;

    // tell all of the spectrumviews in the tabs
    int nTabs = ui->pTabWidget->count();
    for (int tab=0; tab<nTabs; ++tab) {

        auto pView = dynamic_cast<MultiSpectrumView*>(ui->pTabWidget->widget(tab));
        if (pView) {
            pView->setSpecTclInterface(m_pSpecTcl);
        }
    }
}

void TabbedMultiSpectrumView::onCurrentChanged(int index)
{
  m_pCurrentView = dynamic_cast<MultiSpectrumView*>(ui->pTabWidget->widget(index));
}

void TabbedMultiSpectrumView::onAddTab()
{

    int nTabs = ui->pTabWidget->count();
    addTab(QString(tr("Tab %1")).arg(nTabs+1));

    if (ui->pTabWidget->count() > 0) {
        ui->pTabWidget->setTabsClosable(true);
    }

}

void TabbedMultiSpectrumView::onTabCloseRequested(int index)
{

  if ((index != 0) || (ui->pTabWidget->count() != 1)) {
      ui->pTabWidget->removeTab(index);
  }

  if (ui->pTabWidget->count() == 1) {
      ui->pTabWidget->setTabsClosable(false);
  }

}

TabWorkspace& TabbedMultiSpectrumView::getCurrentWorkspace()
{
    return dynamic_cast<TabWorkspace&>(*ui->pTabWidget->currentWidget());
}



} // end Viewer namespace
