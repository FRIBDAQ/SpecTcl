//    This software is Copyright by the Board of Trustees of Michigan
//    State University (c) Copyright 2015.
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

class SpectrumViewer;
class SpecTclInterface;
class GGate;
class GSlice;

namespace Ui {
class DockableGateManager;
}



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
    explicit DockableGateManager(const SpectrumViewer& viewer,
                                 SpecTclInterface* pSpecTcl,
                                 QWidget *parent = 0);

    /*! Destructor */
    virtual ~DockableGateManager();

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

private:
    Ui::DockableGateManager *ui;
    const SpectrumViewer& m_view;
    SpecTclInterface* m_pSpecTcl;
};

#endif // DOCKABLEGATEBUILDER_H
