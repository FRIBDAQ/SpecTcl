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

#ifndef GATEBUILDERDIALOG_H
#define GATEBUILDERDIALOG_H

#include "HistogramBundle.h"

#include <QDialog>
#include <QString>
#include <QButtonGroup>

#include <TCutG.h>

#include <memory>
#include <vector>
#include <utility>

class TPad;

namespace Ui {
class GateBuilderDialog;
}


namespace Viewer
{

class GGate;
class QRootCanvas;

/*! Dialog for editing 2d gates
 *
 *
 * This maintains 1 GGate object that it owns for editing. While
 * the user edits, they are actually interacting with the local 
 * edit version of the gate. In the beginning, the user may or may 
 * not provide a cut to edit. If they prvided a cut for editing, the 
 * state of that cut is copied into the editable version. Only if the
 * the user accepts the changes they make to the cut will the changes
 * made to the editable cut be committed to the orginal cut. If the user
 * rejects the changes, then the changes are not committed back to it.
 *
 *
 */ 
class GateBuilderDialog : public QDialog
{
    Q_OBJECT
    
public:
    /*! Constructor
     *
     * Creates the ui.
     * Creates an editable cut
     * If provided a cut, hides original and draws editable
     * Initializes dialog based on cut
     * Connects the signals and slots for dialog.
     * 
     * \param viewer    the current canvas in focus
     * \param hist      the histogram in we are editing a gate for
     * \param pCut      cut to edit (if it exists)
     * \param parent    owner of this dialog widget
     */
    explicit GateBuilderDialog(QRootCanvas& viewer,
                               HistogramBundle& hist,
                               GGate* pCut = nullptr,
                               QWidget *parent = nullptr);

    /*! Destrcutor */
    virtual ~GateBuilderDialog();

    /*! Update name of the cut */
    void setCutName(const QString& name);

    ////////// SLOTS ///////////////////
public slots:
    /*! Commit edits to cut for returning
     *
     */
    virtual void accept();

    /*!  If needed, redraws original cut */
    virtual void reject();

    virtual void onMousePress(QWidget* pad);
    virtual void onMouseRelease(QWidget* pad);

    /*! Slot for receiving click events */
    void gateMoved(QWidget* pad);

    /*! Checks for state of text and update accept button state */
    void onNameChanged(const QString& name);

    /*! Slot for gate type radio buttons */
    void onTypeChanged(int type);

    void onValuesChanged(std::vector<std::pair<double, double> > points);
    void valueChanged(int row, int col);

signals:
    /*! Signal emitted when changes accepted */
    void completed(GGate* pCut);

private:
    void newPoint(QRootCanvas* fCanvas);
    void clearTable();
    void hideOldCut(GGate& gate);
    void appendPointToTable(double x, double y);
    void appendPointToCut(double x, double y);
    void fillTableWithData(GGate& rCut);
    void fillTableWithData(const std::vector<std::pair<double, double> >& points);

    // utility methods for handling whether cut is closed or open
    void ensureLastPointDiffersFromFirst();
    void ensureLastPointMatchesFirst();



private:
    Ui::GateBuilderDialog *ui;          //!< the ui
    QRootCanvas& m_canvas;              
    HistogramBundle& m_histPkg;
    QString m_name;
    std::unique_ptr<GGate> m_pEditCut;  //!< editiable cut
    GGate* m_pOldCut;                   //!< "original" cut
    QButtonGroup m_radioButtons;
    bool m_matchLast;
    bool m_isMoveEvent;

    std::pair<int, int> m_lastMousePressPos;
};

} // end of namespace

#endif // GATEBUILDERDIALOG_H
