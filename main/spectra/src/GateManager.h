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

class QListWidgetItem;

namespace Viewer
{

class SpectrumView;
class SpecTclInterface;
class GGate;
class GSlice;
class MasterGateList;


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
                                 std::shared_ptr<SpecTclInterface> pSpecTcl,
                                 QWidget *parent = 0);

    /*! Destructor */
    virtual ~GateManager();

    void setSpecTclInterface(std::shared_ptr<SpecTclInterface> pSpecTcl);
    QList<QListWidgetItem*> getSelectedItems() const;

signals:
    void addGateClicked();
    void editGateClicked();
    void deleteGateClicked();


    //////// SLOTS /////////////
public slots:
    /*! \brief Slot for opening a gate builder dialog */
    void launchAddGateDialog();

    /*! \brief Slot for opening a gate builder dialog for editing */
    void launchEditGateDialog();

    /*! \brief Slot for adding gate to list of managed gates
     *
     * This wraps pCut in a GateListItem that will own it and that
     * GateListItem is then owned by this.
     */
    void registerGate(GGate* pCut);

    /*! \brief Slot for adding slice to list of manage gates
     *
     * This wraps pSlice in a GateListItem that will own it and that
     * SliceListItem is then owned by this.
     */
    void registerSlice(GSlice* pSlice);

    /*! Makes a call to SpecTcl interface to edit the gate */
    void editGate(GGate* pCut);
    void editSlice(GSlice* pSlice);

    void deleteGate();

    /*! Update to list */
    void onGateListChanged();

    void setGateList(const std::vector<QString>& gates);

public:
    QListWidgetItem* findItem(const QString& name);
    void removeGate(QListWidgetItem* pItem);
    void clearList();
    std::vector<QListWidgetItem*> getItems() const;

private:
    void addSliceToList(GSlice* pSlice);
    void addGateToList(GGate* pGate);
    void connectSignals();

    void populateListWithoutSync();
    void populateListWithSync();

private:
    Ui::GateManager*                          ui;
    SpectrumView&                             m_view;
    std::shared_ptr<SpecTclInterface>         m_pSpecTcl;
    std::map<QString, int>                   m_gateRowMap;

};

} // end Viewer namespace
#endif // GATEMANAGER_H
