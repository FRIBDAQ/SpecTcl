//  CGate.cpp
// Abstract base class for all gates.
// Gates are composed of 'constituents'
// a constituent is something with a textual
// representation.  This abstraction allows uniform
// treatment of raw gates (whose constituents are 
// points in parameter space) and compound gates 
// (whose constituents are other gates).

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


#include "Gate.h"                               

static const char* Copyright = 
"Gate.cpp: Copyright 1999 NSCL, All rights reserved\n";

// Functions for class CGate


//////////////////////////////////////////////////////////////////////////
//
//  Function:   
//    Bool_t operator() ( CEvent& rEvent )
//  Operation Type:
//     Overridable.
//
Bool_t 
CGate::operator()(CEvent& rEvent) 
{
  // assert's that the m_fChecked members is
  // kfTRUE  if so, then returns the value of
  // m_fCachedValue.
  // inGate evaluate the gate for a given parameter,
  // and vNoParams is an empty vector, necessary for
  // the implementation of gamma gates.
  // The acceptance of a CEvent parameter
  // allows subclasses to be written with code like:
  //
  //  if(!wasChecked() {
  //      check the gate..[using Event]....
  //      Set(value)
  //  }
  //  return operator()(Event);
  //
  // Formal Parameters:
  //     CEvent& rEvent:
  //         Event for which the gate is being checked.
  //
  // Exceptions:  

  if(!wasChecked()) {
    Set(inGate(rEvent));
  }
  
  return getCachedValue();
}
