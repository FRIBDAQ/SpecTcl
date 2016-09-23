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

#ifndef VIEWDRAWPANEL_H
#define VIEWDRAWPANEL_H

#include "HistogramList.h"
#include "QHistInfo.h"
#include "ui_ViewDrawPanel.h"

#include <QWidget>
#include <QModelIndex>

#include <vector>
#include <memory>

class QListWidgetItem;
class TH1;
class QKeyEvent;

namespace Viewer
{

class SpecTclInterface;
class ListRequestHandler;
class GuardedHist;
class GeometrySelector;

/*! \brief A widget that contains the list of histograms
 *
 * The ViewDrawPanel provides the user a graphical interaction point
 * to view and select which histogram the user wants to draw. This only
 * maintains a reference to the histograms that are owned by the HistogramList.
 * This therefore has a synchronization mechanism to ensure that the displayed
 * names are what are available.
 *
 * This also implements the requisite method to be wrapped in a
 * GenericSpecTclInterfaceObserver.
 */
class ViewDrawPanel : public QWidget
{
    Q_OBJECT

public:

    explicit ViewDrawPanel(std::shared_ptr<SpecTclInterface> pSpecTcl,
                           QWidget *parent = 0);
    ~ViewDrawPanel();

    /*!
     * \brief Pass in a new SpecTclInterface
     *
     * \param pSpecTcl
     */
    void setSpecTclInterface(std::shared_ptr<SpecTclInterface> pSpecTcl);

    //////////////////////////////////////////////////////////////////////////

    /*!
     * \brief Synchronize the view to the master histogram list
     *
     * \param pHistList    pointer to the histogram list
     */
    void setHistogramList(HistogramList* pHistList);

    void removeStaleEntries(HistogramList *pHistList);

    void filterHistogramList(const QString& filter);

signals:
    void histSelected(HistogramBundle* hist, QString drawOption);
    void geometryChanged(int nRows, int nCols);

    //////////////////////////////////////////////////////////////////////////
public slots:
    /*!
     * \brief
     * \param index
     */
    void onDoubleClick(QModelIndex index);

    /*!
     * \brief Synchronize to the master histogram list
     */
    void onHistogramListChanged();

    void onRowCountChanged(int nRows);
    void onColumnCountChanged(int nColumns);

    GeometrySelector& getGeometrySelector() { return *m_pGeoSelector; }

    void applyFilter(const QString& pattern);

    void setZoomedState(bool state);

    void onSelectionChanged();

    //////////////////////////////////////////////////////////////////////////
private:
    void appendHistogramToList(HistogramList::iterator it);
    void setIcon(QListWidgetItem& item, HistogramList::iterator it);

    void clearHistogramList();
    void keyPressEvent(QKeyEvent* pEvent);
    void setDrawOptions(int dimension);

private:
    std::unique_ptr<Ui::ViewDrawPanel> ui;
    std::shared_ptr<SpecTclInterface> m_pSpecTcl;
    GeometrySelector*                 m_pGeoSelector;
    int                               m_currentDimension;
};

} // end of namespace

#endif // VIEWDRAWPANEL_H
