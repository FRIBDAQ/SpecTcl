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

#ifndef GATEBUILDER1DDIALOG_H
#define GATEBUILDER1DDIALOG_H

#include "GSlice.h"
#include <QDialog>

#include <utility>

class TPad;

class QLineEdit;
class QValidator;


namespace Ui {
class GateBuilder1DDialog;
}


namespace Viewer
{

class QRootCanvas;
class HistogramBundle;


/*! Dialog for editing 1D gates (aka slices)
 *
 * This actually operates almost identically to the GateBuilderDialog
 * except that it maintains a GSlice rather than a GGate. See the 
 * GateBuilderDialog docs for the working of this.
 *
 * The accept() and reject() methods decorate the standard QDialog::accept() and 
 * QDialog::reject() methods.
 */
class GateBuilder1DDialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit GateBuilder1DDialog(QRootCanvas& canvas,
                                 HistogramBundle& hist,
                                 GSlice* pSlice = nullptr,
                                 QWidget *parent = nullptr);
    ~GateBuilder1DDialog();
    
signals:
    /*! Emitted when a user accepts changes 
     *
     * \param pSlice  pointer to slice that will be kept by DockableGateManager
     */
    void completed(GSlice* pSlice);

public slots:
    virtual void accept();
    virtual void reject();
    virtual void onMousePress(QRootCanvas* pad);
    virtual void onMouseRelease(QRootCanvas* pad);
    virtual void onClick(QRootCanvas* pad);

    /*! Set the name and also update the accept button */
    void onNameChanged(QString name);
    void lowEditChanged();
    void highEditChanged();
    void onLowChanged(double x1, double y1, double x2, double y2);
    void onHighChanged(double x1, double y1, double x2, double y2);

private:

    // Update lines and text
    void updateLow(double x);
    void updateHigh(double x);

    // Update text edits
    void updateLowEdit(double x);
    void updateHighEdit(double x);

    // utility to determine if user is focused on low entry
    bool focusIsLow();

    // Hides the original lines
    void removeOldLines(GSlice& pSlice);

    void connectSignals();

private:
    Ui::GateBuilder1DDialog *ui;
    QRootCanvas& m_canvas;
    HistogramBundle& m_histPkg;
    GSlice m_editSlice;
    GSlice* m_pOldSlice;
    QLineEdit* m_editFocus;
    QValidator* m_editValidator;
    std::pair<int, int> m_lastMousePressPos;
};

} // end of namespace

#endif // GATEBUILDER1DDIALOG_H

