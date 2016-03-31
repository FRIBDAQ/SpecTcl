#ifndef GATEMANAGERWIDGET_H
#define GATEMANAGERWIDGET_H

#include <QWidget>
#include <QString>

#include <memory>
#include <utility>
#include <map>

class QHBoxLayout;



namespace Viewer
{

class QRootCanvas;
class GateManager;
class OneDimGateEdit;
class SpectrumView;
class SpecTclInterface;
class HistogramBundle;
class GGate;
class GSlice;

class GateManagerWidget : public QWidget
{
    Q_OBJECT
public:
    explicit GateManagerWidget(SpectrumView& rView,
                               std::shared_ptr<SpecTclInterface> pSpecTcl,
                               QWidget *parent = 0);


    void setGateList(const std::map<QString, GSlice*>& gateMap);
    void setGateList(const std::map<QString, GGate*>& gateMap);

    void setHistogramDimension(int dim) { m_histDim = dim; }
    int getHistogramDimension() const { return m_histDim; }

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
    int                 m_histDim;
};

} // end Viewer namespace

#endif // GATEMANAGERWIDGET_H
