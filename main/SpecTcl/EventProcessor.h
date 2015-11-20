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

// Class: CEventProcessor            //ANSI C++
// File: EventProcessor.h
/*
  This is a base class for all elements in the analysis pipeline.  
  Elements in the pipeline can do whatever they want with the raw
  event and the unpacked event array.  They are registered into the
  analyzer as a sequential set of processors which are called in registration
  The base class makes no assumption about which of the run events
  is actually handled and is essentially a no-op for all member functions.
  This allows a user to write e.g. an event processor which only cares about
  things like the Begin Run event.

  BUGBUGBUG - Need to extend this class at some point to have
  entries for Documentation buffers.
  order for each event.
*/
// Author:
//   Ron Fox
//   NSCL
//   Michigan State University
//   East Lansing, MI 48824-1321
//   mailto:fox@nscl.msu.edu
//
// Copyright 

#ifndef __EVENTPROCESSOR_H  //Required for current class
#define __EVENTPROCESSOR_H

// Include files:

#ifndef __HISTOTYPES_H
#include <histotypes.h>
#endif

#ifndef __STL_STRING
#include <string>
#ifndef __STL_STRING
#define __STL_STRING
#endif
#endif

class CAnalyzer;
class CBufferDecoder;
class CEvent;

class CEventProcessor {
 public:
  // Constructors:
  CEventProcessor(); // Default Constructor.
  CEventProcessor(const CEventProcessor& aCEventProcessor); // Copy Constructor.
  virtual ~CEventProcessor(); //Destructor

  // Operators:
  CEventProcessor& operator= (const CEventProcessor& aCEventProcessor); // Assignment operator.
  int operator==(const CEventProcessor& aCEventProcessor) const; // Equality operator.
  int operator!=(const CEventProcessor& aCEventProcessor) const {
    return !(*this == aCEventProcessor);
  }
  virtual Bool_t operator()(const Address_t pEvent,
			    CEvent& rEvent,
			    CAnalyzer& rAnalyzer,
			    CBufferDecoder& rDecoder); // Physics Event.

  // Functions:
  virtual Bool_t OnAttach(CAnalyzer& rAnalyzer); // Called on registration.
  virtual Bool_t OnBegin(CAnalyzer& rAnalyzer,
			 CBufferDecoder& rDecoder); // Begin Run.
  virtual Bool_t OnEnd(CAnalyzer& rAnalyzer,
		       CBufferDecoder& rBuffer); // End Run.
  virtual Bool_t OnPause(CAnalyzer& rAnalyzer,
			 CBufferDecoder& rDecoder); // Pause Run.
  virtual Bool_t OnResume(CAnalyzer& rAnalyzer,
			  CBufferDecoder& rDecoder); // Resume Run.
  virtual Bool_t OnOther(UInt_t nType,
			 CAnalyzer& rAnalyzer,
			 CBufferDecoder& rDecoder); // Unrecognized buftype.

  virtual Bool_t OnEventSourceOpen(STD(string) name);
  virtual Bool_t OnEventSourceEOF();
};

#endif
