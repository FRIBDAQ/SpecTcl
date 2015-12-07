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

#ifndef HISTOGRAMVIEW_H
#define HISTOGRAMVIEW_H

#include "HistogramList.h"
#include "QHistInfo.h"

#include <QDockWidget>
#include <QModelIndex>

#include <vector>

class QListWidgetItem;
class TH1;

namespace Ui {
class HistogramView;
}

namespace Viewer 
{

class SpecTclInterface;
class ListRequestHandler;
class GuardedHist;

/*! Graphical list of histograms
 */
class HistogramView : public QDockWidget
{
    Q_OBJECT
    
public:
    explicit HistogramView(SpecTclInterface* pSpecTcl, QWidget *parent = 0);
    ~HistogramView();

signals:
    void histSelected(HistogramBundle* hist);

public slots:
    void setList(std::vector<SpJs::HistInfo> list);
    void onDoubleClick(QModelIndex index);
    void onHistogramListChanged();

private:
    bool histExists(const QString& name);
    void deleteHists();
    void setIcon(QListWidgetItem* pItem);
    int binarySearch(int min, int max, const QString& name);

private:
    Ui::HistogramView *ui;
    ListRequestHandler* m_req;
    SpecTclInterface* m_pSpecTcl;
};

} // end of namespace

#endif // HISTOGRAMVIEW_H
