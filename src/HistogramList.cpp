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

HistogramList* HistogramList::m_instance = nullptr;

map<QString, unique_ptr<HistogramBundle> > HistogramList::m_hists;
QMutex HistogramList::m_mutex;

HistogramList::HistogramList(QObject *parent) :
    QObject(parent)
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

void HistogramList::addHist(std::unique_ptr<TH1> pHist, const SpJs::HistInfo& info)
{
    QString name(pHist->GetName());

    if (histExists(name)) {
        return;
    } else {
        QMutexLocker lock(&m_mutex);
        unique_ptr<HistogramBundle> pBundle(new HistogramBundle(unique_ptr<QMutex>(new QMutex), 
                                                                std::move(pHist), 
                                                                info));

        m_hists.insert(make_pair(name, std::move(pBundle)));
    }
}

void HistogramList::addHist(unique_ptr<HistogramBundle> pHist) 
{
  QString name = pHist->getName();
  QMutexLocker lock(&m_mutex);

  // this is kind of clunky but it i cannot use insert or assignment
  m_hists[name].reset(pHist.release());
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

void HistogramList::addSlice(GSlice* pSlice)
{
  auto it = begin();
  auto it_end = end();

  auto name = pSlice->getName();

  while ( it != it_end ) {
    
    if ( ! it->second->hist()->InheritsFrom(TH2::Class()) ) {
      auto& cuts = it->second->getCut1Ds();
      // check if the cut exists and remove it if it does
      it->second->addCut1D(pSlice);

    }
    
    // update iterator 
    ++it;
  }
}
