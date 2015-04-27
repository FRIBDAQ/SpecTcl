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
#include "GateList.h"

#include <QMutex>
#include <QMutexLocker>
#include <QString>

#include <TH1.h>
#include <TH2.h>

#include <utility>
#include <iostream>
#include <algorithm>

using namespace std;

namespace Viewer
{

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
    m_cuts1d.insert(make_pair(pSlice->getName(), pSlice)) ;
}

void HistogramBundle::addCut2D(GGate* pCut) {
    QMutexLocker lock(m_pMutex.get());
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

bool HistogramBundle::synchronizeGates(GateList* pGateList)
{
  bool somethingChanged = false;

  if ( m_pHist->InheritsFrom(TH2::Class()) ) {

    map<QString, GGate*> tempList;

    // 2d spectra need only to update their 2d cuts
    auto it2d = pGateList->begin2d();
    auto it2d_end = pGateList->end2d();

    // update existing or add nonexisting
    while ( it2d != it2d_end ) {
      // loop through 1d cuts in GateList

      // for convenience declare variable to store value referenced by iterator
      auto& pExtGate = (*it2d);
      QString name = pExtGate->getName();
      QString param0 = pExtGate->getParameterX();
      QString param1 = pExtGate->getParameterY();

      auto& localParams = m_hInfo.s_params;
      if ( (QString::fromStdString(localParams.at(0)) == param0 )
          && (QString::fromStdString(localParams.at(1))  == param1) ) {
        // parameter matches. see if this exists already

//        auto itFound = find_if( m_cuts2d.begin(), m_cuts2d.end(), 
//            [&name](const pair<QString, GGate*>& pGate) {
//            return ( pGate.second->getName() == name );
//            });
//        if ( itFound != m_cuts2d.end() ) {
//          // gate exists... update it
//          GGate& localGate = *(itFound->second);
//          GGate& externGate = *pExtGate;
//
//          if (localGate != externGate) {
//            // gates differ, need to update local gate
//            somethingChanged = true;
//          }
//        } else {
//          // gate did not exist... add it
//          somethingChanged = true;
//        }
//
        tempList[pExtGate->getName()] = pExtGate.get();

      } 

      ++it2d;
    }

    if ( distance(m_cuts2d.begin(), m_cuts2d.end()) != distance(tempList.begin(), tempList.end()) ) {
      somethingChanged = true;
    } else {
      if ( ! equal(m_cuts2d.begin(), m_cuts2d.end(), tempList.end() ) ) {
        somethingChanged = true;
      }
    }
    swap( m_cuts2d, tempList);

  } else {

    map<QString, GSlice*> tempList;

    // 1d spectra need only to update their 1d cuts
    auto it1d = pGateList->begin1d();
    auto it1d_end = pGateList->end1d();

    // update existing or add nonexisting
    while ( it1d != it1d_end ) {
      // loop through 1d cuts in GateList

      auto& pExtSlice = *it1d;
      QString name = pExtSlice->getName();
      QString param = pExtSlice->getParameter();

      if ( QString::fromStdString(m_hInfo.s_params.at(0)) == param ) {
        // parameter matches. see if this exists already
//
//        auto itFound = find_if( m_cuts1d.begin(), m_cuts1d.end(), 
//            [&name](const pair<QString, GSlice*>& pSlice) {
//            return ( pSlice.second->getName() == name );
//            });
//
//        if ( itFound != m_cuts1d.end() ) {
//          // gate exists... update it
//          GSlice& localSlice = *(itFound->second);
//          GSlice& externSlice = *pExtSlice;
//
//          if (localSlice != externSlice) {
//            // gates differ, need to update local gate
//            somethingChanged = true;
//          }
//        } else {
//          // gate did not exist... add it
//          somethingChanged = true;
//        }

        tempList[pExtSlice->getName()] = pExtSlice.get();

      }

      ++it1d;
    }

    if ( distance(m_cuts1d.begin(), m_cuts1d.end()) != distance(tempList.begin(), tempList.end()) ) {
      somethingChanged = true;
    } else {
      if ( ! equal(m_cuts1d.begin(), m_cuts1d.end(), tempList.end() ) ) {
        somethingChanged = true;
      }
    }

    swap(m_cuts1d, tempList);
  }

    return somethingChanged;
}

} // end of Viewer namespace


std::ostream& operator<<(std::ostream& str, const Viewer::HistogramBundle& hist)
{
  cout << "HistogramBundle name=" << hist.getName().toStdString() << " @ " << (void*) &hist << endl;
  cout << "Type = " << hist.getInfo().s_type << endl;
  cout << "Slices = (";
  auto cuts1d = hist.getCut1Ds();
  auto it = cuts1d.begin();
  auto itend = cuts1d.end();
  while (it != itend) {
    cout << it->second->getName().toStdString();

    ++it;

    if (it != itend) {
      cout << ", ";
    }
  }
  cout << ")" << endl;

  cout << "Gates = (";
  auto cuts2d = hist.getCut2Ds();
  auto it2 = cuts2d.begin();
  auto it2end = cuts2d.end();
  while (it2 != it2end) {
    cout << it2->second->getName().toStdString();

    ++it2;

    if (it2 != it2end) {
      cout << ", ";
    }
  }
  cout << ")" << endl;

  return str;
}


