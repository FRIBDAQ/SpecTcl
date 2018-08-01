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


//  CEventUnpacker.h:
//
//    This file defines the CEventUnpacker class.
//
// Author:
//    Ron Fox
//    NSCL
//    Michigan State University
//    East Lansing, MI 48824-1321
//    mailto:fox@nscl.msu.edu
//
//  Copyright 1999 NSCL, All Rights Reserved.
//
/////////////////////////////////////////////////////////////

#ifndef EVENTUNPACKER_H  //Required for current class
#define EVENTUNPACKER_H

#include <histotypes.h>

// Forward Class references:
class CAnalyzer;
class CBufferDecoder;
class CEvent;

// The event unpacker abstract base class.
class CEventUnpacker {
 public:
  CEventUnpacker() {} // Default constructor.
  virtual ~CEventUnpacker() {} //Destructor.

  // Not necessary, but makes it less thinking for derivers.
  CEventUnpacker(const CEventUnpacker& aCEventUnpacker) {}

  // Also not necessary but reduces thinking required by derivers.
  CEventUnpacker& operator=(const CEventUnpacker& aCEventUnpacker) { 
    return *this;
  }

  //Operator== Equality Operator also not necessary, but reduces
  //           thinking required by derivers.  Default is to 
  //           return true since any abstract base object is just
  //           as equal to any other.
  int operator==(const CEventUnpacker& aCEventUnpacker) {
    return (1==1); // TRUE regardless of how it's defined.
  }

  //  The whole point of deriving from CEventUnpacker is to supply
  //  an event unpacking function.  Therefore this is a pure virtual.
  virtual UInt_t operator() (const Address_t pEvent,
			     CEvent& rEvent,
			     CAnalyzer& rAnalyzer,
			     CBufferDecoder& rDecoder) = 0;

  // These virtual functions default to no-op so that a deriver doesn't
  // have to supply it.
  virtual void OnAttach(CAnalyzer& rAnalyzer) {}
  virtual void OnDetach(CAnalyzer& rAnalyzer) {}
};
#endif
