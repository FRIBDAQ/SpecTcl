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

//
// Include files:
//
#ifndef __HISTOTYPES_H
#include <histotypes.h>
#endif

class CAnalyzer;
class CBufferDecoder;
class CEvent;


class CEventProcessor      
{
  

public:
             //Default constructor alternative to compiler provided default constructor

  CEventProcessor ();
  virtual  ~ CEventProcessor ( );  //Destructor

  // It's up to the extender to decide if these should be implemented.
  
             //Copy Constructor 

  CEventProcessor (const CEventProcessor& aCEventProcessor );

             //Operator= Assignment Operator 

  CEventProcessor& operator= (const CEventProcessor& aCEventProcessor);
 
            //Operator== Equality Operator 

  int operator== (const CEventProcessor& aCEventProcessor) const;


public:

  virtual   Bool_t OnAttach (CAnalyzer& rAnalyzer) ; // Called on registration
  virtual   Bool_t OnBegin (CAnalyzer& rAnalyzer, 
			    CBufferDecoder& rDecoder)   ; // Begin Run
   virtual   Bool_t OnEnd (CAnalyzer& rAnalyzer, 
			   CBufferDecoder& rBuffer)   ; // End Run
  virtual   Bool_t OnPause (CAnalyzer& rAnalyzer, 
			    CBufferDecoder& rDecoder)   ; // Pause Run.
  virtual   Bool_t OnResume (CAnalyzer& rAnalyzer, 
			     CBufferDecoder& rDecoder)   ; // Resume Run
  virtual   Bool_t operator() (const Address_t pEvent, 
			       CEvent& rEvent, 
			       CAnalyzer& rAnalyzer, 
			       CBufferDecoder& rDecoder)   ; // Physics Event

};

#endif
