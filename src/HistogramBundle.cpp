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
#include "HistogramBundle.h"
#include "GSlice.h"
#include "GGate.h"
#include <QMutex>
#include <QMutexLocker>
#include <QString>
#include <TH1.h>
#include <TH2.h>

#include <utility>
#include <iostream>

using namespace std;

HistogramBundle::HistogramBundle()
    : m_pMutex(),
      m_pHist(nullptr),
      m_cuts1d(),
      m_cuts2d(),
      m_hInfo()
{}

HistogramBundle::HistogramBundle(unique_ptr<QMutex> pMutex, 
                                 unique_ptr<TH1> pHist, const SpJs::HistInfo& info)
    : m_pMutex(std::move(pMutex)),
      m_pHist(std::move(pHist)),
      m_cuts1d(),
      m_cuts2d(),
      m_hInfo(info)
{}

void HistogramBundle::addCut1D(GSlice* pSlice) {
    QMutexLocker lock(m_pMutex.get());
    cout << "Adding gate " << pSlice->getName().toStdString() 
         << " (@" << (void*)pSlice << ") to " << m_hInfo.s_name << endl;
    m_cuts1d.insert(make_pair(pSlice->getName(), pSlice)) ;
}

void HistogramBundle::addCut2D(GGate* pCut) {
    QMutexLocker lock(m_pMutex.get());
    cout << "Adding gate " << pCut->getName().toStdString() 
      << " (tcutg @" << (void*)pCut->getGraphicObject() << ") to " << m_hInfo.s_name << endl;
    m_cuts2d.insert(make_pair(pCut->getName(), pCut)) ;
}

void HistogramBundle::draw(const QString& opt) {

  QString opts(opt);
    if (opts.isNull()) {
      if ( dynamic_cast<TH2*>(m_pHist.get()) ) {
        opts = "colz";
      }
    }
    const char* cOpts = opts.toAscii().constData();

    m_pMutex->lock();
    m_pHist->Draw(cOpts);
    m_pMutex->unlock();

    for (auto cut : m_cuts1d) {
        cut.second->draw();
    }

    for (auto cut : m_cuts2d) {
        cut.second->draw();
    }
}
