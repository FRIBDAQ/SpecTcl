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

static const char* Copyright =
  "(C) Copyright Michigan State University 2007, All rights reserved";

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
#include <config.h>
#include "Gate.h"                               

#ifdef HAVE_STD_NAMESPACE
using namespace std;
#endif
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

//
// Recursive reset for simple gates is just a cache
// invalidation.  More complex gates  will override this.
//
void
CGate::RecursiveReset()
{
	m_fChecked = kfFALSE;
}



/*!
    Returns true if the gate caches its value... false otherwise.
    this is used to restrict the set of gates cleared for
    each event to only those that need it (e.g not T or F gates
    which are, after all constant).
    Default is a gate that caches.
*/
Bool_t
CGate::caches()
{
  return kfTRUE;
}
