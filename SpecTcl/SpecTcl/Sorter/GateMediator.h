#ifndef __GATEMEDIATOR_H
#define __GATEMEDIATOR_H

#ifndef __GATECONTAINER_H
#include "GateContainer.h"
#endif

#ifndef __SPECTRUM_H
#include <Spectrum.h>
#endif

#ifndef __STL_STRING
#include <string>
#define __STL_STRING
#endif

#ifndef __STL_VECTOR
#include <vector>
#define __STL_VECTOR
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
};

#endif
