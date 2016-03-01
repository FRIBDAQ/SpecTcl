#include "ControlPanel.h"
#include "GeometrySelector.h"
#include "SpecTclInterface.h"
#include "SpectrumView.h"
#include "HistogramList.h"
#include "HistogramBundle.h"
#include "ui_ControlPanel.h"
#include <set>

using namespace std;

namespace Viewer
{

ControlPanel::ControlPanel(std::shared_ptr<SpecTclInterface> pSpecTcl,
                           SpectrumView *pView, QWidget *parent) :
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

void ControlPanel::setSpecTclInterface(std::shared_ptr<SpecTclInterface> pSpecTcl)
{
    m_pSpecTcl = pSpecTcl;
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
      auto canvases = m_pView->getAllCanvases();

      // this needs thread synchronization
      auto it = canvases.begin();
      auto it_end = canvases.end();
      std::set<TH1*> requestHistory;
      while ( it != it_end ) {
          vector<TH1*> hists = m_pView->getAllHists(*it);
          for (auto pHist : hists) {
              // try to insert into the request history.. if the hist already
              // lives in the request history, the second element of the
              // returned pair will be false
              if ( requestHistory.insert(pHist).second == true ) {
                m_pSpecTcl->requestHistContentUpdate(QString(pHist->GetName()));
              }
          }
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
