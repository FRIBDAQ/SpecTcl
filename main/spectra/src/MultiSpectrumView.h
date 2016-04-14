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

#ifndef MULTISPECTRUMVIEW_H
#define MULTISPECTRUMVIEW_H

#include "SpectrumView.h"

#include <QWidget>
#include <QGridLayout>
#include <QStringList>

#include <map>
#include <memory>
#include <vector>
#include <utility>
#include <tuple>

class TPad;

class QMouseEvent;

namespace Viewer
{

class HistogramBundle;
class SpecTclInterface;
class StatusBar;

/*!
 * \brief The MultiSpectrumView class
 *
 * An implementation of the SpectrumView interface that solves the
 * problem of multiple panes using a QGridLayout and multiple
 * canvases (i.e. QRootCanvas).
 *
 * This is responsible for managing the geometry of the layout and
 * also updating the canvases that are displayed. The canvases that are added
 * to the grid layout are owned by this class.
 */
class MultiSpectrumView : public SpectrumView
{
    Q_OBJECT
public:
    explicit MultiSpectrumView(std::shared_ptr<SpecTclInterface> pSpecTcl,
                               QWidget *parent = 0);

    virtual ~MultiSpectrumView();
  /*!
     * \brief getRowCount
     * \return  number of visible rows
     *
     * This is different than what QGridLayout::rowCount() would return.
     */
    int getRowCount() const;

    /*!
     * \brief getColumnCount
     * \return  number of visible columns
     *
     * This is different that what QGridLayout::columnCount() would return.
     */
    int getColumnCount() const;

    /*!
     * \brief Retrieve a list of all the canvases that are visible
     *
     * \return list of visible canvases
     */
    std::vector<QRootCanvas*> getAllCanvases();

    /*!
     * \brief getCurrentCanvas
     * \return the canvas with focus
     */
    QRootCanvas* getCurrentCanvas();

    /*!
     * \brief paintEvent
     *
     * \param evt  paint event
     *
     * This is where the current window frame is drawn.
     */
    void paintEvent(QPaintEvent *evt);

    /*!
     * \brief keyPressEvent
     *
     * Handles the key bindings to change the current canvas using arrow
     * keys.
     */
    void keyPressEvent(QKeyEvent *);

    /*!
     * \brief mouseDoubleClickEvent
     *
     * \param evt
     *
     * This does not do anything at the moment but will eventually be used
     * to handle expanding the current window to be the full size of the window.
     */
    virtual void mouseDoubleClickEvent(QMouseEvent* evt);


    /*!
     * \brief setSpecTclInterface
     *
     * \param pSpecTcl  the new SpecTclInterface
     */
    void setSpecTclInterface(std::shared_ptr<SpecTclInterface> pSpecTcl);

    void toggleZoom(QWidget& pWidget);

    void clearLayout();

    void layoutSpectra(QStringList spectrumList);

    std::tuple<int, int> computeOptimalGeometry(int nSpectra);

public slots:
    /*!
     * \brief onGeometryChanged
     * \param row
     * \param cols
     *
     * Set the geometry to be a grid of dimensions row x cols. It handles
     * which widgets to get rid of and which to keep intelligently.
     */
    void onGeometryChanged(int row, int cols);

    /*!
     * \brief onHistogramRemoved
     * \param pHistBundle - address of deleted histogram bundle
     *
     * DO NOT DEREFERENCE the pointer! This is for updating canvases that were
     * displaying the histogram so that the histogram is removed properly.
     */
    void onHistogramRemoved(HistogramBundle* pHistBundle);

    /*!
     * \brief setCurrentCanvas
     * \param pCanvas canvas to take focus
     */
    void setCurrentCanvas(QWidget *pCanvas);

    /*!
     * \brief update
     * \param pHist - histogram bundle to draw
     *
     *  Draw the histogram bundle on the current canvas
     */
    void updateView(HistogramBundle* pHist);

    /*!
     * \brief drawHistogram
     *
     * \param pHist the histogram to draw
     *
     * This draws the histogram in the current canvas
     */
    void drawHistogram(HistogramBundle *pHist);
    /*!
     * \brief refreshAll
     *
     * Update the histograms on all canvases in the grid. This does not communicate with
     * SpecTcl, it merely synchronizes what is displayed with the local state of
     * the histograms.
     */
    void refreshAll();

    void toggleZoom();

    void onPadDoubleClick(TPad* pPad);

    void onCanvasUpdated();

private:
    /*!
     * \brief Find (row,col) of specific widget
     *
     * \param pWidget   the widget to find
     *
     * This searches the visible widgets to see if the widget exists.
     *
     * \return (row, col) if found
     * \retval (-1, -1) if widget not found
     */
    std::pair<int,int> findLocation(QWidget* pWidget);

    /*!
     * \brief Check whether a certain histogram is drawn in a canvas
     *
     * \param pHist
     *
     * \return boolean
     * \retval true     histogram is visible
     * \retval false    otherwise
     */
    bool histogramVisible(HistogramBundle* pHist);

    /*!
     * \brief Check whether a histogram lives in a canvas
     *
     * \param pHist     the histogram bundle
     * \param pCanvas   the canvas
     *
     * \return boolean
     * \retval true     histogram is drawn in canvas
     * \retval false    otherwise
     */
    bool histogramInCanvas(HistogramBundle* pHist, QRootCanvas *pCanvas);

private:
    QGridLayout*                                    m_pLayout;
    std::multimap<HistogramBundle*, QRootCanvas*>   m_histMap;
    std::shared_ptr<SpecTclInterface>               m_pSpecTcl;
    QRootCanvas                                    *m_pCurrentCanvas;
    int                                             m_currentNRows;
    int                                             m_currentNColumns;
    std::map<std::pair<int, int>, QWidget*>         m_canvases;
    bool                                            m_isZoomed;
    StatusBar*                                      m_pStatusBar;

}; // end MultiSpectrumView

} // end of namespace

#endif // MULTISPECTRUMVIEW_H
