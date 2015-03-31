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
#include <QMap>
#include <QString>

class TH1;
class TCutG;

namespace SpJs {
    class HistInfo;
}

class HistogramList : public QObject
{
    Q_OBJECT
    
public:
    using iterator       = typename QMap<QString,HistogramBundle>::iterator;
    using const_iterator = typename QMap<QString,HistogramBundle>::const_iterator;

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

    static QList<QString> histNames() { return m_hists.keys(); }
    static QMap<QString,HistogramBundle>::iterator begin() { return m_hists.begin();}
    static QMap<QString,HistogramBundle>::iterator end() { return m_hists.end();}

public slots:

    static bool histExists(const QString& name);
    static HistogramBundle* getHist(const QString& name);
    static void addHist(TH1& hist, const SpJs::HistInfo& info);


private:
    static HistogramList* m_instance;

    static QMap<QString,HistogramBundle> m_hists;
    static QMutex m_mutex;

};

#endif // HISTOGRAMLIST_H
