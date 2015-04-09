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

class DockableGateManager : public QDockWidget
{

    Q_OBJECT
    
public:
    explicit DockableGateManager(const SpectrumViewer& viewer,
                                 SpecTclInterface* pSpecTcl,
                                 QWidget *parent = 0);
    ~DockableGateManager();

public slots:
    void launchAddGateDialog();
    void launchEditGateDialog();
    void registerGate(GGate* pCut);
    void registerSlice(GSlice* pSlice);
    void editGate(GGate* pCut);
    void editSlice(GSlice* pSlice);

private:
    Ui::DockableGateManager *ui;
    const SpectrumViewer& m_view;
    SpecTclInterface* m_pSpecTcl;
};

#endif // DOCKABLEGATEBUILDER_H
