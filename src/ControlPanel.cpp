#include "ControlPanel.h"
#include "GeometrySelector.h"
#include "SpecTclInterface.h"
#include "SpectrumView.h"
#include "HistogramList.h"
#include "HistogramBundle.h"
#include "ui_ControlPanel.h"

namespace Viewer
{

ControlPanel::ControlPanel(SpecTclInterface *pSpecTcl, SpectrumView *pView, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ControlPanel),
    m_pGeoSelector(new GeometrySelector(this)),
    m_pSpecTcl(pSpecTcl),
    m_pView(pView)
{
    ui->setupUi(this);

    ui->horizontalLayout->insertWidget(0, m_pGeoSelector, 0, Qt::AlignLeft);

    // at the moment we will deal with these independently... ultimately the user should
    // accept their
    connect(m_pGeoSelector, SIGNAL(rowCountChanged(int)), this, SLOT(onRowCountChanged(int)));
    connect(m_pGeoSelector, SIGNAL(columnCountChanged(int)), this, SLOT(onColumnCountChanged(int)));

    connect(ui->pUpdateSelected, SIGNAL(clicked()), this, SLOT(onUpdateSelected()));
    connect(ui->pUpdateAll, SIGNAL(clicked()), this, SLOT(onUpdateAll()));
    connect(ui->pRefresh, SIGNAL(clicked()), this, SLOT(onRefresh()));
}

ControlPanel::~ControlPanel()
{
    delete ui;
}

void ControlPanel::onUpdateSelected()
{
  if (m_pSpecTcl) {
      m_pSpecTcl->requestHistContentUpdate(m_pView->getCurrentCanvas());
  }
}

// naively this currently just updates ALL of the histograms... will need to
// be alterred to update only the hists that are displayed
void ControlPanel::onUpdateAll()
{
  if (m_pSpecTcl) {
    auto pHistList = m_pSpecTcl->getHistogramList();

    // no need for thread sync b/c nothing alters the list in a separate thread
    // only the content of the histograms in the list might be alterred
    auto it = pHistList->begin();
    auto it_end = pHistList->end();
    while ( it != it_end ) {
      m_pSpecTcl->requestHistContentUpdate(it->first);
      ++it;
    }
  }
}

void ControlPanel::onRefresh()
{
  m_pView->refreshAll();
}

void ControlPanel::onRowCountChanged(int nRows)
{
  emit geometryChanged(nRows, m_pGeoSelector->getColumnCount());
}

void ControlPanel::onColumnCountChanged(int nColumns)
{
  emit geometryChanged(m_pGeoSelector->getRowCount(), nColumns);
}


} // end of namespace
