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

#include "TestXamineShMem.h"
#include "Spectrum.h"
#include "GateContainer.h"

#include <algorithm>

CTestXamineShMem::CTestXamineShMem()
    : m_slotIndex(0)
{
}

const std::map<CSpectrum*, BoundSpectrum>& CTestXamineShMem::boundSpectra() const
{
    return m_map;
}

const std::map<int, CXamineGate>& CTestXamineShMem::getGates() const
{
    return m_gateMap;
}

void CTestXamineShMem::setManaged(bool value) {}

bool CTestXamineShMem::isManaged() const {return false;}

void CTestXamineShMem::attach() {}
void CTestXamineShMem::detach() {}

size_t CTestXamineShMem::getSize() const {return 1024*1024;}

UInt_t CTestXamineShMem::addSpectrum(CSpectrum& spec, CHistogrammer& sorter) {

    BoundSpectrum bs;
    bs.s_info = spec.getName();
    bs.s_slot = m_slotIndex;
    m_map[&spec] = bs;

    m_slotIndex++;
    return bs.s_slot;
}

UInt_t CTestXamineShMem::removeSpectrum(UInt_t slot, CSpectrum& ) {return 0;}


DisplayBindings CTestXamineShMem::getDisplayBindings() const {

    DisplayBindings bindings;
    for (auto& entry : m_map) {
        int slot = entry.second.s_slot;
        if (bindings.size() <= slot) {
            bindings.resize(slot+1);
        }
        bindings.at(slot) = entry.first->getName();
    }

    return bindings;
}

CXamineSpectrum& CTestXamineShMem::operator[](UInt_t n) {}

Int_t CTestXamineShMem::findDisplayBinding(std::string name) {
    return 10;
}
Int_t CTestXamineShMem::findDisplayBinding(CSpectrum& rSpectrum) { return 0;}

void CTestXamineShMem::addFit(CSpectrumFit& fit) {}
void CTestXamineShMem::deleteFit(CSpectrumFit& fit) {}

std::string CTestXamineShMem::createTitle(CSpectrum& rSpectrum,
                                          UInt_t maxLength,
                                          CHistogrammer &rSorter) {
    auto title = rSpectrum.getName();
    return std::string(title.begin(),
                  title.begin() + std::min(size_t(maxLength), title.size()) );
}

UInt_t CTestXamineShMem::getTitleSize()  const { return 128;}

void CTestXamineShMem::setTitle(std::string name, UInt_t slot) {
    auto it = findBySlot(m_map, slot);
    if (it != m_map.end()) {
        it->second.s_info = name;
    }
}
void CTestXamineShMem::setInfo(CSpectrum& rSpectrum, std::string name) {
    auto it = m_map.find(&rSpectrum);
    if (it != m_map.end()) {
        it->second.s_info = name;
    }
}
void CTestXamineShMem::setInfo(std::string  name, UInt_t slot) {
    auto it = findBySlot(m_map, slot);
    if (it != m_map.end()) {
        it->second.s_info = name;
    }
}


void CTestXamineShMem::addGate (CXamineGate& rGate) {
    auto candidate = std::make_pair(rGate.getId(), CXamineGate(rGate));
    m_gateMap.insert(m_gateMap.begin(), candidate);
}

void CTestXamineShMem::removeGate (UInt_t nSpectrum, UInt_t nId, GateType_t eType)  {
    auto it = m_gateMap.find(nSpectrum);
    if (it != m_gateMap.end()) {
        m_gateMap.erase(it);
    }
}
void CTestXamineShMem::removeGate(UInt_t ns, UInt_t gid) {}

CXamineGates* CTestXamineShMem::GetGates (UInt_t nSpectrum)  {
    return nullptr;
}

void CTestXamineShMem::setUnderflows(unsigned slot, unsigned x, unsigned y)
{
    auto it = findBySlot(m_map, slot);
    if (it != m_map.end()) {
        BoundSpectrum& info = it->second;
        if (y != 0) {
            info.s_underflows = {x, y};
        } else {
            info.s_underflows = {x};
        }
    }
}

void CTestXamineShMem::setOverflows(unsigned slot, unsigned x, unsigned y)
{
    auto it = findBySlot(m_map, slot);
    if (it != m_map.end()) {
        BoundSpectrum& info = it->second;
        if (y != 0) {
            info.s_overflows = {x, y};
        } else {
            info.s_overflows = {x};
        }
    }
}

void CTestXamineShMem::clearStatistics(unsigned slot) {
    auto it = findBySlot(m_map, slot);
    if (it != m_map.end()) {
        BoundSpectrum& info = it->second;
        info.s_underflows.clear();
        info.s_overflows.clear();
    }
}



std::map<CSpectrum*, BoundSpectrum>::iterator
findBySlot(std::map<CSpectrum*, BoundSpectrum> &map, int slot)
{
    return find_if(map.begin(),
                   map.end(),
                   [&slot](std::map<CSpectrum*, BoundSpectrum>::const_reference value) -> bool {
                        return (value.second.s_slot == slot);
                    });
}

std::map<CSpectrum*, BoundSpectrum>::iterator
findByName(std::map<CSpectrum*, BoundSpectrum> &map, std::string name)
{
    return find_if(map.begin(),
                   map.end(),
                   [&name](std::map<CSpectrum*, BoundSpectrum>::const_reference value) -> bool {
                        return (value.first->getName() == name);
                   });
}
