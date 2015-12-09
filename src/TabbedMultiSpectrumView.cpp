#include "TabbedMultiSpectrumView.h"
#include "ui_TabbedMultiSpectrumView.h"
#include "MultiSpectrumView.h"
#include "SpecTclInterface.h"

namespace Viewer
{

TabbedMultiSpectrumView::TabbedMultiSpectrumView(SpecTclInterface* pSpecTcl, QWidget *parent) :
    SpectrumView(parent),
    ui(new Ui::TabbedMultiSpectrumView),
    m_pCurrentView(nullptr),
    m_pSpecTcl(pSpecTcl)
{
    ui->setupUi(this);

    addTab("Tab 1");
    addTab("Tab 2");
    addTab("Tab 3");

    connect(ui->pTabWidget, SIGNAL(currentChanged(int)), this, SLOT(onCurrentChanged(int)));
}

TabbedMultiSpectrumView::~TabbedMultiSpectrumView()
{
    delete ui;
}

MultiSpectrumView* TabbedMultiSpectrumView::addTab(const QString &title)
{

  int count = ui->pTabWidget->count();

  ui->pTabWidget->insertTab(count-1, new MultiSpectrumView(m_pSpecTcl, this), title);

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

void TabbedMultiSpectrumView::onCurrentChanged(int index)
{
  m_pCurrentView = dynamic_cast<MultiSpectrumView*>(ui->pTabWidget->widget(index));
}

}
