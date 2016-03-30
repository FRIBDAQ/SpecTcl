#ifndef GATEMANAGERWIDGET_H
#define GATEMANAGERWIDGET_H

#include <QWidget>

#include <memory>
#include <utility>

class QHBoxLayout;


namespace Viewer
{

class QRootCanvas;
class GateManager;
class OneDimGateEdit;
class SpectrumView;
class SpecTclInterface;
class HistogramBundle;

class GateManagerWidget : public QWidget
{
    Q_OBJECT
public:
    explicit GateManagerWidget(SpectrumView& rView,
                               std::shared_ptr<SpecTclInterface> pSpecTcl,
                               QWidget *parent = 0);

public slots:
    void onEditPressed();
    void onAddPressed();
    void onDeletePressed();

    void closeDialog();

private:
    std::pair<QRootCanvas*, HistogramBundle*> setUpDialog();
    void addGate(QRootCanvas& rCanvas, HistogramBundle& rHistPkg);

private:
    SpectrumView&       m_view;
    GateManager*        m_pManager;
    OneDimGateEdit*     m_p1DGateEdit;
    QHBoxLayout*        horizontalLayout;
    std::shared_ptr<SpecTclInterface> m_pSpecTcl;
};

} // end Viewer namespace

#endif // GATEMANAGERWIDGET_H
