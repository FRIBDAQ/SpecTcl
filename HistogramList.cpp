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

static const char* Copyright = "(C) Copyright Michigan State University 2015, All rights reserved";
#include "HistogramList.h"
#include <memory>
#include <TH1.h>
#include <QString>
#include <QMap>
#include <QMutexLocker>
#include <stdexcept>

HistogramList* HistogramList::m_instance = nullptr;

QMap<QString,HistogramBundle> HistogramList::m_hists;
QMutex HistogramList::m_mutex;

HistogramList::HistogramList(QObject *parent) :
    QObject(parent)
{}

HistogramList::~HistogramList()
{
    // free all of the hists
    for (auto pair : m_hists) {

        // do not delete a hist if it is being used.
        pair.lock();
        delete pair.hist();
        pair.unlock();

        // delete the hist's mutex
        delete pair.hist();
    }
}


bool HistogramList::histExists(const QString &name)
{
    QMutexLocker lock(&m_mutex);
    auto iter = m_hists.find(name);
    return (iter!=m_hists.end());
}


HistogramBundle* HistogramList::getHist(const QString &name)
{
    QMutexLocker lock(&m_mutex);
    auto iter = m_hists.find(name);
    if (iter!=m_hists.end()) {
        return &(*iter);
    } else {
        throw std::runtime_error("Requested histogram not found");
    }
}

void HistogramList::addHist(TH1& rHist)
{
    QString name(rHist.GetName());

    if (histExists(name)) {
        return;
    } else {
        QMutexLocker lock(&m_mutex);

        m_hists.insert(name, HistogramBundle(*(new QMutex),rHist));
    }
}

