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

#ifndef INFORMATIONPANEL_H
#define INFORMATIONPANEL_H

#include <memory>
#include <QWidget>

namespace Ui {
class InformationPanel;
}

namespace Viewer {

class HistogramBundle;
class GateManagerWidget;
class SpecTclInterface;
class TabWorkspace;
class SpectrumView;


/*!
 * \brief The InformationPanel class
 *
 * The information panel is the panel that is drawn on the right side of main window
 * containing information about the spectrum in the current view. An information panel
 * is only ever associated with a single spectrum in the canvas. It should be understand
 * to be the content of a tab in a MultiInfoPanel widget. The information panel shows:
 *
 *  - parameters associated with the spectrum
 *  - statistics (total integral, view integral, minimum, maximum)
 *  - gate manager widget for manipulating and viewing information about the gates
 *
 * The graphical layout is primarily defined in the InformationPanel.ui file created
 * by qtdesigner.
 */
class InformationPanel : public QWidget
{
    Q_OBJECT
    
    // docs are provided in the source file. also can be found in the generated doxygen
    // documentation

public:
    explicit InformationPanel(TabWorkspace& rView,
                              std::shared_ptr<SpecTclInterface> pSpecTcl,
                              const QString& histName,
                              QWidget *parent = 0);
    ~InformationPanel();
    
    void setSpecTclInterface(std::shared_ptr<SpecTclInterface> pSpecTcl);

    void clearParameterList();
    void updateParameterList(HistogramBundle &rHist);
    void updateStatistics(HistogramBundle& rHist);
    void updateGates(HistogramBundle& rHist);

public slots:
    void onHistogramChanged(HistogramBundle& rHist);


private:
    void setUpStatisticsTable();
private:
    Ui::InformationPanel *ui;

    GateManagerWidget* m_pGateManager;
    SpectrumView* m_pView;
};

} // end of Viewer namespace
#endif // INFORMATIONPANEL_H
