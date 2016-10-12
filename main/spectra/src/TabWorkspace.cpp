#include "TabWorkspace.h"

#include "SpecTclInterface.h"
#include "MultiSpectrumView.h"
#include "ViewDrawPanel.h"
#include "ControlPanel.h"
#include "GeometrySelector.h"
#include "MultiInfoPanel.h"
#include "AutoUpdater.h"

#include <QSplitter>
#include <QToolBar>

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

TabWorkspace::~TabWorkspace()
{
//    std::cout << "~TabWorkpace()" << std::endl;
}

SpectrumView& TabWorkspace::getView() {
    return *m_pView;
}

ControlPanel& TabWorkspace::getControlPanel() {
    return *m_pControls;
}

void TabWorkspace::setSpecTclInterface(std::shared_ptr<SpecTclInterface> pSpecTcl)
{
    // connect up the view to the new spectcl
    disconnect(m_pSpecTcl.get(), SIGNAL(histogramContentUpdated(HistogramBundle*)),
            m_pView, SLOT(updateView(HistogramBundle*)));

    disconnect(m_pSpecTcl.get(), SIGNAL(gateListChanged()),
            m_pView, SLOT(redrawView()));


    m_pSpecTcl = pSpecTcl;

    m_pControls->setSpecTclInterface(pSpecTcl);
    m_pDrawPanel->setSpecTclInterface(pSpecTcl);
    m_pInfoPanel->setSpecTclInterface(pSpecTcl);

    // connect up the view to the new spectcl
    connect(m_pSpecTcl.get(), SIGNAL(histogramContentUpdated(HistogramBundle*)),
            m_pView, SLOT(updateView(HistogramBundle*)));

    connect(m_pSpecTcl.get(), SIGNAL(gateListChanged()),
            m_pView, SLOT(redrawView()));

}

void TabWorkspace::setUpUI()
{
    QSplitter* pVSplitter = new QSplitter(this);
    pVSplitter->setOrientation(Qt::Vertical);

    QSplitter* pHSplitter = new QSplitter(this);
    pHSplitter->setOrientation(Qt::Horizontal);

    m_pDrawPanel = new ViewDrawPanel(m_pSpecTcl, this);
    m_pView      = new MultiSpectrumView(m_pSpecTcl, this);
    m_pInfoPanel = new MultiInfoPanel(*this, m_pSpecTcl, this);
    m_pControls  = new ControlPanel(m_pSpecTcl, m_pView, this);

    m_pToolBar = new QToolBar(this);
    m_pLogxAction = m_pToolBar->addAction(QIcon(":/icons/logx-icon.png"),"toggle logx");
    m_pLogxAction->setCheckable(true);
    m_pLogyAction = m_pToolBar->addAction(QIcon(":/icons/logy-icon.png"),"toggle logy");
    m_pLogyAction->setCheckable(true);
    m_pLogzAction = m_pToolBar->addAction(QIcon(":/icons/logz-icon.png"),"toggle logz");
    m_pLogzAction->setCheckable(true);

    m_pToolBar->addSeparator();
    m_pUnzoomXAction = m_pToolBar->addAction(QIcon(":/icons/unzoom-icon.png"),"zoom out x");
    m_pZeroXAction = m_pToolBar->addAction(QIcon(":/icons/center_x-icon.png"),"reset x");
    m_pZoomXAction = m_pToolBar->addAction(QIcon(":/icons/zoom-icon.png"),"zoom in x");

    m_pToolBar->addSeparator();
    m_pUnzoomYAction = m_pToolBar->addAction(QIcon(":/icons/unzoom-icon.png"),"zoom out y");
    m_pZeroYAction = m_pToolBar->addAction(QIcon(":/icons/center_y-icon.png"),"reset y");
    m_pZoomYAction = m_pToolBar->addAction(QIcon(":/icons/zoom-icon.png"),"zoom in y");


    pVSplitter->addWidget(m_pToolBar);
    pVSplitter->addWidget(m_pView);
    pVSplitter->addWidget(m_pControls);

    // adjust the sizes of the
    QSize viewSize    = m_pView->sizeHint();
    QSize controlSize = m_pView->sizeHint();
    pVSplitter->setSizes({ 10, viewSize.height()*100, 10 });

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
    connect(m_pDrawPanel, SIGNAL(histSelected(HistogramBundle*, QString)),
            m_pView, SLOT(drawHistogram(HistogramBundle*, QString)));

    connect(m_pDrawPanel, SIGNAL(histSelected(HistogramBundle*,QString)),
            this, SLOT(onHistogramSelected(HistogramBundle*,QString)));

    connect(m_pSpecTcl.get(), SIGNAL(histogramContentUpdated(HistogramBundle*)),
            m_pView, SLOT(updateView(HistogramBundle*)));

    connect(m_pSpecTcl.get(), SIGNAL(gateListChanged()),
            m_pView, SLOT(redrawView()));

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

    connect(m_pLogxAction,SIGNAL(triggered()), m_pView, SLOT(toggleLogx()));
    connect(m_pLogyAction,SIGNAL(triggered()), m_pView, SLOT(toggleLogy()));
    connect(m_pLogzAction,SIGNAL(triggered()), m_pView, SLOT(toggleLogz()));

    connect(m_pZoomXAction, SIGNAL(triggered()), m_pView, SLOT(zoomX()));
    connect(m_pZeroXAction, SIGNAL(triggered()), m_pView, SLOT(zeroX()));
    connect(m_pUnzoomXAction, SIGNAL(triggered()), m_pView, SLOT(unzoomX()));

    connect(m_pZoomYAction, SIGNAL(triggered()), m_pView, SLOT(zoomY()));
    connect(m_pZeroYAction, SIGNAL(triggered()), m_pView, SLOT(zeroY()));
    connect(m_pUnzoomYAction, SIGNAL(triggered()), m_pView, SLOT(unzoomY()));

}


void TabWorkspace::onHistogramSelected(HistogramBundle *pBundle, QString name)
{
    if (pBundle->getHist().InheritsFrom(TH2::Class())) {
        m_pZoomYAction->setEnabled(true);
        m_pZeroYAction->setEnabled(true);
        m_pUnzoomYAction->setEnabled(true);
        m_pLogzAction->setEnabled(true);
    } else {
        m_pZoomYAction->setEnabled(false);
        m_pZeroYAction->setEnabled(false);
        m_pUnzoomYAction->setEnabled(false);
        m_pLogzAction->setEnabled(false);
    }
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
    m_pView->update();

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
