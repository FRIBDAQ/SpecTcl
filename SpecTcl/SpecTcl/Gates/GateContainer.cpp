// GateContainer.cpp
//   This class allows Gates to be treated as 
//   named items which have constant 'pointers'
//   even if the gate itslef changes.  This is provided
//   for the convenience of CCompoundGate objects.
//   By retaining CGateContainer object references and not
//   CGate object reference, underlying gates can be 
//   modified etc.

//
//   Author:
//      Ron Fox
//      NSCL
//      Michigan State University
//      East Lansing, MI 48824-1321
//      mailto:fox@nscl.msu.edu
//
//////////////////////////.cpp file/////////////////////////////////////////////////////

//
// Header Files:
//

#include "GateContainer.h"                               

static const char* Copyright = 
"GateContainer.cpp: Copyright 1999 NSCL, All rights reserved\n";

// Functions for class CGateContainer

////////////////////////////////////////////////////////////////////////////
//
// Function:
//    CGateContainer (std::string& sName, UInt_t nNumber, 
//		      const CGate& rGate)
// Operation Type:
//    Constructor.
//
CGateContainer::CGateContainer(const std::string& sName, UInt_t nNumber, 
			       CGate& rGate) :
  CNamedItem(sName, nNumber),

  m_pGate(rGate.clone())
{
}
///////////////////////////////////////////////////////////////////////////
//
// Function:
//    ~CGateContainer()
// Operation Type:
//     Destructor.
//
CGateContainer::~CGateContainer()
{
  delete m_pGate;
}
/////////////////////////////////////////////////////////////////////////
//
// Function:
//   CGateContainer(const CGateContainer& rhs)
// Operation Type:
//   Copy construction
//
CGateContainer::CGateContainer(const CGateContainer& rhs) :
  CNamedItem(rhs),
  m_pGate(0)
{
  if(rhs.m_pGate) m_pGate = rhs.m_pGate->clone();
}
/////////////////////////////////////////////////////////////////////////
//
//  Function:
//     operator=(const CGateContainer& aCGateContainer)
//  Operation Type:
//     Assignment.
//
CGateContainer&
CGateContainer::operator=(const CGateContainer& aCGateContainer)
{
  if(this != &aCGateContainer) {
    CNamedItem::operator=(aCGateContainer);
    delete m_pGate;
    m_pGate = 0;
    if(aCGateContainer.m_pGate)
      m_pGate = aCGateContainer.m_pGate->clone();
  }
  return *this;
}
