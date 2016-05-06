#include "TabWorkspace.h"

#include "SpecTclInterface.h"
#include "MultiSpectrumView.h"
#include "ViewDrawPanel.h"
#include "ControlPanel.h"
#include "GeometrySelector.h"
#include "MultiInfoPanel.h"
#include "AutoUpdater.h"

#include <QSplitter>

#include <stdexcept>

namespace Viewer
{

TabWorkspace::TabWorkspace(std::shared_ptr<SpecTclInterface> pSpecTcl, QWidget *parent) :
    QWidget(parent),
    m_pSpecTcl(pSpecTcl),
    m_pView(nullptr),
    m_pDrawPanel(nullptr),
    m_pControls(nullptr),
    m_pInfoPanel(nullptr),
    m_pAutoUpdater(nullptr)

{

    setUpUI();

    m_pAutoUpdater = new AutoUpdater(pSpecTcl, *m_pView, this);

    // populate the histogram draw panel with histograms
    m_pDrawPanel->setHistogramList(m_pSpecTcl->getHistogramList());

    connectSignals();

}

SpectrumView& TabWorkspace::getView() {
    return *m_pView;
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
    pVSplitter->setSizes({ viewSize.height()*100, 10 });

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
            m_pView, SLOT(setGeometry(int, int)));

    connect(m_pView, SIGNAL(currentCanvasChanged(QRootCanvas&)),
            m_pInfoPanel, SLOT(currentCanvasChanged(QRootCanvas&)));

    connect(m_pView, SIGNAL(canvasContentChanged(QRootCanvas&)),
            m_pInfoPanel, SLOT(currentCanvasChanged(QRootCanvas&)));

    connect(m_pView, SIGNAL(canvasUpdated(QRootCanvas&)),
            m_pInfoPanel, SLOT(updateContent(QRootCanvas&)));

    connect(m_pControls, SIGNAL(statisticsButtonClicked()),
            this, SLOT(showHideStatistics()));

    connect(m_pControls, SIGNAL(displayButtonClicked()),
            this, SLOT(showHideDrawPanel()));

    connect(m_pControls, SIGNAL(zoomButtonClicked()), m_pView, SLOT(toggleZoom()));
    connect(m_pControls, SIGNAL(zoomButtonClicked()), m_pDrawPanel, SLOT(toggleZoom()));
    connect(m_pView, SIGNAL(zoomChanged(bool)), m_pControls, SLOT(setZoomed(bool)));
    connect(m_pView, SIGNAL(zoomChanged(bool)), m_pDrawPanel, SLOT(setZoomedState(bool)));

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
        QRootCanvas* pCanvas = m_pView->getCurrentCanvas();
        if (pCanvas) {
            if (m_pInfoPanel->getNumberOfTabs()==0) {
                m_pInfoPanel->currentCanvasChanged(*pCanvas);
            } else {
                m_pInfoPanel->updateContent(*pCanvas);
            }

        }
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

AutoUpdater& TabWorkspace::getUpdater()
{
    if (m_pAutoUpdater == nullptr) {
        throw std::runtime_error("TabWorkspace::getUpdater() Auto updater does not exist! Can't return it");
    }

    return *m_pAutoUpdater;
}

ViewDrawPanel& TabWorkspace::getDrawPanel()
{
    if (m_pDrawPanel == nullptr) {
        throw std::runtime_error("TabWorkspace::getDrawPanel() ViewDrawPanel does not exist! Cannot return it!");
    }
    return *m_pDrawPanel;
}

} // end Viewer namespace
