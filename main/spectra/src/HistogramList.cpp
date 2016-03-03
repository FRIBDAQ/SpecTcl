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

#include "HistogramList.h"
#include "MasterGateList.h"
#include "GSlice.h"
#include "GGate.h"
#include "SpecTclInterface.h"

#include <HistFactory.h>
#include <HistInfo.h>
#include "Benchmark.h"

#include <TH1.h>
#include <TH2.h>

#include <QString>
#include <QMap>
#include <QMutexLocker>

#include <stdexcept>
#include <memory>
#include <algorithm>
#include <chrono>

using namespace std;

namespace Viewer
{

HistogramList::HistogramList(SpecTclInterface* pSpecTcl, QObject *parent) :
    QObject(parent),
    m_hists(),
    m_mutex(),
    m_pSpecTcl(pSpecTcl)
{}

HistogramList::~HistogramList()
{}

QList<QString> HistogramList::histNames()
{
  QList<QString> retList;

  for (auto& item : m_hists) {
    retList.push_back(item.first);
  }

  return retList;
}

void HistogramList::clear() {
  m_hists.clear();
}

bool HistogramList::histExists(const QString &name)
{
    auto iter = m_hists.find(name);
    return (iter!=m_hists.end());
}


HistogramBundle* HistogramList::getHist(const QString &name)
{
    auto iter = m_hists.find(name);
    if (iter!=m_hists.end()) {
        return iter->second.get();
    } else {
        throw std::runtime_error("Requested histogram not found");
    }
}

HistogramBundle* HistogramList::getHist(const TH1* pHist)
{
  return getHist(QString(pHist->GetName()));
}

HistogramBundle* HistogramList::addHist(std::unique_ptr<TH1> pHist, const SpJs::HistInfo& info)
{
  HistogramBundle* pHistBundle = nullptr;

  QString name(pHist->GetName());

    if (histExists(name)) {
        pHistBundle = getHist(name);
    } else {
        QMutexLocker lock(&m_mutex);
        unique_ptr<HistogramBundle> pBundle(new HistogramBundle(unique_ptr<QMutex>(new QMutex), 
                                                                std::move(pHist), 
                                                                info));

        auto itHist = m_hists.insert(make_pair(name, std::move(pBundle)));
        pHistBundle = itHist.first->second.get();
    }

    return pHistBundle;
}

HistogramBundle* HistogramList::addHist(unique_ptr<HistogramBundle> pHist) 
{
  QString name = pHist->getName();

  // this is kind of clunky but i cannot use insert or assignment
  m_hists[name].reset(pHist.release());

  return m_hists[name].get();
}

void HistogramList::removeSlice(const GSlice& slice)
{

  auto it = begin();
  auto it_end = end();

  auto name = slice.getName();

  while ( it != it_end ) {
    
    auto& cuts = it->second->getCut1Ds();

    // check if the cut exists and remove it if it does
    auto it_match = cuts.find(name);
    if ( it_match != cuts.end() ) {
      cuts.erase(it_match);
    }
    
    // update iterator 
    ++it;
  }
}

void HistogramList::removeGate(const GGate& gate)
{
  auto it = begin();
  auto it_end = end();

  auto name = gate.getName();

  while ( it != it_end ) {
    
    auto& cuts = it->second->getCut2Ds();

    // check if the cut exists and remove it if it does
    auto it_match = cuts.find(name);
    if ( it_match != cuts.end() ) {
      cuts.erase(it_match);
    }
    
    // update iterator 
    ++it;
  }
}

void HistogramList::synchronize(const MasterGateList& list)
{

  for (auto& bundlePair : m_hists) {
      bundlePair.second->synchronizeGates(&list);
  }
}


// The HistogramBundles don't own the cuts and only maintain a cache of pointers to
// the gates in the GateList. So we can essentially just clear their caches
// and then repopulate them.
void HistogramList::synchronize1d(MasterGateList::iterator1d b, MasterGateList::iterator1d e)
{
    auto it = begin();
    auto it_end = end();

    // histogram bundles just own a reference to the cuts so we don't care
    // about clearing their cut cache...
    while ( it != it_end ) {
        auto& pHist = it->second;

        // only operate on 1d hists
        if ( ! pHist->getHist().InheritsFrom(TH2::Class()) ) {
            pHist->getCut1Ds().clear();
        }
        ++it;
    }

    auto it1d = b;
    while ( it1d != e ) {
        // it1d points to a unique_ptr<GSlice>
        addSlice( it1d->get() );
        ++it1d;
    }

}

// The HistogramBundles don't own the cuts and only maintain a cache of pointers to
// the gates in the GateList. So we can essentially just clear their caches
// and then repopulate them.
void HistogramList::synchronize2d(MasterGateList::iterator2d b, MasterGateList::iterator2d e)
{
    auto it = begin();
    auto it_end = end();

    // histogram bundles just own a reference to the cuts so we don't care
    // about clear their cut cache...
    while ( it != it_end ) {
        auto& pHist = it->second;

        // only operate on 1d hists
        if ( pHist->getHist().InheritsFrom(TH2::Class()) ) {
            pHist->getCut2Ds().clear();
        }
        ++it;
    }

    auto it2d = b;
    while ( it2d != e ) {
        // it2d points to a unique_ptr<GGate>
        addGate( it2d->get() );
        ++it2d;
    }
}

bool HistogramList::update(const vector<SpJs::HistInfo>& hists)
{

//  Benchmark<1, std::chrono::high_resolution_clock> bm;
  bool somethingChanged = false;


  auto it = hists.begin();
  auto it_end = hists.end();

  // go through and add or update current histograms
  while (it != it_end) {

      const SpJs::HistInfo& info = (*it);
      QString name = QString::fromStdString(info.s_name);

      auto lessThanNameOnly = [&name](const pair<const QString, unique_ptr<HistogramBundle> >& item) {
          return (item.first < name);
      };

      auto itFound = m_hists.find(name);

      if (itFound == end()) {
          // there was no previous histogram... we need to create it

          SpJs::HistFactory factory;

          // returns a unique_ptr
          auto upHist = factory.create(info);

          // take ownership
          auto pHist = addHist(std::move(upHist), info);

          // a new hist needs to be told what gates it should
          // know about

          somethingChanged = true;
        } else {
          // check for if it is the same
          if ( itFound->second->getInfo() != info ) {
            // if not the same, then we need to replace the existing histogram
            // with a new one. This is basically a histogram that has changed
            // its type or axis or something else.

              SpJs::HistFactory factory;

              // returns a unique_ptr
              auto upHist = factory.create(info);

              auto pOldHist = itFound->second.get();

              // get rid of the old hist
              m_hists.erase(itFound);

              // take ownership of the new hist
              auto pHist = addHist(std::move(upHist), info);

              // a new hist needs to be told what gates it should
              // know about

              somethingChanged = true;

              emit histogramRemoved( pOldHist );
          } else {
              // this was the same info so we do not do anything
          }
        }

      ++it;
  }

  auto byName = [](const SpJs::HistInfo& info1, const SpJs::HistInfo& info2) {
                    return (info1.s_name < info2.s_name);
                  };

//  Benchmark<11, std::chrono::high_resolution_clock> bm2;
  auto sortedHists = hists;
  sort(sortedHists.begin(), sortedHists.end(), byName);
  // remove any defunct histograms
  auto nameInList = [&sortedHists, &byName](const QString& name) {
      SpJs::HistInfo temp;
      temp.s_name = name.toStdString();
      return binary_search(sortedHists.begin(), sortedHists.end(),
                           temp, byName);
  };

  // The following little algorithm structure was shamelessly stolen from
  // http://stackoverflow.com/questions/800955/remove-if-equivalent-for-stdmap
  // by Steve Folly (12/15/2010)
  auto itLocal = m_hists.begin();
  auto itLocal_end = m_hists.end();
  while ( itLocal != itLocal_end ) {
    if (!nameInList(itLocal->first)) {

        auto pHist = itLocal->second.get();

        m_hists.erase(itLocal++);
        somethingChanged = true;

        emit histogramRemoved( pHist );

      } else {
       ++itLocal;
      }
  }

  return somethingChanged;
}


void HistogramList::addSlice(GSlice* pSlice)
{
  auto it = begin();
  auto it_end = end();

  auto name = pSlice->getParameter();

  while ( it != it_end ) {
    
    auto& pHist = it->second;
    // only apply to 1d hists
    if ( ! pHist->getHist().InheritsFrom(TH2::Class()) ) {

      auto histParam = QString::fromStdString(pHist->getInfo().s_params.at(0));

      // only apply if parameter is matched
      if ( name == histParam ) {
        it->second->addCut1D(pSlice);
      }

    }
    
    // update iterator 
    ++it;
  }
}

void HistogramList::addGate(GGate* pGate)
{
  auto it = begin();
  auto it_end = end();

  auto nameX = pGate->getParameterX();
  auto nameY = pGate->getParameterY();

  while ( it != it_end ) {
    
    auto& pHist = it->second;
    // only apply to 2d hists
    if ( pHist->getHist().InheritsFrom(TH2::Class()) ) {

      auto histParamX = QString::fromStdString(pHist->getInfo().s_params.at(0));
      auto histParamY = QString::fromStdString(pHist->getInfo().s_params.at(1));

      // only apply if parameter is matched
      if ( (nameX == histParamX) && (nameY == histParamY) ) {
        pHist->addCut2D(pGate);
      }

    }

    // update iterator 
    ++it;
  }
}

void HistogramList::clearCuts()
{
  auto it = begin();
  auto it_end = end();

  while ( it != it_end ) {
    it->second->clearCut1Ds();
    it->second->clearCut2Ds();

    ++it;
  }
}

} // end of namespace
