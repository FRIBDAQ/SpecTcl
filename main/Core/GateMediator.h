/*
    This software is Copyright by the Board of Trustees of Michigan
    State University (c) Copyright 2005.

    You may use this software under the terms of the GNU public license
    (GPL).  The terms of this license are described at:

     http://www.gnu.org/licenses/gpl.txt

     Author:
             Ron Fox
	     NSCL
	     Michigan State University
	     East Lansing, MI 48824-1321
*/

#ifndef __GATEMEDIATOR_H
#define __GATEMEDIATOR_H

#ifndef __GATECONTAINER_H
#include "GateContainer.h"
#endif

#ifndef __SPECTRUM_H
#include <Spectrum.h>
#endif



class CSpectrum;
class CGateContainer;

class CGateMediator
{
  CGateContainer& m_rGate;   // The gate to mediate
  CSpectrum* m_pSpec;        // The spectrum to mediate
  
 public:

  // Default constructor
  CGateMediator(CGateContainer& rGate, CSpectrum* pSpec) :
    m_rGate(rGate),
    m_pSpec(pSpec)
    { }

  CGateMediator(const CGateMediator& aCGateMediator) :
    m_rGate (aCGateMediator.m_rGate),
    m_pSpec (aCGateMediator.m_pSpec)
    { }

  // Assignment operator
  CGateMediator operator= (const CGateMediator& aCGateMediator)
    {
      m_rGate = aCGateMediator.m_rGate;
      m_pSpec = aCGateMediator.m_pSpec;
    }

  ~CGateMediator() { }

  // Member functions
 public: 

  Bool_t operator() ();
 protected:
  Bool_t mediate1d();
  Bool_t mediate2d();
  Bool_t mediateGamma1();
  Bool_t mediateGamma2();
  Bool_t mediate2dMultiple();
  Bool_t mediateGamma2Deluxe();
};

#endif
