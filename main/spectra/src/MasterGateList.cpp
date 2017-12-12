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


#include "MasterGateList.h"
#include "GSlice.h"
#include "GGate.h"

#include <GateInfo.h>

#include <algorithm>
#include <chrono>
#include "Benchmark.h"

using namespace std;

namespace Viewer
{

/**
 ** This nested 'struct' provides a predicate that allows two gates to
 ** be compared. by name.
 **   @param   - left hand side of the comparison.
 **   @param   - right hand side of the comparison.
 **   @return bool - if the lhs name is lexicographically earlier than rhs's name
 **/
bool MasterGateList::Compare1D::operator()(const std::unique_ptr<GSlice>& lhs,
                                    const std::unique_ptr<GSlice>& rhs) const 
{
  return (lhs->getName().compare(rhs->getName()))<0;
}


/**
 *  See Compare2D - this is an unfortunate consequence of not deriving all
 *  gates from a 'named item' base class in which case both  there would
 *  not be a need to differentiate how 2d an 2d gate names are compared.
 */
bool MasterGateList::Compare2D::operator()(const std::unique_ptr<GGate>& lhs,
                                      const std::unique_ptr<GGate>& rhs) const 
{
  return (lhs->getName().compare(rhs->getName()))<0;
}

MasterGateList::MasterGateList()
    : m_cuts1d(),
    m_cuts2d()
{
}

bool MasterGateList::synchronize(std::vector<SpJs::GateInfo*> gates)
{
 bool somethingChanged=false;

 //Benchmark<4, std::chrono::high_resolution_clock> bm4;
 // make sure that we add all non existing gates to the this
 for (auto pGate : gates) {
    
    // Note that this business of finding/fixing existing gates:  That's
    // going to fail if a gate changes from a 1-d to a 2-d gate.
    // TODO: Fix that.  For now that's a low probability event.
    
    SpJs::GateType type = pGate->getType();
    if ((type == SpJs::GammaSliceGate) || (type == SpJs::SliceGate)) {

        auto it = find1D(QString::fromStdString(pGate->getName()));

        // if we did not find it, then add it
        if ( it == end1d() ) {
            addCut1D(*pGate);
            somethingChanged = true;
        } else {
            // we found it, so make sure its state is updated to reflect
            // the new state

            auto& existingSlice = *(*it);
            
            // This is ugly:
            
            if (type == SpJs::SliceGate) {
              GSlice newSlice(dynamic_cast<SpJs::Slice&>(*pGate));
              if ( existingSlice != newSlice ) {
                  existingSlice = newSlice;
                  somethingChanged = true;
              }
            } else if (type == SpJs::GammaSliceGate) {
              GSlice newSlice(dynamic_cast<SpJs::GammaSlice&>(*pGate));
              if (existingSlice != newSlice);
                existingSlice = newSlice;
                somethingChanged = true;
            }
        }

    } else if (
      (type == SpJs::BandGate) || (type == SpJs::ContourGate)  ||
      (type == SpJs::GammaBandGate) || (type == SpJs::GammaContourGate))
    ) {

        auto it = find2D(QString::fromStdString(pGate->getName()));

        // if we did not find it, then add it
        if (it == end2d() ) {
            SpJs::GateInfo2D& g2d = dynamic_cast<SpJs::GateInfo2D&>(*pGate);
            addCut2D(g2d);
            somethingChanged = true;
        } else {
            // we found it, so make sure its state is updated to reflect
            // the new state
            auto& existingGate = *(*it);
            GGate newGate(dynamic_cast<SpJs::GateInfo2D&>(*pGate));
            if (existingGate != newGate) {
              existingGate = newGate;
              somethingChanged = true;
            }
          }
    }
  }

  //Benchmark<44, std::chrono::high_resolution_clock> bm44;
  // remove all gates in gate list that are no longer is spectcl

  auto byName = [](SpJs::GateInfo* lhs, SpJs::GateInfo* rhs) {
      return lhs->getName() < rhs->getName();
  };

  // 1d cuts first
  auto it_1d = begin1d();
  auto itend_1d = end1d();

  auto sortedGates = gates;
  sort(sortedGates.begin(), sortedGates.end(), byName);

  while ( it_1d != itend_1d ) {
    QString name = (*it_1d)->getName();
    SpJs::Slice toFind;
    toFind.setName(name.toStdString());
    auto it = lower_bound(sortedGates.begin(), sortedGates.end(), &toFind, byName);

    if ( it != sortedGates.end() ) {
        if ( (*it)->getType() == SpJs::FalseGate ) {
          removeCut1D(name);
          somethingChanged = true;
        }
    } else {
      removeCut1D(name);
      somethingChanged = true;
    }

    // increment!
    ++it_1d;
  }

  //Benchmark<444, std::chrono::high_resolution_clock> bm444;
  // 2d cuts next
  auto it_2d = begin2d();
  auto itend_2d = end2d();
  while ( it_2d != itend_2d ) {
    QString name = (*it_2d)->getName();
    auto it = find_if(gates.begin(), gates.end(), [&name](SpJs::GateInfo* pInfo) {
                        return (name == QString::fromStdString(pInfo->getName()));
                      });
    if ( it != gates.end() ) {
      if ( (*it)->getType() == SpJs::FalseGate ) {
        removeCut2D(name);
        somethingChanged = true;
      }
    } else {
      cout << "about to remove " << name.toStdString() << endl;
      removeCut2D(name);
      somethingChanged = true;
    }

    // increment
    ++it_2d;
  }

  return somethingChanged;
}

void MasterGateList::addCut1D(const SpJs::GateInfo& slice)
{
  

  // There  are cuts and gamma cuts. unrecognized gate types are ignored:

  SpJs::GateType type = slice.getType();
  GSlice* pGate(0);
  if (type == SpJs::SliceGate) {
    const SpJs::Slice& jsSlice = dynamic_cast<const SpJs::Slice&>(slice);
    pGate = (new GSlice(jsSlice));
    
  } else if (type == SpJs::GammaSliceGate) {
    const SpJs::GammaSlice& gslice(dynamic_cast<const SpJs::GammaSlice&>(slice));
    pGate = (new GSlice(gslice));
  }
  if (pGate) {                   // Be sure we actually made a gate to add one
    unique_ptr<GSlice> gsl(pGate);
    addCut1D(move(gsl));
  }  

}

void MasterGateList::addCut1D(unique_ptr<GSlice> slice)
{
  slice->setEditable(false);
  m_cuts1d.insert( move(slice) );
}

void MasterGateList::addCut2D(const SpJs::GateInfo2D& gate)
{
  unique_ptr<GGate> ggate(new GGate(gate));
  
  addCut2D( move(ggate) );
}

void MasterGateList::addCut2D(unique_ptr<GGate> gate)
{
  gate->setEditable(false);
  m_cuts2d.insert( move(gate) );
}

size_t MasterGateList::size() const
{
  return m_cuts1d.size() + m_cuts2d.size();
}

void MasterGateList::clear()
{
  m_cuts1d.clear(); 
  m_cuts2d.clear();
}


void MasterGateList::removeCut1D(const QString& name)
{
  auto it = find1D(name);
  if (it != m_cuts1d.end()) {
    m_cuts1d.erase(it);
  }
}

void MasterGateList::removeCut2D(const QString& name)
{
  auto it = find2D(name);
  if (it != m_cuts2d.end()) {
    m_cuts2d.erase(it);
  }
}

MasterGateList::iterator1d MasterGateList::find1D(const QString& name)
{
  unique_ptr<GSlice> pSlice(new GSlice);
  pSlice->setName(name);
  return m_cuts1d.find(pSlice);
}

MasterGateList::iterator2d MasterGateList::find2D(const QString& name)
{
  unique_ptr<GGate> pGate(new GGate(SpJs::Contour()));
  pGate->setName(name);

  return m_cuts2d.find(pGate);
}

} // end of namespace
