//  CAndGate.cpp
// Encapsulates a gate which consists of the AND of
// a set of gates. Note that the set of gates can be 
// in turn compound gates allowing arbitrary expressions
// to be built up.
// 
//  Evaluation takes advantage of short circuiting to
//  fail the gate if an early constituent gate fails.
// 

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


#include "AndGate.h"                               

static const char* Copyright = 
"CAndGate.cpp: Copyright 1999 NSCL, All rights reserved\n";

// Functions for class CAndGate

//////////////////////////////////////////////////////////////////////////
// Function:
//   CAndGate(list<CGateContainer*>& rGates)
// Operation Type:
//   Constructor.
//
CAndGate::CAndGate(list<CGateContainer*>& rGates) :
  CCompoundGate(rGates) 
{}

Bool_t
CAndGate::inGate(CEvent& rEvent, vector<UInt_t>&)
{
  std::list<CGateContainer*>& rGates(GetConstituents());
  std::list<CGateContainer*>::iterator i;
  for(i = rGates.begin(); i != rGates.end(); i++) {
    if(!((*i)->operator()(rEvent))) {
      return kfFALSE;
    }
  }
  return kfTRUE;
}

//////////////////////////////////////////////////////////////////////////
//
//  Function:   
//    CGate* clone (  )
//  Operation Type:
//     Construction
//
CGate* 
CAndGate::clone() 
{
// Constructs a gate which is a copy of the
// current gate dynamically allocated.
//

  return new CAndGate(*this);

}
//////////////////////////////////////////////////////////////////////////
//
//  Function:   
//    std::string Type (  )
//  Operation Type:
//     // Selector.
//
std::string 
CAndGate::Type() const
{
// Returns a string describing the type of the gate.
// in this case the single character '*' is returned.
// Exceptions:  

  return std::string("*");
}
