//  COrGate.cpp
// Encapsulates the OR of a set of gates.
// Note that the gates can themselves be compound allowing
// for arbitrary boolean expressions.
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

#include "OrGate.h"                               

static const char* Copyright = 
"COrGate.cpp: Copyright 1999 NSCL, All rights reserved\n";

// Functions for class COrGate
/////////////////////////////////////////////////////////////////////////
//  Function:
//     COrGate(list<CGateContainer*>& rGates)
// Operation Type:
//    Construtor
//
COrGate::COrGate(list<CGateContainer*>& rGates) :
  CCompoundGate(rGates) 
{}

//////////////////////////////////////////////////////////////////////////
//
//  Function:   
//    Bool_t inGate ( CEvent& rEvent, vector<UInt_t>& Params )
//  Operation Type:
//     Evaluation
//
Bool_t
COrGate::inGate(CEvent& rEvent, vector<UInt_t>&)
// Evaluates the gate.  Short circuiting is done
// 
// Formal Parameters:
//     CEvent& rEvent:
//        The event on which the gate is being
//        evaluated.
//     vector<UInt_t>& Params
//        The vector of parameters in the gate (empty)
// Exceptions:  
{
  std::list<CGateContainer*>& rGates(GetConstituents());
  std::list<CGateContainer*>::iterator  i;
  for(i = rGates.begin(); i != rGates.end(); i++) {
    if((*i)->operator()(rEvent)) {     // Any gate makes the whole gate
      return kfTRUE;
    }
  }
  return kfFALSE;
}
//////////////////////////////////////////////////////////////////////////
//
//  Function:   
//    CGate* clone (  )
//  Operation Type:
//     Selector.
//
CGate* 
COrGate::clone() 
{
// Returns a pointer to a dynamically allocated copy of this
// gate.

  return new COrGate(*this);

}
//////////////////////////////////////////////////////////////////////////
//
//  Function:   
//    std::string Type (  )
//  Operation Type:
//     Selector.
//
std::string 
COrGate::Type() const
{
// Returns a string indicating the gate type.
// In this case, the single character "+"
//
// Exceptions:  

  return std::string("+");
}
