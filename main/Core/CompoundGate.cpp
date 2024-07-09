
/*
    This software is Copyright by the Board of Trustees of Michigan
    State University (c) Copyright 2024

    You may use this software under the terms of the GNU public license
    (GPL).  The terms of this license are described at:

     http://www.gnu.org/licenses/gpl.txt

     Authors:
             Ron F.
             Simon G.
             Giordano C.
             Aaron C.
	     NSCL
	     Michigan State University
	     East Lansing, MI 48824-1321
*/
//
// Header Files:
//

#include <config.h>
#include "CompoundGate.h"                               
#include "GateListIterator.h"
#include <algorithm>

#ifdef HAVE_STD_NAMESPACE
using namespace std;
#endif

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
/**
 * getDependentGates
 *    @return std::vector<CGateContainer*> - the dependent gates.
 * 
 */
std::vector<CGateContainer*> 
CCompoundGate::getDependentGates() {
  std::vector<CGateContainer*> result(m_vConstituents.begin(), m_vConstituents.end());
  return result;
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


/*!
   Reset the gate cache recusively. the base class reset is called in order to 
   reset our cache, and the Reset for each of our constituents is called
   in order to recursively reset their caches.
*/

static void ResetGate(CGateContainer* pGate) {
  CGateContainer& rGate(*pGate);
  rGate->RecursiveReset();
}

void
CCompoundGate::RecursiveReset()
{
  CGate::RecursiveReset();	// Clear our cache flag.
  
  for_each(m_vConstituents.begin(), m_vConstituents.end(),
	  ResetGate);
}
