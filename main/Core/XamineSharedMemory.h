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

#ifndef XAMINESHAREDMEMORY_H
#define XAMINESHAREDMEMORY_H

#include <daqdatatypes.h>
#include <histotypes.h>


#include <vector>
#include <string>

class CSpectrumFit;
class CSpectrum;
class CXamineSpectrum;
class CHistogrammer;
class CGateContainer;
class CXamineGate;
class CXamineGates;

using DisplayBindings         = std::vector<std::string>;
using DisplayBindingsIterator = DisplayBindings::iterator;
using SpectrumContainer       = std::vector<CSpectrum*>;

/*!
 * \brief The CXamineSharedMemory class
 *
 * Xamine and Spectra both depend on the ability to interact with
 * a shared memory region. For testing purposes, it is useful to
 * abstract away the actual reliance on the shared memory. This class
 * defines the interface that the rest of SpecTcl will depend on.
 *
 * The interface defines a lower level interface to interact wit hthe
 * shared memory that models what the libXamine client interface supports.
 * Memory regions (or slots) are accessed via slot indexes.
 */
class CXamineSharedMemory
{

public:
    virtual void setManaged(bool value) = 0;

    virtual bool isManaged() const = 0;

    virtual void attach() = 0;
    virtual void detach() = 0;

    virtual size_t getSize() const = 0;

    virtual UInt_t addSpectrum(CSpectrum& , CHistogrammer& ) = 0;
    virtual void removeSpectrum(UInt_t slot, CSpectrum& ) = 0;

    virtual DisplayBindings getDisplayBindings() const = 0;

    virtual CXamineSpectrum& operator[](UInt_t n) = 0;

    virtual Int_t findDisplayBinding(std::string name) = 0;
    virtual Int_t findDisplayBinding(CSpectrum& rSpectrum) = 0;

    virtual void addFit(CSpectrumFit& fit) = 0;
    virtual void deleteFit(CSpectrumFit& fit) = 0;

    virtual std::string createTitle(CSpectrum& rSpectrum,
                            UInt_t maxLength,
                            CHistogrammer &rSorter) = 0;
    virtual UInt_t getTitleSize()  const = 0;
    virtual void setTitle(std::string name, UInt_t slot) = 0;
    virtual void setInfo(CSpectrum& rSpectrum, std::string name) = 0;
    virtual void setInfo(std::string  name, UInt_t slot) = 0;

    virtual void addGate (CXamineGate& rGate) = 0;
    virtual void removeGate (UInt_t nSpectrum, UInt_t nId, GateType_t eType)  = 0;
    virtual void removeGate(UInt_t nSPectrum, UInt_t nId) = 0;
    virtual CXamineGates* GetGates (UInt_t nSpectrum)  = 0;

    virtual void setUnderflows(unsigned slot, unsigned x, unsigned y) = 0;
    virtual void setOverflows(unsigned slot, unsigned x, unsigned y) = 0;
    virtual void clearStatistics(unsigned slot) = 0;

};

#endif // XAMINESHAREDMEMORY_H
