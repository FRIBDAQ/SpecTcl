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
#include "GateList.h"
#include "GSlice.h"
#include "GGate.h"

#include <memory>
#include <TH1.h>
#include <TH2.h>
#include <QString>
#include <QMap>
#include <QMutexLocker>
#include <stdexcept>

#include <memory>
#include <algorithm>

using namespace std;

namespace Viewer
{

HistogramList::HistogramList(QObject *parent) :
    QObject(parent),
    m_hists(),
    m_mutex()
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
    QMutexLocker lock(&m_mutex);
    auto iter = m_hists.find(name);
    return (iter!=m_hists.end());
}


HistogramBundle* HistogramList::getHist(const QString &name)
{
    QMutexLocker lock(&m_mutex);
    auto iter = m_hists.find(name);
    if (iter!=m_hists.end()) {
        return iter->second.get();
    } else {
        throw std::runtime_error("Requested histogram not found");
    }
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
  QMutexLocker lock(&m_mutex);

  // this is kind of clunky but it i cannot use insert or assignment
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

void HistogramList::synchronize(const GateList& list)
{
  for (auto& bundlePair : m_hists) {
      bundlePair.second->synchronizeGates(&list);
  }
}


// The HistogramBundles don't own the cuts and only maintain a cache of pointers to
// the gates in the GateList. So we can essentially just clear their caches
// and then repopulate them.
void HistogramList::synchronize1d(GateList::iterator1d b, GateList::iterator1d e)
{
    auto it = begin();
    auto it_end = end();

    // histogram bundles just own a reference to the cuts so we don't care
    // about clearing their cut cache...
    while ( it != it_end ) {
        auto& pHist = it->second;

        // only operate on 1d hists
        if ( ! pHist->hist()->InheritsFrom(TH2::Class()) ) {
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
void HistogramList::synchronize2d(GateList::iterator2d b, GateList::iterator2d e)
{
    auto it = begin();
    auto it_end = end();

    // histogram bundles just own a reference to the cuts so we don't care
    // about clear their cut cache...
    while ( it != it_end ) {
        auto& pHist = it->second;

        // only operate on 1d hists
        if ( pHist->hist()->InheritsFrom(TH2::Class()) ) {
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

bool HistogramList::update(const vector<SpJs::HistInfo*>& hists)
{
  return false;
}

void HistogramList::addSlice(GSlice* pSlice)
{
  auto it = begin();
  auto it_end = end();

  auto name = pSlice->getParameter();

  while ( it != it_end ) {
    
    auto& pHist = it->second;
    // only apply to 1d hists
    if ( ! pHist->hist()->InheritsFrom(TH2::Class()) ) {

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
    if ( pHist->hist()->InheritsFrom(TH2::Class()) ) {

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
