#include "TabbedMultiSpectrumView.h"
#include "ui_TabbedMultiSpectrumView.h"
#include "MultiSpectrumView.h"
#include "SpecTclInterface.h"

#include <QPushButton>

#include <iostream>

using namespace std;

namespace Viewer
{

TabbedMultiSpectrumView::TabbedMultiSpectrumView(shared_ptr<SpecTclInterface> pSpecTcl,
                                                 QWidget *parent) :
    SpectrumView(parent),
    ui(new Ui::TabbedMultiSpectrumView),
    m_pCurrentView(nullptr),
    m_pSpecTcl(pSpecTcl),
    m_pAddButton(new QPushButton(this))
{
    ui->setupUi(this);

    m_pAddButton->setText("Add Tab");
    connect(m_pAddButton, SIGNAL(clicked()), this, SLOT(onAddTab()));

    ui->pTabWidget->setCornerWidget(m_pAddButton);

    addTab("Tab 1");

    ui->pTabWidget->setTabsClosable(true);

    updateCurrentViewToVisibleTab();

    connect(ui->pTabWidget, SIGNAL(currentChanged(int)), this, SLOT(onCurrentChanged(int)));

    connect(ui->pTabWidget, SIGNAL(tabCloseRequested(int)), this, SLOT(onTabCloseRequested(int)));
}

TabbedMultiSpectrumView::~TabbedMultiSpectrumView()
{
    delete ui;
}

MultiSpectrumView* TabbedMultiSpectrumView::addTab(const QString &title)
{

  ui->pTabWidget->addTab(new MultiSpectrumView(m_pSpecTcl, this), title);

  auto pView = dynamic_cast<MultiSpectrumView*>(ui->pTabWidget->currentWidget());

  return pView;
}

int TabbedMultiSpectrumView::getRowCount() const
{
    return m_pCurrentView->getRowCount();
}

int TabbedMultiSpectrumView::getColumnCount() const
{
    return m_pCurrentView->getColumnCount();
}

QRootCanvas* TabbedMultiSpectrumView::getCurrentCanvas()
{
    return m_pCurrentView->getCurrentCanvas();
}

std::vector<QRootCanvas*> TabbedMultiSpectrumView::getAllCanvases()
{
    return m_pCurrentView->getAllCanvases();
}

void TabbedMultiSpectrumView::onGeometryChanged(int row, int col)
{
    m_pCurrentView->onGeometryChanged(row, col);
}

void TabbedMultiSpectrumView::setCurrentCanvas(QWidget *pCanvas)
{
    m_pCurrentView->setCurrentCanvas(pCanvas);
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

void TabbedMultiSpectrumView::refreshAll()
{
    m_pCurrentView->refreshAll();
}

void TabbedMultiSpectrumView::onHistogramRemoved(HistogramBundle *pBundle)
{
    m_pCurrentView->onHistogramRemoved(pBundle);
}

void TabbedMultiSpectrumView::update(HistogramBundle *pHist)
{
    m_pCurrentView->update(pHist);
}

void TabbedMultiSpectrumView::drawHistogram(HistogramBundle *pHist)
{
    m_pCurrentView->drawHistogram(pHist);
}

void TabbedMultiSpectrumView::updateCurrentViewToVisibleTab()
{
  int index = ui->pTabWidget->currentIndex();
  m_pCurrentView = dynamic_cast<MultiSpectrumView*>(ui->pTabWidget->widget(index));
//  emit visibleGeometryChanged(m_pCurrentView->getRowCount(), m_pCurrentView->getColumnCount());
}

void TabbedMultiSpectrumView::onCurrentChanged(int index)
{
  cout << "onCurrentChanged to " << index << endl;
  m_pCurrentView = dynamic_cast<MultiSpectrumView*>(ui->pTabWidget->widget(index));
}

void TabbedMultiSpectrumView::onAddTab()
{
  int nTabs = ui->pTabWidget->count();
  addTab(QString("Tab %1").arg(nTabs+1));
}

void TabbedMultiSpectrumView::onTabCloseRequested(int index)
{
  if ((index == 0) && (ui->pTabWidget->count() == 1)) {
      return;
  } else {
      cout << "removing index = " << index << endl;
      ui->pTabWidget->removeTab(index);
  }
}

}
