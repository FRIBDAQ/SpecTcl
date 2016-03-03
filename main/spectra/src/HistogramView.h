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

#ifndef HISTOGRAMVIEW_H
#define HISTOGRAMVIEW_H

#include "HistogramList.h"
#include "QHistInfo.h"
#include "ui_HistogramView.h"

#include <QDockWidget>
#include <QModelIndex>

#include <vector>
#include <memory>

class QListWidgetItem;
class TH1;


namespace Viewer 
{

class SpecTclInterface;
class ListRequestHandler;
class GuardedHist;

/*! \brief A dockable widget that contains the list of histograms
 *
 * The HistogramView provides the user a graphical interaction point
 * to view and select which histogram the user wants to draw. This only
 * maintains a reference to the histograms that are owned by the HistogramList.
 * This therefore has a synchronization mechanism to ensure that the displayed
 * names are what are available.
 *
 * This also implements the requisite method to be wrapped in a
 * GenericSpecTclInterfaceObserver.
 */
class HistogramView : public QDockWidget
{
    Q_OBJECT
    
public:

    explicit HistogramView(std::shared_ptr<SpecTclInterface> pSpecTcl,
                           QWidget *parent = 0);
    ~HistogramView();

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
    void synchronize(HistogramList* pHistList);

    void appendEntry(const QString& name, HistogramList::iterator it);
    void updateEntry(HistogramList::iterator it, const QString& name);
    void removeStaleEntries(HistogramList *pHistList);
signals:
    void histSelected(HistogramBundle* hist);

    //////////////////////////////////////////////////////////////////////////
public slots:
    void setList(std::vector<SpJs::HistInfo> list);

    /*!
     * \brief
     * \param index
     */
    void onDoubleClick(QModelIndex index);

    /*!
     * \brief Synchronize to the master histogram list
     */
    void onHistogramListChanged();

    //////////////////////////////////////////////////////////////////////////
private:
    /*!
     * \brief histExists
     * \param name
     * \return
     */
    bool histExists(const QString& name);
    void deleteHists();
    void setIcon(QListWidgetItem* pItem);

    /*!
     * \brief Binary search the ListWidget for a
     * \param min    lower bound index
     * \param max    upper bound index
     * \param name   name
     * \return
     */
    int binarySearch(int min, int max, const QString& name);

private:
    std::unique_ptr<Ui::HistogramView> ui;
    std::shared_ptr<SpecTclInterface> m_pSpecTcl;
};

} // end of namespace

#endif // HISTOGRAMVIEW_H
