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

#ifndef TESTXAMINESHMEM_H
#define TESTXAMINESHMEM_H


#include "XamineSharedMemory.h"
#include "XamineGate.h"

#include <string>
#include <map>

class CSpectrum;


struct BoundSpectrum {
    int                   s_slot;
    std::string           s_info;
    std::vector<unsigned> s_underflows;
    std::vector<unsigned> s_overflows;
};

/*!
 * \brief The CTestXamineShMem class
 *
 * In order avoid actually dealing wth shared memory during test,
 * this class attempts to mimic some basic bookkeeping. It behaves kind
 * of like shared memory but all of the spectra and gates are stored in
 * std::maps. The goal was to keep this as simple as possible and fast.
 */
class CTestXamineShMem : public CXamineSharedMemory
{
private:
    int m_slotIndex;

    std::map<CSpectrum*, BoundSpectrum>    m_map;
    std::map<int, CXamineGate>             m_gateMap;

public:
    CTestXamineShMem();

    const std::map<CSpectrum*, BoundSpectrum>& boundSpectra() const;
    const std::map<int, CXamineGate>& getGates() const;

    virtual void setManaged(bool value);

    virtual bool isManaged() const;

    virtual void attach();
    virtual void detach();

    virtual size_t getSize() const;

    virtual UInt_t addSpectrum(CSpectrum& , CHistogrammer& );
    virtual void removeSpectrum(UInt_t slot, CSpectrum& );

    virtual DisplayBindings getDisplayBindings() const;

    virtual CXamineSpectrum& operator[](UInt_t n);

    virtual Int_t findDisplayBinding(std::string name);
    virtual Int_t findDisplayBinding(CSpectrum& rSpectrum);

    virtual void addFit(CSpectrumFit& fit);
    virtual void deleteFit(CSpectrumFit& fit);

    virtual std::string createTitle(CSpectrum& rSpectrum,
                            UInt_t maxLength,
                            CHistogrammer &rSorter);
    virtual UInt_t getTitleSize()  const;
    virtual void setTitle(std::string name, UInt_t slot);
    virtual void setInfo(CSpectrum& rSpectrum, std::string name);
    virtual void setInfo(std::string  name, UInt_t slot);

    virtual void addGate (CXamineGate& rGate);
    virtual void removeGate(CSpectrum& rSpectrum, CGateContainer& rGate) ;
    virtual void removeGate (UInt_t nSpectrum, UInt_t nId, GateType_t eType) ;
    virtual CXamineGates* GetGates (UInt_t nSpectrum) ;

    virtual void setUnderflows(unsigned slot, unsigned x, unsigned y);
    virtual void setOverflows(unsigned slot, unsigned x, unsigned y);
    virtual void clearStatistics(unsigned slot);
};

extern
std::map<CSpectrum*, BoundSpectrum>::iterator
findBySlot(std::map<CSpectrum*, BoundSpectrum>& map, int slot);

extern
std::map<CSpectrum*, BoundSpectrum>::iterator
findByName(std::map<CSpectrum*, BoundSpectrum>& map, std::string name);

#endif // TESTXAMINESHMEM_H
