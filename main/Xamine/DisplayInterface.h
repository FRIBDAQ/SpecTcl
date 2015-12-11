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

class CSpectrumFit;

class CDisplayInterface
{


public:
    CDisplayInterface();
    CDisplayInterface(const CDisplayInterface&);
    virtual ~CDisplayInterface();

    UInt_t BindToDisplay(const std::string& rsName);
    UInt_t UnBindFromDisplay(UInt_t nSpec);
    void addFit(CSpectrumFit& fit);
    void deleteFit(CSpectrumFit& fit);
    void updateStatistics();

    CDisplayGate* GateToXamineGate(UInt_t nBindingId, CGateContainer& rGate);
    void AddGateToBoundSpectra(CGateContainer& rGate);
    void RemoveGateFromBoundSpectra(CGateContainer& rGate);
    std::vector<CGateContainer> GatesToDisplay(const std::string& rSpectrum);
    CSpectrum* DisplayBinding(UInt_t xid);
    Display
    
};

#endif // DISPLAYINTERFACE_H
