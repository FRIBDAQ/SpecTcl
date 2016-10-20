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
 * the OneDimGateEdit or the TwoDimGateEdit widget will be displayed. Communication
 * between the manage widgets and this class is handled with the signal/slot mechanism.
 */
class GateManagerWidget : public QWidget
{
    Q_OBJECT

    // All source documentation is provided in the source file. Check out the
    // doxygen docs that can be generated as well. They are probably the best
    // user facing documentation.

public:
    explicit GateManagerWidget(SpectrumView& rView,
                               ControlPanel& rControls,
                               std::shared_ptr<SpecTclInterface> pSpecTcl,
                               const QString& hName,
                               QWidget *parent = 0);


    void setSpecTclInterface(std::shared_ptr<SpecTclInterface> pSpecTcl);

    void setGateList(const std::map<QString, GSlice*>& gateMap);
    void setGateList(const std::map<QString, GGate*>& gateMap);

    void setHistogramDimension(int dim) { m_histDim = dim; }
    int getHistogramDimension() const { return m_histDim; }

    void updateGateIntegrals(HistogramBundle& rHistPkg);

    void setUpGUI(std::shared_ptr<SpecTclInterface> pSpecTcl);
public slots:
    void onEditPressed();
    void onAddPressed();
    void onDeletePressed();

    void closeDialog();
    void updateGateList();

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
