/*
    This software is Copyright by the Board of Trustees of Michigan
    State University (c) Copyright 2014.

    You may use this software under the terms of the GNU public license
    (GPL).  The terms of this license are described at:

     http://www.gnu.org/licenses/gpl.txt

     Authors:
             Ron Fox
             Jeromy Tompkins 
	     NSCL
	     Michigan State University
	     East Lansing, MI 48824-1321
*/


#ifndef XAMINEGATEFACTORY_H
#define XAMINEGATEFACTORY_H


#include "daqdatatypes.h"

class CXamineSharedMemory;
class CXamineGate;
class CSpectrum;
class CGateContainer;

class CXamineGateFactory
{
private:
    CXamineSharedMemory* m_pMemory;

public:
    CXamineGateFactory(CXamineSharedMemory* pSharedMem);

    CXamineGate* fromSpecTclGate(CSpectrum &rSpectrum, CGateContainer& rGate);

private:
    bool flip2dGatePoints(CSpectrum* pSpectrum, UInt_t gXparam);
};

#endif // XAMINEGATEFACTORY_H
