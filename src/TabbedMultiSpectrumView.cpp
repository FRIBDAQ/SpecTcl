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
}

TabbedMultiSpectrumView::~TabbedMultiSpectrumView()
{
    delete ui;
}

MultiSpectrumView* TabbedMultiSpectrumView::addTab(const QString &title)
{

    ui->pTabWidget->addTab(new MultiSpectrumView(m_pSpecTcl, this), title);

    m_pCurrentView = dynamic_cast<MultiSpectrumView*>(ui->pTabWidget->currentWidget());
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


}
