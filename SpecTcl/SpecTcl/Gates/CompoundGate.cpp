//  CCompoundGate.cpp
// An abstract base class for a set of gates which 
// evaulate some boolean function on 
// a set of contained gates.  The member
// m_vConstituents is a vector of pointers to
// gate containers which represent the gates
// we must evaluate. 
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


#include "CompoundGate.h"                               
#include "GateListIterator.h"

static const char* Copyright = 
"CCompoundGate.cpp: Copyright 1999 NSCL, All rights reserved\n";

// Functions for class CCompoundGate
//////////////////////////////////////////////////////////////////////////
//  Function:
//    CCompoundGate(list<CGateContainer*> pGates)
//  Operation Type:
//    Constructor.
//
CCompoundGate::CCompoundGate(list<CGateContainer*> pGates) :
  m_vConstituents(pGates)
{
}
//////////////////////////////////////////////////////////////////////////
//
// Function:
//   CCompoundGate(vector<CGateContainer*> pGates)
// Operation Type:
//   Constructor
//
CCompoundGate::CCompoundGate(vector<CGateContainer*> pGates)
{
  vector<CGateContainer*>::iterator p;
  for(p = pGates.begin(); p != pGates.end(); p++) {
    CGateContainer* pGC = *p;
    AddGateBack(*pGC);
  }
}
//////////////////////////////////////////////////////////////////////////
//
// Function:
//   CCompoundGate(UInt_t nGates, CGateContainer** ppGates)
// Operation Type:
//   Constructor
//
CCompoundGate::CCompoundGate(UInt_t nGates, CGateContainer** ppGates)
{
  while(nGates) {
    AddGateBack(**ppGates);
    ppGates++;
    nGates--;
  }
}
//////////////////////////////////////////////////////////////////////////
//
//  Function:   
//    CConstituentIterator Begin (  )
//  Operation Type:
//     Selector
//
CConstituentIterator 
CCompoundGate::Begin() 
{
// Returns a constituent iterator representing the
// first gate in the sequence of gates stored in
// m_vConstituentList
//
// 

  CGateListIterator ai(m_vConstituents.begin());
  CConstituentIterator it(ai);
  return it;

}
//////////////////////////////////////////////////////////////////////////
//
//  Function:   
//    CConstituentIterator End (  )
//  Operation Type:
//     Selector
//
CConstituentIterator 
CCompoundGate::End() 
{
// Returns an iterator representing the end
// of iteration in the list of gates contained by
// m_vConstituents.
// Exceptions:  

  CGateListIterator ai(m_vConstituents.end());
  return CConstituentIterator(ai);

}
//////////////////////////////////////////////////////////////////////////
//
//  Function:   
//    UInt_t Size (  )
//  Operation Type:
//     Selector
//
UInt_t 
CCompoundGate::Size() 
{
// Returns the number of gates in the
// m_vConstituents gate list.
// Exceptions:  

  return m_vConstituents.size();
}
//////////////////////////////////////////////////////////////////////////
//
//  Function:   
//    std::string GetConstituent ( CConstituentIterator& rIterator )
//  Operation Type:
//     Selector
//
std::string 
CCompoundGate::GetConstituent(CConstituentIterator& rIterator) 
{
// Returns the textualized value of a gate
// pointed to by the constituent iterator.
//
// Formal Parameters:
//   CConstituentIterator& rIterator

  std::list<CGateContainer*>::iterator p = ConvertIterator(rIterator);
  return (*p)->getName();
  

}

////////////////////////////////////////////////////////////////////////
//
// Function:
//   void      AddGateFront(CGateContainer& rGate)
// Operation Type:
//    Incremental gate build-up.
//
void 
CCompoundGate::AddGateFront(CGateContainer& rGate)
{
  m_vConstituents.push_front(&rGate);
}
/////////////////////////////////////////////////////////////////////////
//
// Function:
//    void      AddGateBack(CGateContainer& rGate)
// Operation type:
//    Incremental Gate build-up.
// 
void 
CCompoundGate::AddGateBack(CGateContainer& rGate)
{
  m_vConstituents.push_back(&rGate);
}
/////////////////////////////////////////////////////////////////////////
//
// Function:
//    void      AddGate(CConstituentIterator& rSpot, CGateContainer& rGate)
// Operation Type:
//    Incremental Gate build-up
//
void
CCompoundGate::AddGate(CConstituentIterator& rSpot, CGateContainer& rGate)
{
  list<CGateContainer*>::iterator p = ConvertIterator(rSpot);
  m_vConstituents.insert(p, &rGate);
  
}
/////////////////////////////////////////////////////////////////////
//
// Function:
//    list<CGateContainer*>::iterator
//            ConvertIterator(CConstituentIterator& rIterator)
//
// Operation Type:
//    Utility
//
list<CGateContainer*>::iterator
CCompoundGate::ConvertIterator(CConstituentIterator& rIterator)
{
   CGateListIterator* pi = (CGateListIterator*)rIterator.getActualIterator();
   std::list<CGateContainer*>::iterator p = pi->getGates();
   return p;
}




