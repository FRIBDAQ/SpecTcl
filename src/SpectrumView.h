#ifndef SPECTRUMVIEW_H
#define SPECTRUMVIEW_H

#include <QWidget>

#include <vector>

class TH1;

class QMouseEvent;

namespace Viewer
{

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
     * \brief onGeometryChanged
     * \param row  number of rows to establish
     * \param col  number of columns to establish
     *
     * Sets the panes to a new grid geometry with a specific number of
     * rows and columns.
     */
    virtual void onGeometryChanged(int row, int col) = 0;

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
    virtual void update(HistogramBundle* pHist) = 0;

    /*!
     * \brief update
     *
     * This is the standard QWidget::update method.
     */
    virtual void update() { QWidget::update(); }

    /*!
     * \brief getAllHists in a pane
     * \param pCanvas - canvas containing histograms
     * \return a vector of histograms
     *
     * A utility method to retrieve the whole set of histograms in a
     * given pane.
     */
    static std::vector<TH1*> getAllHists(QRootCanvas* pCanvas);
};

} // end of namespace

#endif // SPECTRUMVIEW_H
