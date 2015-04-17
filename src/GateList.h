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

#ifndef GATELIST_H
#define GATELIST_H

#include <QString>
#include <set>
#include <memory>

namespace SpJs
{
  class GateInfo;
  class GateInfo2D;
}

#include "GGate.h"
#include "GSlice.h"

class GateList
{
  public:
    struct Compare1D {
      bool operator()(const std::unique_ptr<GSlice>& lhs,
                      const std::unique_ptr<GSlice>& rhs) const;
    };

    struct Compare2D {
      bool operator()(const std::unique_ptr<GGate>& lhs,
                      const std::unique_ptr<GGate>& rhs) const;
    };

  public:
    using container1d_type = std::set<std::unique_ptr<GSlice>, Compare1D>;
    using iterator1d = typename container1d_type::iterator;

    using container2d_type = std::set<std::unique_ptr<GGate>, Compare2D>;
    using iterator2d = typename container2d_type::iterator;

public:
    GateList();

    void synchronize(std::vector<SpJs::GateInfo*> gates);

    void addCut1D(const SpJs::GateInfo& slice);
    void addCut1D(std::unique_ptr<GSlice> slice);

    void addCut2D(const SpJs::GateInfo2D& gate);
    void addCut2D(std::unique_ptr<GGate> gate);

    size_t size() const;

    iterator1d find1D(const QString& name);
    iterator2d find2D(const QString& name);

    iterator1d begin1d() const { return m_cuts1d.begin(); }
    iterator1d end1d() const { return m_cuts1d.end(); }

    iterator2d begin2d() const { return m_cuts2d.begin(); }
    iterator2d end2d() const { return m_cuts2d.end(); }
private:
    container1d_type m_cuts1d;
    container2d_type m_cuts2d;
};

#endif // GATELIST_H
