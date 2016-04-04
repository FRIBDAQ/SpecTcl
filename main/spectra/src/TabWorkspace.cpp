#include "TabWorkspace.h"

#include "SpecTclInterface.h"
#include "MultiSpectrumView.h"
#include "ViewDrawPanel.h"
#include "ControlPanel.h"
#include "GeometrySelector.h"
#include "MultiInfoPanel.h"

#include <QSplitter>

namespace Viewer
{

TabWorkspace::TabWorkspace(std::shared_ptr<SpecTclInterface> pSpecTcl, QWidget *parent) :
    QWidget(parent),
    m_pSpecTcl(pSpecTcl),
    m_pView(nullptr),
    m_pDrawPanel(nullptr),
    m_pControls(nullptr),
    m_pInfoPanel(nullptr)

{

    setUpUI();

    // populate the histogram draw panel with histograms
    m_pDrawPanel->setHistogramList(m_pSpecTcl->getHistogramList());

    connectSignals();

}


void TabWorkspace::setUpUI()
{
    QSplitter* pVSplitter = new QSplitter(this);
    pVSplitter->setOrientation(Qt::Vertical);

    QSplitter* pHSplitter = new QSplitter(this);
    pHSplitter->setOrientation(Qt::Horizontal);

    m_pDrawPanel = new ViewDrawPanel(m_pSpecTcl);
    m_pView      = new MultiSpectrumView(m_pSpecTcl);
    m_pInfoPanel = new MultiInfoPanel(*m_pView, m_pSpecTcl, this);
    m_pControls  = new ControlPanel(m_pSpecTcl, m_pView, this);

    pVSplitter->addWidget(m_pView);
    pVSplitter->addWidget(m_pControls);

    // adjust the sizes of the
    QSize viewSize    = m_pView->sizeHint();
    QSize controlSize = m_pView->sizeHint();
    pVSplitter->setSizes({ viewSize.height()*2, 40 });

    pHSplitter->addWidget(m_pDrawPanel);
    pHSplitter->addWidget(pVSplitter);
    pHSplitter->addWidget(m_pInfoPanel);

    auto pMainLayout = new QVBoxLayout;
    pMainLayout->addWidget(pHSplitter);


    m_pInfoPanel->hide();

    setLayout(pMainLayout);
}

void TabWorkspace::connectSignals()
{
    connect(m_pDrawPanel, SIGNAL(histSelected(HistogramBundle*)),
            m_pView, SLOT(drawHistogram(HistogramBundle*)));

    connect(m_pSpecTcl.get(), SIGNAL(histogramContentUpdated(HistogramBundle*)),
            m_pView, SLOT(updateView(HistogramBundle*)));

    connect(m_pDrawPanel, SIGNAL(geometryChanged(int, int)),
            m_pView, SLOT(onGeometryChanged(int, int)));

    connect(m_pView, SIGNAL(currentCanvasChanged(QRootCanvas&)),
            m_pInfoPanel, SLOT(currentCanvasChanged(QRootCanvas&)));

    connect(m_pView, SIGNAL(canvasContentChanged(QRootCanvas&)),
            m_pInfoPanel, SLOT(currentCanvasChanged(QRootCanvas&)));

    connect(m_pControls, SIGNAL(statisticsButtonClicked()),
            this, SLOT(showHideStatistics()));

    connect(m_pControls, SIGNAL(displayButtonClicked()),
            this, SLOT(showHideDrawPanel()));

}

void TabWorkspace::layoutSpectra(QStringList spectrumList)
{
    m_pView->clearLayout();

    // we first set the geometry before laying out the spectra to avoid
    // triggering any redraws that might change the
    int nRows, nCols;
    std::tie(nRows, nCols) = m_pView->computeOptimalGeometry(spectrumList.count());
    m_pDrawPanel->getGeometrySelector().setGeometry(nRows, nCols);

    m_pView->layoutSpectra(spectrumList);
}


void TabWorkspace::showHideStatistics()
{
    if (m_pInfoPanel->isVisible()) {
        m_pInfoPanel->hide();
    } else {
        m_pInfoPanel->show();
    }
}


void TabWorkspace::showHideDrawPanel()
{
    if (m_pDrawPanel->isVisible()) {
        m_pDrawPanel->hide();
    } else {
        m_pDrawPanel->show();
    }
}

} // end Viewer namespace
