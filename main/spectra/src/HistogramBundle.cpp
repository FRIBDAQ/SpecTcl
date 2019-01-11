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

#include "HistogramBundle.h"
#include "GSlice.h"
#include "GGate.h"
#include "MasterGateList.h"
#include "CanvasOps.h"
#include "GraphicalObject.h"

#include <QMutex>
#include <QMutexLocker>
#include <QString>

#include <TH1.h>
#include <TH2.h>

#include <TCanvas.h>

#include <utility>
#include <iostream>
#include <algorithm>

using namespace std;


namespace Viewer
{

//
//
HistogramBundle::HistogramBundle(unique_ptr<QMutex> pMutex,
                                 unique_ptr<TH1> pHist, const SpJs::HistInfo& info)
    : m_pMutex(std::move(pMutex)),
      m_pHist(std::move(pHist)),
      m_cuts1d(),
      m_cuts2d(),
      m_grobs(),
      m_hInfo(info),
      m_defaultDrawOption(),
      m_subscriber(*this)
{
	if (m_pHist) {
		if (m_pHist->InheritsFrom(TH2::Class())) {
			m_defaultDrawOption = "col2";
		}
	}
}

HistogramBundle::~HistogramBundle()
{
    unsubscribeFromAllClones();
    
    // @todo Should get rid of all the objects when there are no remaining clones
    // but that's also not done for cuts; so this is a memory leak IMHO
}

void HistogramBundle::addGrobj(GraphicalObject* obj)
{
    m_grobs.insert({obj->getName(), obj});      // c++11 supports container inits.
}

//
//
void HistogramBundle::addCut1D(GSlice* pSlice) {
    m_cuts1d.insert(make_pair(pSlice->getName(), pSlice)) ;
}

//
//
void HistogramBundle::addCut2D(GGate* pCut) {
    m_cuts2d.insert(make_pair(pCut->getName(), pCut)) ;
}

//
//
void HistogramBundle::drawClone(const QString &opts)
{
    QByteArray ascii = opts.toAscii();
    const char* cOpts = ascii.constData();

    if (m_pHist->InheritsFrom(TH2::Class())) {
        auto pClonedHist = dynamic_cast<SubscribableH1<TH2D>* >(m_pHist->DrawCopy(cOpts, "_copy"));
        m_clones[gPad] = pClonedHist;
        pClonedHist->subscribe(m_subscriber);
    } else {
        auto pClonedHist = dynamic_cast<SubscribableH1<TH1D>* >(m_pHist->DrawCopy(cOpts, "_copy"));
        m_clones[gPad] = pClonedHist;
        pClonedHist->subscribe(m_subscriber);
    }
}

void HistogramBundle::unsubscribeFromAllClones()
{
    for (auto cloneInfo : m_clones) {
        TH1* pHist = cloneInfo.second;
        if (pHist->InheritsFrom(TH2::Class())) {
            auto pSubscribable = dynamic_cast< SubscribableH1<TH2D>* >(pHist);
            pSubscribable->unsubscribe(m_subscriber);
        } else {
            auto pSubscribable = dynamic_cast< SubscribableH1<TH1D>* >(pHist);
            pSubscribable->unsubscribe(m_subscriber);
        }
    }
}

void HistogramBundle::updateClone(TH1& hClone, const QString& opts)
{
    gPad->Modified(1);
    if (opts != CanvasOps::getDrawOption(gPad, &hClone)) {
        CanvasOps::setDrawOption(gPad, &hClone, opts);
    }
}

void HistogramBundle::draw(const QString& opt) {

    QString opts(opt);
    if (opts.isEmpty()) {
        opts = m_defaultDrawOption;
    }

//    std::cout << opts.toUtf8().constData() << std::endl;

    // first check to see if there is already a histogram copy on the current pad
    // if there is, then we just need to update it. Otherwise, we need to draw a brand
    // new clone of the histogram.

    auto pFoundPair = m_clones.find(gPad);
    if (pFoundPair != m_clones.end()) {

        // if there is a clone on the pad already, update the draw option only...
        // don't create another copy
        if (opts.contains("same", Qt::CaseInsensitive)) {
            updateClone(*pFoundPair->second, opts);
        } else {
            drawClone(opts);
        }
    } else {
        drawClone(opts);
    }

    for (auto cut : m_cuts1d) { cut.second->draw(); }

    for (auto cut : m_cuts2d) { cut.second->draw(); }
    
    for (auto o : m_grobs) { o.second->draw(); }
}

bool HistogramBundle::isVisible() const {
    return (m_clones.size() != 0);
}

//
//
bool HistogramBundle::synchronize2DGates(const MasterGateList* pGateList)
{
    bool somethingChanged = false;
    map<QString, GGate*> tempList;

    // 2d spectra need only to update their 2d cuts
    auto it2d = pGateList->begin2d();
    auto it2d_end = pGateList->end2d();

    // update existing or add nonexisting
    while ( it2d != it2d_end ) {
      // loop through 1d cuts in GateList

      // for convenience declare variable to store value referenced by iterator
      auto& pExtGate = (*it2d);
      QString name   = pExtGate->getName();

      // Keep the gate if all gate parameters are in our spectrum.
      
      auto& localParams = m_hInfo.s_params;
      auto  gateParams  = pExtGate->getParameters();
      if (localParams.size() == gateParams.size()) {
        bool match(true);
        for (int i = 0; i < localParams.size(); i++) {
            if (gateParams.count(QString::fromStdString(localParams[i])) == 0) { 
                match = false;              // Not in set of gate parameters.
                break;
            }
        }
        if (match) tempList[pExtGate->getName()] = pExtGate.get();
      }

      ++it2d;
    }

    if ( m_cuts2d.size() != tempList.size() ) {
      somethingChanged = true;
    } else {

        // predicate to compare the object referred to by the ptrs rather
        // rather than the pointers
        auto compareObjects = [](const pair<QString, GGate*>& lhs,
                                 const pair<QString, GGate*>& rhs) {
            return *(lhs.second) == *(rhs.second);
          };

        if ( ! equal(m_cuts2d.begin(), m_cuts2d.end(), tempList.begin(), compareObjects) ) {
             somethingChanged = true;
      }
    }
    swap( m_cuts2d, tempList);

    return somethingChanged;
}

//
// Synchrnozie the 1D gates stored with this histogram bundle. The synch
// process creates a new list of gates, checks whether it is different
// than the existing list of gates, and then swaps it out
bool HistogramBundle::synchronize1DGates(const MasterGateList* pGateList)
{
    bool somethingChanged = false;
    map<QString, GSlice*> tempList;

    // 1d spectra need only to update their 1d cuts
    auto it1d     = pGateList->begin1d();
    auto it1d_end = pGateList->end1d();

    // update existing or add nonexisting
    while ( it1d != it1d_end ) {
      // loop through 1d cuts in GateList

      // There are gamma slices and slices.  Gamma slices have several parameters
      // that all must be present in the current spectrum.
      // slices only have one parameter that has to be the parameter of this
      // spectrum.  Doing it this way I think removes the need to
      // care about the detailed spectrum type and gate types:
      
      auto& pExtSlice = *it1d;
      QString name = pExtSlice->getName();
      
      bool matchingGate(true);
      
      
      if (m_hInfo.s_params.size() == pExtSlice->parameterCount()) {
        for(int i = 0; i < m_hInfo.s_params.size(); i++) {
            if (m_hInfo.s_params[i] != pExtSlice->getParameter(i).toStdString()) {
                matchingGate = false;
                break;
            }
        }
      } else {
        matchingGate = false;
      }

      if (matchingGate) {
        tempList[name] = pExtSlice.get();
      }

      ++it1d;
    }

    if ( m_cuts1d.size() != tempList.size() ) {
        somethingChanged = true;
    } else {
      
      // predicate to compare the object referred to by the ptrs rather
      // rather than the pointers
      auto compareObjects = [](const pair<QString, GSlice*>& lhs,
                             const pair<QString, GSlice*>& rhs) {
        return *(lhs.second) == *(rhs.second);
      };

      if ( ! equal(m_cuts1d.begin(), m_cuts1d.end(), tempList.begin(), compareObjects) ) {
        somethingChanged = true;
      }
    }

    swap(m_cuts1d, tempList);

    return somethingChanged;
}

bool HistogramBundle::synchronizeGates(const MasterGateList* pGateList)
{
  bool somethingChanged = false;

  if ( m_pHist->InheritsFrom(TH2::Class()) ) {
      somethingChanged = (somethingChanged || synchronize2DGates(pGateList));

  } else {
      somethingChanged = (somethingChanged || synchronize1DGates(pGateList));

  }

  return somethingChanged;
}


void HistogramBundle::setDefaultDrawOption(const QString& opt)
{
	m_defaultDrawOption = opt;
}

QString HistogramBundle::getDefaultDrawOption() const
{
	return m_defaultDrawOption;
}


//
// This gets called when a clone is destroyed
// We have to forget about tahat clone so we don't continue to try to
// update it.
void HistogramBundle::notify(TH1 &hist)
{
    auto itFound = m_clones.end();
    for (auto it=m_clones.begin(); it!=m_clones.end(); ++it) {
        if (it->second == &hist) {
            itFound = it;
            break;
        }
    }

    if (itFound != m_clones.end()) {
//        std::cout << "Unsubscribing from " << (void*)itFound->second << std::endl;
        m_clones.erase(itFound);
        // the canvas owns the histogram so we don't need to delete it
    } // else do nothing
}


TH1* HistogramBundle::getClone(TVirtualPad &pad) const
{
    auto itFound = m_clones.find(&pad);
    if (itFound == m_clones.end()) {
        return nullptr;
    } else {
        return itFound->second;
    }
}


} // end of Viewer namespace


//
//
std::ostream& operator<<(std::ostream& str, const Viewer::HistogramBundle& hist)
{
  str << "HistogramBundle name=" << hist.getName().toStdString() << " @ " << (void*) &hist << endl;
  str << "Type = " << hist.getInfo().s_type << endl;
  str << "Slices = (";
  auto cuts1d = hist.getCut1Ds();
  auto it = cuts1d.begin();
  auto itend = cuts1d.end();
  while (it != itend) {
    str << it->second->getName().toStdString();

    ++it;

    if (it != itend) {
      str << ", ";
    }
  }
  str << ")" << endl;

  str << "Gates = (";
  auto cuts2d = hist.getCut2Ds();
  auto it2 = cuts2d.begin();
  auto it2end = cuts2d.end();
  while (it2 != it2end) {
    str << it2->second->getName().toStdString();

    ++it2;

    if (it2 != it2end) {
      str << ", ";
    }
  }
  str << ")" << endl;

  return str;
}




