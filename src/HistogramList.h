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

#ifndef HISTOGRAMLIST_H
#define HISTOGRAMLIST_H

#include "HistogramBundle.h"
#include "GateList.h"

#include <QFrame>
#include <QString>

#include <memory>
#include <map>

class TH1;

namespace SpJs {
    class HistInfo;
}

namespace Viewer
{

/*! \brief Owner of all the HistogramBundles in the application
 *
 */
class HistogramList : public QObject
{
    Q_OBJECT
    
public:
    using iterator       = typename std::map<QString, std::unique_ptr<HistogramBundle> >::iterator;
    using const_iterator = typename std::map<QString, std::unique_ptr<HistogramBundle> >::const_iterator;

    explicit HistogramList(QObject *parent = 0);
    HistogramList(const HistogramList&) = delete;
    ~HistogramList();

    QList<QString> histNames();

    void clear();
    std::map<QString, HistogramBundle>::size_type size() { return m_hists.size(); }

    iterator begin() { return m_hists.begin();}
    iterator end() { return m_hists.end();}

    HistogramBundle* addHist(std::unique_ptr<TH1> hist, const SpJs::HistInfo& info);
    HistogramBundle* addHist(std::unique_ptr<HistogramBundle> hist);

    void clearCuts();

    void synchronize(const GateList& pList);
    void synchronize1d(GateList::iterator1d begin, GateList::iterator1d end);
    void synchronize2d(GateList::iterator2d begin, GateList::iterator2d end);

public slots:

    bool histExists(const QString& name);
    HistogramBundle* getHist(const QString& name);

    void removeSlice(const GSlice& slice);
    void removeGate(const GGate& gate);

    void addSlice(GSlice* slice);
    void addGate(GGate* gate);

private:
    std::map<QString, std::unique_ptr<HistogramBundle> > m_hists;
    QMutex m_mutex;

};

} // end of namespace

#endif // HISTOGRAMLIST_H
