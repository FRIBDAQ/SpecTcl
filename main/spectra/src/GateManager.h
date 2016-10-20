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
 * show an appropriate edit widget. This also handles the logic of removing gates
 * if the "Remove" button is pressed.
 *
 * The GUI is mostly assembled by the GateManager.ui file created in qtdesigner.
 *
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
