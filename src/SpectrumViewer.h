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

#ifndef SPECTRUMVIEWER_H
#define SPECTRUMVIEWER_H

#include "ContentRequestHandler.h"
#include "SpectrumView.h"
#include <QPair>

#include <set>

class QString;

namespace Ui {
class SpectrumViewer;
}


namespace Viewer
{

class HistogramBundle;
class QRootCanvas;
class SpecTclInterface;


/*!
 * \brief The SpectrumViewer class
 *
 * An implementation of the SpectrumView interface that uses the subpad mechanism
 * native to a TCanvas (i.e. QRootCanvas) for establishing a grid of panes. This also
 * contains a tabbed widget.
 *
 * It is merely responsible for managing a single canvas. That is dividing it into
 * subpads and updating it.
 *
 */
class SpectrumViewer : public SpectrumView
{
    Q_OBJECT
    
public:
    explicit SpectrumViewer(SpecTclInterface* pSpecTcl, QWidget *parent = 0);
    ~SpectrumViewer();

  /*!
     * \brief getCurrentCanvas
     * \return the canvas this manages
     */
    QRootCanvas* getCurrentCanvas();

    /*!
     * \brief setCurrentCanvas
     * \param pCanvas - not used
     *
     * This is not used because there is one and only one canvas managed by this
     * class.
     */
    void setCurrentCanvas(QRootCanvas *pCanvas) {};

    /*!
     * \brief getRowCount
     * \return number of rows the canvas has been divided into
     */
    int getRowCount() const { return m_currentNRows; }

    /*!
     * \brief getColumnCount
     * \return number of columns the canvas has been divided into
     */
    int getColumnCount() const { return m_currentNColumns; }

public slots:

    /*!
     * \brief onGeometryChanged
     * \param nRows
     * \param nColumns
     *
     * Divides the canvas into subpads so into a geometry of nRows x nColumns.
     */
    void onGeometryChanged(int nRows, int nColumns);

    /*!
     * \brief onHistogramRemoved
     * \param pHistBundle address of a hist that was recently deleted
     *
     * DO NOT DEREFERENCE the argument! This is to compare with canvases
     * for the purpose of seeing if they reference that histogram that USED
     * to live at the address.
     */
    void onHistogramRemoved(HistogramBundle* pHistBundle);

    /*!
     * \brief update
     * \param gHist - histogram to draw
     *
     * Draw the histogrma in the current pad
     */
    void update(HistogramBundle* gHist);

    /*!
     * \brief Redraw all subpads
     *
     */
    void refreshAll();

    void onError(int errorCode, const QString& reason);

private:
    Ui::SpectrumViewer*   ui;
    HistogramBundle*      m_currentHist;
    QRootCanvas*          m_canvas;
    QRootCanvas*          m_currentCanvas;
    SpecTclInterface*     m_pSpecTcl;
    std::set<QRootCanvas*> m_canvasList;
    int m_currentNRows = 1;
    int m_currentNColumns = 1;
};

} // end of namespace

#endif // SPECTRUMVIEWER_H
