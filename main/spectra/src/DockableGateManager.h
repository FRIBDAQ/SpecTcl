//    This software is Copyright by the Board of Trustees of Michigan
//    State University (c) Copyright 2016.
//
//    This program is free software: you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation, either version 3 of the License, or
//    any later version.
//
//    This program is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License
//    along with this program.  If not, see <http://www.gnu.org/licenses/>.
//
//    Authors:
//    Jeromy Tompkins 
//    NSCL
//    Michigan State University
//    East Lansing, MI 48824-1321

#ifndef DOCKABLEGATEMANAGER_H
#define DOCKABLEGATEMANAGER_H

#include <QDockWidget>
#include <vector>
#include <map>
#include <memory>

namespace SpJs {
  class GateInfo;
}

class QListWidgetItem;


namespace Ui {
class DockableGateManager;
}

namespace Viewer
{

class SpectrumView;
class SpecTclInterface;
class GGate;
class GSlice;
class MasterGateList;


/*! \brief Graphical object that owns all gates 
 *
 * THis is a dockable widget. It provides buttons for the user
 * to select a gate to edit and also delete. It then also enables
 * the user to create new gates for the histogram in focus.
 *
 */
class DockableGateManager : public QDockWidget
{
    Q_OBJECT
    
public:
    /*! Constructor
     *  \param viewer     the histogram view manager
     *  \param pSpecTcl   the interface object for communicating with SpecTcl
     *  \param parent     parent widget
     */
    explicit DockableGateManager(SpectrumView& viewer,
                                 std::shared_ptr<SpecTclInterface> pSpecTcl,
                                 QWidget *parent = 0);

    /*! Destructor */
    virtual ~DockableGateManager();

    void setSpecTclInterface(std::shared_ptr<SpecTclInterface> pSpecTcl);

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
    Ui::DockableGateManager*  ui;
    SpectrumView&             m_view;
    std::shared_ptr<SpecTclInterface>         m_pSpecTcl;
    std::map<QString, int>    m_gateRowMap;
};

} // end of namespace

#endif // DOCKABLEGATEBUILDER_H
