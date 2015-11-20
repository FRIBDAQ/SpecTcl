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

static const char* Copyright = "(C) Copyright Michigan State University 2008, All rights reserved";

// Class: CEventProcessor

////////////////////////// FILE_NAME.cpp /////////////////////////////////////////////////////
#include <config.h>
#include "EventProcessor.h"    				
#include <Analyzer.h>
#include <BufferDecoder.h>
#include <Event.h>

#ifdef HAVE_STD_NAMESPACE
using namespace std;
#endif

// Constructors:
CEventProcessor::CEventProcessor() {} // Default Constructor.
CEventProcessor::CEventProcessor(const CEventProcessor& aCEventProcessor) {} // Copy Constructor.
CEventProcessor::~CEventProcessor() {} // Destructor.

// Operators:
CEventProcessor& CEventProcessor::operator=(const CEventProcessor& aCEventProcessor) { // Assignment.
  return *this;
}

int CEventProcessor::operator==(const CEventProcessor& aCEventProcessor) const { // Equality.
  return kfFALSE;
}

// Functions for class CEventProcessor

//  Function: 	
//    Bool_t OnAttach(CAnalyzer& rAnalyzer) 
//  Operation Type:
//     Default method
/*  
    Purpose:

    Called once when the event processor is
    registered on the analyzer.  The default action
    is null.  If this member returns kfFALSE,
    the registration is aborted with an error message.

*/
Bool_t CEventProcessor::OnAttach(CAnalyzer& rAnalyzer) {
  return kfTRUE;
}

//  Function: 	
//    Bool_t OnBegin(CAnalyzer& rAnalyzer, CBufferDecoder& rDecoder) 
//  Operation Type:
//     Default Behavior
/*  
    Purpose: 	

    Called when a begin run buffer/event is received.
    If kfFALSE is returned, the pipeline is silently aborted.

*/
Bool_t CEventProcessor::OnBegin(CAnalyzer& rAnalyzer, CBufferDecoder& rDecoder) {
  return kfTRUE;
}

//  Function: 	
//    Bool_t OnEnd(CAnalyzer& rAnalyzer, CBufferDecoder& rBuffer) 
//  Operation Type:
//     Default behavior
/*  
    Purpose: 	

    Called when an end of run event/buffer is received.
    If kfFALSE is returned, the processing pipeline is 
    silently aborted.  The default implementation is a
    no-op returning kfTRUE

*/
Bool_t CEventProcessor::OnEnd(CAnalyzer& rAnalyzer, CBufferDecoder& rBuffer) {
  return kfTRUE;
}

//  Function:
//    Bool_t OnPause(CAnalyzer& rAnalyzer, CBufferDecoder& rDecoder)
//  Operation Type:
//     Default Behavior
/*
    Purpose:

    Called when a pause run event/buffer is received.
    Returning kfFALSE will silently abort the remainder of the
    event pipeline.  The default is an no-op which returns kfTRUE

*/
Bool_t CEventProcessor::OnPause(CAnalyzer& rAnalyzer, CBufferDecoder& rDecoder) {
  return kfTRUE;
}

//  Function:
//    Bool_t OnResume(CAnalyzer& rAnalyzer, CBufferDecoder& rDecoder)
//  Operation Type:
//     Default Behavior
/*
    Purpose:

    Called when a resume run event/buffer is received
    If this function returns kfFALSE, the remainder of the
    event pipeline is silently aborted.  The default
    implementation is a no-op which returns kfTRUE.

*/
Bool_t CEventProcessor::OnResume(CAnalyzer& rAnalyzer, CBufferDecoder& rDecoder) {
  return kfTRUE;
}

//  Function:
//    Bool_t operator()(const Address_t pEvent, CEvent& rEvent,
//                     CAnzlyer& rAnalyzer, CBufferDecoder& rDecoder)
//  Operation Type:
//     Base class behavior
/*
    Purpose:

    This member is called whenever a physics event is
    received.  At least one of the processors in the processing
    pipeline must call the analyzer's SetEventSize() member function.
    This member informs the analyzer how many bytes are in the raw event.
    Other useful members involved in that are:
    IncrementEventSize(int nSize=2)      - Adds to the current event size.
    GetEventSize()                                 - Retrieves the current event size.

    If the function returns kfFALSE, the entire event processing pipeline is
    aborted and the unpacked event being construction is not sent on
    to the event sink.

*/
Bool_t CEventProcessor::operator()(const Address_t pEvent,
				   CEvent& rEvent,
				   CAnalyzer& rAnalyzer,
				   CBufferDecoder& rDecoder) {
  return kfTRUE;
}
/*!
   Called whenever an unrecognized buffer type is encountered.
   This is done to allow the user to take application specific 
   action on these buffer types.
   \param nType     (UInt_t [in]):
      Type of buffer received.
   \param rAnalyzer (CAnalyzer& [in]):
      Reference to the analyzer that is calling us.
   \param rDecoder (CBufferDecoder& [in]):
      Reference to the  buffer decoder that understands the
      overall buffer structure.

   \return 
   - kfTRUE -- If the buffer was successfully processed.
   - kfFALSE - If the buffer was not successfully processed.
              Note that if the processor does not understand the
	      type of of the buffer, it should return kfTRUE
	      since some other processor in the pipeline might.
	      If it understands the buffer type but the format is
	      weird/wrong etc.  Then it's appropriate to return
	      kfFALSE.
*/
Bool_t
CEventProcessor::OnOther(UInt_t nType,
			 CAnalyzer& rAnalyzer,
			 CBufferDecoder& rDecoder)
{
  return kfTRUE;
}

/*!
   Called whenever a new event source is opened.
   \param name  : std::string
     The name of the event source. This is of the form:
     type:connection  where type: is a connection type and is one of:
     - File: event source is a file the connection is the path to the file.
     - Tape: event source is a tape drive.  The connection is the tape drive
             name (probably nearly obsolete).
     - Pipe from:  event soure is a pipe, the connection is the command that is
             generating the data.
     - Test Source - the data source is a test data source and has no connection
             identifier.
     - Null - The data source is one that is always at end of file.

     \return Bool_t
     \retval kfTRUE - continue processing.
     \retval kfFALSE  - abort event processing pipeline.

     The default action, implemented by this member is a successful no-op.

*/
Bool_t
CEventProcessor::OnEventSourceOpen(string name)
{
  return kfTRUE;
}

/*!
   Called when an event source has it the end of file.
     \return Bool_t
     \retval kfTRUE - continue processing.
     \retval kfFALSE  - abort event processing pipeline.

     The default action, implemented by this member is a successful no-op.
*/
Bool_t
CEventProcessor::OnEventSourceEOF()
{
  return kfTRUE;
}
/**
 * OnInitialize
 *
 * Called when SpecTcl initialization is complete.  This allows event
 * processors to access stuff that may have been set up in OnAttach of other
 * event processors (e.g. tree parameters/variables) or in the SpecTclInit.tcvl
 * and SpecTclRC.tcl scripts.
 *    @return Bool_t
 *    @retval kfTRUE - continue processing.
 *    @retval kfFALSE  - abort event processing pipeline.
 *
 *    The default action, implemented by this member is a successful no-op.
 */
Bool_t
CEventProcessor::OnInitialize()
{
    return kfTRUE;
}
