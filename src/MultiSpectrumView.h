#ifndef MULTISPECTRUMVIEW_H
#define MULTISPECTRUMVIEW_H

#include "SpectrumView.h"

#include <QWidget>
#include <QGridLayout>

#include <map>
#include <memory>
#include <vector>
#include <utility>

class QMouseEvent;

namespace Viewer
{

class HistogramBundle;
class SpecTclInterface;


/*!
 * \brief The MultiSpectrumView class
 *
 * An implementation of the SpectrumView interface that solves the
 * problem of multiple panes using a QGridLayout and multiple
 * canvases (i.e. QRootCanvas).
 *
 * This is responsible for managing the geometry of the layout and
 * also updating the canvases that are displayed.
 */
class MultiSpectrumView : public SpectrumView
{
    Q_OBJECT
public:
    explicit MultiSpectrumView(SpecTclInterface* pSpecTcl, QWidget *parent = 0);

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
     * \brief getCurrentCanvas
     * \return the canvas with focus
     */
    QRootCanvas* getCurrentCanvas();

    void paintEvent(QPaintEvent *evt);

    void keyPressEvent(QKeyEvent *);

public slots:
    /*!
     * \brief onGeometryChanged
     * \param row
     * \param cols
     *
     * Set the geometry to be a grid of dimensions row x cols.
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
    void update(HistogramBundle* pHist);

    /*!
     * \brief refreshAll
     *
     * Update the histograms on all canvases in the grid. This does not communicate with
     * SpecTcl, it merely synchronizes what is displayed with the local state of
     * the histograms.
     */
    void refreshAll();

    std::pair<int,int> findLocation(QWidget* pWidget);

private:
    std::unique_ptr<QGridLayout> m_pLayout;
    std::multimap<HistogramBundle*, QRootCanvas*> m_histMap;
    SpecTclInterface *m_pSpecTcl;
//    std::vector<std::unique_ptr<QRootCanvas> > m_canvases;
    QRootCanvas* m_pCurrentCanvas;
    int m_currentNRows;
    int m_currentNColumns;

};

} // end of namespace

#endif // MULTISPECTRUMVIEW_H
