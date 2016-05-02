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


#ifndef SPECTRUMVIEW_H
#define SPECTRUMVIEW_H

#include <QWidget>
#include <QStringList>

#include <vector>
#include <memory>
#include <tuple>

class TH1;

class QMouseEvent;

namespace Viewer
{

class SpecTclInterface;
class QRootCanvas;
class HistogramBundle;


/*!
 * \brief The SpectrumView class
 *
 * An abstract base class for a generic multipaned viewer. There need not be more
 * than one pane, but this is a minimum set of methods to support dealing with
 * more than on pane. In this documentation, the term pane is to refer to a segment of
 * the grid of drawable entities. This may be a subpad in a canvas  or a top-level
 * canvas.
 *
 */
class SpectrumView : public QWidget
{
    Q_OBJECT

public:
    SpectrumView(QWidget* parent) : QWidget(parent) {}

    /*!
     * \brief mousePressEvent
     * \param pEvent  the event
     *
     *  Ensure that derived classes inherit a virtual mousePressEvent
     *
     */
    virtual void mousePressEvent(QMouseEvent *pEvent) { QWidget::mousePressEvent(pEvent);}

    /*!
     * \brief getRowCount
     * \return number of rows of panes
     */
    virtual int getRowCount() const = 0;

    /*!
     * \brief getColumnCount
     * \return number of columns of panes
     */
    virtual int getColumnCount() const = 0;

    /*!
     * \brief getCurrentCanvas
     * \return the canvas currently focused
     */
    virtual QRootCanvas* getCurrentCanvas() = 0;

    /*!
     * \brief Retrieve a specific canvas
     *
     * \param row   row containing canvas
     * \param col   column containing canvas
     * \return QRootCanvas*
     * \retval nullptr if canvas does not exist
     * \retval pointer to canvas if it exists.
     */
    virtual QRootCanvas* getCanvas(int row, int col) = 0;

    virtual std::vector<QRootCanvas*> getAllCanvases() = 0;
    /*!
     * \brief onGeometryChanged
     * \param row  number of rows to establish
     * \param col  number of columns to establish
     *
     * Sets the panes to a new grid geometry with a specific number of
     * rows and columns.
     */
    virtual void setGeometry(int row, int col) = 0;

    /*!
     * \brief setCurrentCanvas
     * \param pCanvas canvas to take focus
     *
     * Useful for setting which pane has the focus
     */
    virtual void setCurrentCanvas(QWidget* pCanvas) = 0;

    /*!
     * \brief refreshAll
     *
     * Updates all of the panes visually without communicating with SpecTcl to
     * update the contents. The user should only expect a change if some previous
     * communication with SpecTcl caused an update to occur.
     *
     */
    virtual void refreshAll() = 0;

    /*!
     * \brief onHistogramRemoved
     * \param pBundle the histogram bundle that was removed (i.e. it has already been deleted)
     *
     * THOU SHALT NOT DEREFERENCE THIS POINTER PASSED IN. Rather it should be used to
     * see if any panes are displaying a histogram that is now defunct.
     */
    virtual void onHistogramRemoved(HistogramBundle* pBundle) = 0;

    /*!
     * \brief update
     * \param pHist - histogram to draw
     *
     * Draws the argument in the pane with focus.
     */
    virtual void updateView(HistogramBundle* pHist) = 0;

    /*!
     * \brief Draw histogram in pad
     * \param pHist - histogram to draw
     *
     * Draws the argument in the pane with focus.
     */
    virtual void drawHistogram(HistogramBundle* pHist) = 0;

    /*!
     * \brief update
     *
     * This is the standard QWidget::update method.
     */
    virtual void update() { QWidget::update(); }

    /*!
     * \brief setSpecTclInterface
     *
     * All derived classes will probably depend on a SpecTclInterface.
     * This method essentially sets the expectation that any derived class
     * might be passed an interface and deal with it appropriately.
     *
     * \param pSpecTcl
     */
    virtual void setSpecTclInterface(std::shared_ptr<SpecTclInterface> pSpecTcl) = 0;

    virtual void clearLayout() = 0;
    virtual void layoutSpectra(QStringList spectrumList) = 0;

    virtual void toggleZoom() = 0;

    virtual void ignoreUpdates(bool state) = 0;
    virtual bool isIgnoringUpdates() const = 0;

    /*!
     * \brief getAllHists in a pane
     * \param pCanvas - canvas containing histograms
     * \return a vector of histograms
     *
     * A utility method to retrieve the whole set of histograms in a
     * given pane.
     */
    static std::vector<TH1*> getAllHists(QRootCanvas* pCanvas);

    virtual std::tuple<int, int> computeOptimalGeometry(int nCanvases) = 0;

signals:
    void currentCanvasChanged(QRootCanvas& rCanvas);
    void canvasContentChanged(QRootCanvas& rCanvas);
    void canvasUpdated(QRootCanvas& rCanvas);
    void zoomChanged(bool zoomState);
};

} // end of namespace

#endif // SPECTRUMVIEW_H
