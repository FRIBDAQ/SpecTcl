#ifndef GATEMANAGER_H
#define GATEMANAGER_H

#include <QWidget>
#include <QList>
#include <QString>

#include <vector>
#include <map>
#include <memory>

namespace Ui {
class GateManager;
}

namespace SpJs {
    class GateInfo;
}

class QTableWidgetItem;

namespace Viewer
{

class SpectrumView;
class SpecTclInterface;
class GGate;
class GSlice;
class MasterGateList;
class HistogramBundle;
class ControlPanel;


/*!
 * \brief The GateManager class
 *
 * The gate manager class is the widget that is is initially displayed by
 * the GateManagerWidget. The GateManager contains a QTableView that shows
 * all of the gates associated with a canvas and also their integral. It also
 * has three buttons "Add","Edit", and "Remove". These buttons just emit signals
 * that are handled by the GateManagerWidget that owns this so that it can
 * show the widget that makes sense. This also handles the logic of removing gates
 * if the "Remove" button is pressed.
 */
class GateManager : public QWidget
{
    Q_OBJECT

public:
    /*! Constructor
     *  \param viewer     the histogram view manager
     *  \param pSpecTcl   the interface object for communicating with SpecTcl
     *  \param parent     parent widget
     */
    explicit GateManager(SpectrumView& viewer,
                         ControlPanel& controls,
                                 std::shared_ptr<SpecTclInterface> pSpecTcl,
                                 int nDimensions,
                                 QWidget *parent = 0);

    /*! Destructor */
    virtual ~GateManager();

    void setSpecTclInterface(std::shared_ptr<SpecTclInterface> pSpecTcl);
    QList<QTableWidgetItem*> getSelectedItems() const;

    void updateGateIntegrals(HistogramBundle& rHistPkg);
    void update1DIntegrals(HistogramBundle& rHistPkg);
    void update2DIntegrals(HistogramBundle& rHistPkg);

signals:
    void addGateClicked();
    void editGateClicked();
    void deleteGateClicked();


    //////// SLOTS /////////////
public slots:
    /*! \brief Slot for opening a gate builder dialog */
    void onAddButtonClicked();

    /*! \brief Slot for opening a gate builder dialog for editing */
    void onEditButtonClicked();

    void onDeleteButtonClicked();
    void setGateList(std::vector<QString> gateNames);

private:
    void connectSignals();


private:
    Ui::GateManager*                          ui;
    SpectrumView&                             m_view;
    ControlPanel&                             m_controls;
    std::shared_ptr<SpecTclInterface>         m_pSpecTcl;
    int                                       m_nRows;
    int                                       m_histDim;

};

} // end Viewer namespace
#endif // GATEMANAGER_H
