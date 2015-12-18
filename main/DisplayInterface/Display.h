/*
    This software is Copyright by the Board of Trustees of Michigan
    State University (c) Copyright 2015.

    You may use this software under the terms of the GNU public license
    (GPL).  The terms of this license are described at:

     http://www.gnu.org/licenses/gpl.txt

     Author:
             Jeromy Tompkins
         NSCL
         Michigan State University
         East Lansing, MI 48824-1321
*/


#ifndef DISPLAY_H
#define DISPLAY_H

#include <daqdatatypes.h>
#include <histotypes.h>

#include <string>
#include <vector>
#include <list>

class CDisplayGate;
class CSpectrum;
class CSpectrumFit;
class CGateContainer;

// Display binding management types:
typedef std::vector<std::string>   DisplayBindings;
typedef DisplayBindings::iterator  DisplayBindingsIterator;

class CDisplay
{
public:
    typedef std::pair<int, std::string> BoundFitline;
    typedef std::list<BoundFitline>     FitlineList;
    typedef std::vector<FitlineList>    FitlineBindings;

public:
    CDisplay();
    CDisplay(const CDisplay&);
    virtual ~CDisplay();

    virtual int operator==(const CDisplay&) = 0;

    virtual CDisplay* clone() const = 0;

    virtual void Start() = 0;
    virtual void Stop() = 0;
    virtual Bool_t isAlive() = 0;
    virtual void Restart() = 0;

    const DisplayBindings&  getDisplayBindings() const;
    void setDisplayBindings (const DisplayBindings& am_DisplayBindings);

    virtual UInt_t BindToDisplay(CSpectrum& rSpectrum) = 0;
    virtual void   UnBindFromDisplay(UInt_t nSpec, CSpectrum& rSpectrum) = 0;

    virtual void addFit(CSpectrumFit& fit) = 0;
    virtual void deleteFit(CSpectrumFit& fit) = 0;

    virtual void updateStatistics() = 0;

    virtual std::vector<CGateContainer> GatesToDisplay(const std::string& rSpectrum) = 0;

    virtual CSpectrum* DisplayBinding(UInt_t xid) = 0;
    virtual Int_t FindDisplayBinding(std::string name) = 0;
    virtual UInt_t DisplayBindingsSize() const = 0;

    virtual void setInfo(std::string name, UInt_t slot) = 0;
    virtual void setTitle(std::string name, UInt_t slot) = 0;
    virtual UInt_t getTitleSize() const = 0;

    virtual void EnterGate(CDisplayGate& rGate) = 0;
    virtual void RemoveGate(UInt_t nSpectrum, UInt_t nId, GateType_t eType) = 0;

    virtual void setOverflows(unsigned slot, unsigned x, unsigned y) = 0;
    virtual void setUnderflows(unsigned slot, unsigned x, unsigned y) = 0;
};

#endif // DISPLAY_H
