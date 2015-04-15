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
#include <QFrame>
#include <QString>

#include <memory>
#include <map>

class TH1;
class TCutG;

namespace SpJs {
    class HistInfo;
}

/*! \brief Owner of all the HistogramBundles in the application
 *
 */
class HistogramList : public QObject
{
    Q_OBJECT
    
public:
    using iterator       = typename std::map<QString, std::unique_ptr<HistogramBundle> >::iterator;
    using const_iterator = typename std::map<QString, std::unique_ptr<HistogramBundle> >::const_iterator;

private:
    explicit HistogramList(QObject *parent = 0);
    HistogramList(const HistogramList&) = delete;
    ~HistogramList();

public:
    static HistogramList* getInstance()
    {
        if (m_instance == nullptr) {
            m_instance = new HistogramList;
        }
        return m_instance;
    }

    static QList<QString> histNames();

    static void clear();
    static std::map<QString, HistogramBundle>::size_type size() { return m_hists.size(); }

    static iterator begin() { return m_hists.begin();}
    static iterator end() { return m_hists.end();}

    static void addHist(std::unique_ptr<TH1> hist, const SpJs::HistInfo& info);
    static void addHist(std::unique_ptr<HistogramBundle> hist);

public slots:

    static bool histExists(const QString& name);
    static HistogramBundle* getHist(const QString& name);

    static void removeSlice(const GSlice& slice);
    static void removeGate(const GGate& gate);

    static void addSlice(GSlice* slice);
    static void addGate(GGate* gate);

private:
    static HistogramList* m_instance;

    static std::map<QString, std::unique_ptr<HistogramBundle> > m_hists;
    static QMutex m_mutex;

};

#endif // HISTOGRAMLIST_H
