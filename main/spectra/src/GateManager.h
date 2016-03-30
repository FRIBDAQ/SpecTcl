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
    void onAddButtonClicked();

    /*! \brief Slot for opening a gate builder dialog for editing */
    void onEditButtonClicked();

    void onDeleteButtonClicked();
    void setGateList(const std::vector<QString>& gates);


private:
    void connectSignals();


private:
    Ui::GateManager*                          ui;
    SpectrumView&                             m_view;
    std::shared_ptr<SpecTclInterface>         m_pSpecTcl;

};

} // end Viewer namespace
#endif // GATEMANAGER_H
