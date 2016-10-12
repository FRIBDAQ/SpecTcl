#ifndef TABWORKSPACE_H
#define TABWORKSPACE_H

#include <QWidget>
#include <QStringList>

#include <memory>

class QToolBar;
class QAction;
class TH1;

namespace Viewer
{

class SpectrumView;
class ViewDrawPanel;
class ControlPanel;
class SpecTclInterface;
class MultiInfoPanel;
class AutoUpdater;
class HistogramBundle;
class QRootCanvas;

class TabWorkspace : public QWidget
{
    Q_OBJECT

public:
    explicit TabWorkspace(std::shared_ptr<SpecTclInterface> pSpecTcl, QWidget *parent = 0);
    
    ~TabWorkspace();

    void layoutSpectra(QStringList spectrumList);

    AutoUpdater& getUpdater();
    SpectrumView& getView();
    ViewDrawPanel& getDrawPanel();
    ControlPanel& getControlPanel();

    void setSpecTclInterface(std::shared_ptr<SpecTclInterface> pSpecTcl);

signals:
    
public slots:
    void showHideStatistics();
    void showHideDrawPanel();
    void onHistogramSelected(HistogramBundle* pBundle, QString name);
    void onCurrentCanvasChanged(QRootCanvas& canvas);

private:
    void setUpUI();
    void connectSignals();
    void configureToolBarForHistogram(TH1& hist);

private:
    std::shared_ptr<SpecTclInterface>   m_pSpecTcl;
    SpectrumView*                       m_pView;
    ViewDrawPanel*                      m_pDrawPanel;
    ControlPanel*                       m_pControls;
    MultiInfoPanel*                     m_pInfoPanel;

    AutoUpdater*                        m_pAutoUpdater;
    QToolBar*                           m_pToolBar;
    QAction*                            m_pLogxAction;
    QAction*                            m_pLogyAction;
    QAction*                            m_pLogzAction;

    QAction*                            m_pZoomXAction;
    QAction*                            m_pZeroXAction;
    QAction*                            m_pUnzoomXAction;

    QAction*                            m_pZoomYAction;
    QAction*                            m_pZeroYAction;
    QAction*                            m_pUnzoomYAction;

};

} // end Viewer namespace

#endif // TABWORKSPACE_H
