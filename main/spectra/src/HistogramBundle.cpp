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

    std::cout << opts.toUtf8().constData() << std::endl;

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
      QString param0 = pExtGate->getParameterX();
      QString param1 = pExtGate->getParameterY();

      auto& localParams = m_hInfo.s_params;
      if ( (QString::fromStdString(localParams.at(0)) == param0 )
          && (QString::fromStdString(localParams.at(1))  == param1) ) {

          // parameter matches. we want this in our new list

        tempList[pExtGate->getName()] = pExtGate.get();

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

      auto& pExtSlice = *it1d;
      QString name = pExtSlice->getName();
      QString param = pExtSlice->getParameter();

      if ( QString::fromStdString(m_hInfo.s_params.at(0)) == param ) {
        // parameter matches. see if this exists already

        tempList[pExtSlice->getName()] = pExtSlice.get();

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


