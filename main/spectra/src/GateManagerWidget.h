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
class ControlPanel;


/*!
 * \brief The GateManagerWidget class
 *
 * This class is a frame manager for the gate management shown at the bottom of the
 * information panel. The initial
 * frame that is displayed is the GateManager, which provides a list of integrals
 * for each of the known gates and buttons to add, edit, or delete them. If the user
 * chooses to add or edit the gate, then depending on the dimensionality of the gate,
 * the OneDimGateEdit or the TwoDimGateEdit widget will be displayed.
 */
class GateManagerWidget : public QWidget
{
    Q_OBJECT
public:
    explicit GateManagerWidget(SpectrumView& rView,
                               ControlPanel& rControls,
                               std::shared_ptr<SpecTclInterface> pSpecTcl,
                               const QString& hName,
                               QWidget *parent = 0);


    void setGateList(const std::map<QString, GSlice*>& gateMap);
    void setGateList(const std::map<QString, GGate*>& gateMap);

    void setHistogramDimension(int dim) { m_histDim = dim; }
    int getHistogramDimension() const { return m_histDim; }

    void updateGateIntegrals(HistogramBundle& rHistPkg);

public slots:
    void onEditPressed();
    void onAddPressed();
    void onDeletePressed();

    void closeDialog();

signals:
    void gateManagerActionComplete();

private:
    std::pair<QRootCanvas*, HistogramBundle*> setUpDialog();
    void addGate(QRootCanvas& rCanvas, HistogramBundle& rHistPkg);

private:
    SpectrumView&       m_view;
    ControlPanel&       m_controls;
    GateManager*        m_pManager;
    OneDimGateEdit*     m_p1DGateEdit;
    QHBoxLayout*        horizontalLayout;
    std::shared_ptr<SpecTclInterface> m_pSpecTcl;
    int                 m_histDim;
    QString             m_histName;
};

} // end Viewer namespace

#endif // GATEMANAGERWIDGET_H
