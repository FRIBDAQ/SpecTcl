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
#include "GateList.h"
#include "GSlice.h"
#include "GGate.h"

#include <GateInfo.h>

#include <algorithm>

using namespace std;

bool GateList::Compare1D::operator()(const std::unique_ptr<GSlice>& lhs,
                                    const std::unique_ptr<GSlice>& rhs) const 
{
  return (lhs->getName().compare(rhs->getName()))<0;
}

bool GateList::Compare2D::operator()(const std::unique_ptr<GGate>& lhs,
                                      const std::unique_ptr<GGate>& rhs) const 
{
  return (lhs->getName().compare(rhs->getName()))<0;
}

GateList::GateList()
    : m_cuts1d(),
    m_cuts2d()
{
}

void GateList::synchronize(std::vector<SpJs::GateInfo*> gates)
{
  m_cuts1d.clear();
  m_cuts2d.clear();

  for (auto pGate : gates) {
    
    SpJs::GateType type = pGate->getType();
    if (type == SpJs::SliceGate) {

      addCut1D(*pGate);

    } else if ( type == SpJs::BandGate || type == SpJs::ContourGate ) {

      SpJs::GateInfo2D& g2d = dynamic_cast<SpJs::GateInfo2D&>(*pGate);

      addCut2D(g2d);
    }
  }
}

void GateList::addCut1D(const SpJs::GateInfo& slice)
{
  const SpJs::Slice& jsSlice = dynamic_cast<const SpJs::Slice&>(slice);

  unique_ptr<GSlice> gsl(new GSlice(jsSlice));
  
  addCut1D( move(gsl) );
}

void GateList::addCut1D(unique_ptr<GSlice> slice)
{
  m_cuts1d.insert( move(slice) );
}

void GateList::addCut2D(const SpJs::GateInfo2D& gate)
{
  unique_ptr<GGate> ggate(new GGate(gate));
  
  addCut2D( move(ggate) );
}

void GateList::addCut2D(unique_ptr<GGate> gate)
{
  m_cuts2d.insert( move(gate) );
}

size_t GateList::size() const 
{
  return m_cuts1d.size() + m_cuts2d.size();
}


GateList::iterator1d GateList::find1D(const QString& name)
{
  return find_if(m_cuts1d.begin(), m_cuts1d.end(), 
                  [&name](const unique_ptr<GSlice>& slice) {
                    return (name == slice->getName());
                  });
}

GateList::iterator2d GateList::find2D(const QString& name)
{
  return find_if(m_cuts2d.begin(), m_cuts2d.end(), 
                  [&name](const unique_ptr<GGate>& slice) {
                    return (name == slice->getName());
                  });
}
