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


#ifndef DISPLAYINTERFACE_H
#define DISPLAYINTERFACE_H

#include <daqdatatypes.h>
#include <histotypes.h>
#include <xamineDataTypes.h>

#include <string>

class CDisplayGate;
class CXamineSpectrum;
class CXamineSpectra;
class CXamineGates;

class CDisplayInterface
{

public:
    CDisplayInterface();
    CDisplayInterface(const CDisplayInterface&);
    virtual ~CDisplayInterface();

    virtual int operator==(const CDisplayInterface&) = 0;

    virtual CDisplayInterface* clone() const = 0;

    virtual void Start() = 0;
    virtual void Stop() = 0;
    virtual Bool_t isAlive() = 0;
    virtual volatile Xamine_shared* getXamineMemory() const = 0;
    virtual Address_t DefineSpectrum(CXamineSpectrum& rSpectrum) = 0;
    virtual void setInfo(std::string name, UInt_t slot) = 0;
    virtual void setTitle(std::string name, UInt_t slot) = 0;
    virtual UInt_t getTitleSize() const = 0;
    virtual void EnterGate(CDisplayGate& rGate) = 0;
    virtual CXamineGates* GetGates(UInt_t nSpectrum) = 0;
    virtual void RemoveGate(UInt_t nSpectrum, UInt_t nId, GateType_t eType) = 0;
    virtual void FreeSpectrum(UInt_t nSpectrum) = 0;
    virtual void setOverflows(unsigned slot, unsigned x, unsigned y) = 0;
    virtual void setUnderflows(unsigned slot, unsigned x, unsigned y) = 0;
    virtual UInt_t GetEventFd() = 0;
};

#endif // DISPLAYINTERFACE_H
