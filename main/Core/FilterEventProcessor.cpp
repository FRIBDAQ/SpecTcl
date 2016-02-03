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

#include <config.h>
#include "FilterEventProcessor.h"
#include <buftypes.h>
#include <Globals.h>
#include <BufferDecoder.h>
#include <Event.h>
#include <Analyzer.h>
#include <Histogrammer.h>
#include <Parameter.h>
#include <EventFormatError.h>
#include <TCLAnalyzer.h>
#include <string.h>
#include <stdint.h>

#ifdef HAVE_STD_NAMESPACE
using namespace std;
#endif


/*!
    Construct a filter event processor.  For now this is null.
*/
CFilterEventProcessor::CFilterEventProcessor()
{}
/*!
   Copy construct a filter event processor.  This just
   requires a copy of the parameter map be constructed.
   \param rhs (const CFilterEventProcessor& rhs [in]):
      The source from which this is constructed.
*/
CFilterEventProcessor::CFilterEventProcessor(const CFilterEventProcessor& rhs) :
  CEventProcessor(rhs),
  m_ParameterMap(rhs.m_ParameterMap)
{}
/*!
   Assignment: just copy the parameter map.
   \param rhs (const CFilterEventProcessor& rhs [in]):
     The source of the assignment.
   \return CFilterEventProcessor&
     *this
*/
CFilterEventProcessor&
CFilterEventProcessor::operator=(const CFilterEventProcessor& rhs)
{
  if(this != &rhs) {
    CEventProcessor::operator==(rhs);
    m_ParameterMap = rhs.m_ParameterMap;
  }
  return *this;
}
/*!
   Equality comparison.  Returns true if the the rhs object is
   equal to us.
   \param rhs (const CEventProcessor& [in]):
     The object we compare to.
   \return int
       non-zero if the objects are equal.
*/
int
CFilterEventProcessor::operator==(const CFilterEventProcessor& rhs)
{
  return (CEventProcessor::operator==(rhs)             &&
	  (m_ParameterMap == rhs.m_ParameterMap));
}
/*!
   Process non event data.  For us, the data must be
   of type PARAMDESCRIP.  The first part of the event must
   be a null terminated string that contains the value:
   "header".
   If any of these conditions are not met, this buffer is not
   processsed (silently as another event processor may know how
   to handle these now or in the future).
   \param nType      (UInt_t [in]):
      The type of buffer we're dealing with (we expect only
      PARAMDESCRIP buffers.
   \param rAnalyzer  (CAnalyzer& [in]):
      Then analyzer that called us.
   \param rDecoder   (CBufferDecoder& [in]):
      The decoder that owns the buffer we are processing.

   \return Bool_t
     kfTRUE unconditionally.

   \note We assume the data in filtered event buffers was 
   translated from some system independent storage format and 
   therefore we don't bother with translating buffer pointers.
   
   See CFilterBufferDecoder for the format of the body of this
   buffer.
*/
Bool_t
CFilterEventProcessor::OnOther(UInt_t nType,
			       CAnalyzer& rAnalyzer,
			       CBufferDecoder& rDecoder)
{
  // Get a cursor into the bufer:

  void* pBuffer = rDecoder.getBody();

  // Decode the header string:

  string id;
  pBuffer = GetString(pBuffer, id);
  if(id == string("header")) {

    CHistogrammer* pHistogrammer = (CHistogrammer*)gpEventSink;

    // If we have an existing parameter map trash it:
    
    if(!m_ParameterMap.empty()) {
      m_ParameterMap.erase(m_ParameterMap.begin(), 
			   m_ParameterMap.end());
    }
    // Retrieve the number of parameters:

    int numParams;
    pBuffer = GetInt(pBuffer, numParams);

    // For each parameter, get it, figure out if it maps and
    // make the appropriate entry in the map array (-1 means does
    // not map).
    for(int i=0; i < numParams; i++) {
      string name;
      pBuffer = GetString(pBuffer, name);
      CParameter* pParam = pHistogrammer->FindParameter(name);
      if(pParam) {
	m_ParameterMap.push_back(pParam->getNumber());
      } else {
	m_ParameterMap.push_back(-1);
      }
    }
  }

  return kfTRUE;
}
/*!
  Decode a physics event.  We assume again that translating pointers
  will not be necessary.  The format of a physics event is described
  in CFilterBufferDecoder.  If at any point, we don't have an event
  that starts with an "event" id, we will throw an event format
  exception as this will indicate that we are not processing
  a filtered data stream.
  \param pEvent    (Address_t [in]):
      A pointer to the raw event.
  \param rEvent    (CEvent& [out]):
      The event vector that we are filling.
  \param rAnalyzer (CAnalyzer& [in]):
      The analyzer that called us.
  \param rDecoder  (CBufferDecoder& [in]):
      The buffer decoder that owns the buffer this event is in.

  \return Bool_t
  kfTRUE unless an exception was thrown.

  \throw CEventFormatError
    If the header is not "event"
  \throw CEventFormatError
    If the bitmask ever has a bit set for a parameter id past the
    range covered by our map.
  \throw CEventFormatError
    If our map is empty.

  \note Elements of the map with value -1 dont' have corresponding
  SpecTcl parameters and are just silently discared.  This is 
  considered a legal case.

  \note Side effect:  rAnalyzer.SetEventSize() is called in order
  to let the analyzer know how many bytes are in this event.
*/
Bool_t
CFilterEventProcessor::operator()(const Address_t pEvent,
				  CEvent& rEvent,
				  CAnalyzer& rAnalyzer,
				  CBufferDecoder& rDecoder)
{
  Address_t pHere(pEvent);	// Retain starting pointer.

  // Convert the analyzer to a CTclAnalyzer in order to be able
  // to set the event size later:

  CTclAnalyzer& rAna((CTclAnalyzer&)rAnalyzer);
  
  // The header needs to be the string "event" otherwise toss an
  // error:

  string header;

  pHere = GetString(pHere, header);
  if(header != string("event")) {
    throw CEventFormatError((int)CEventFormatError::knPacketIdMismatch,
			    "Decoding filtered event header",
			    (UInt_t*)pEvent, 8,
			    0, 0);

  }
  // Throw if the parameter map is empty:

  if(m_ParameterMap.empty()) {
    throw CEventFormatError((int)CEventFormatError::knEmptyParameterMap,
			    "Checking size of parameter map",
			    (UInt_t*)pEvent, 8, 0,0);
  }
  // From the size of the parameter map, we can deduce the
  // number of bitmask entries.

  int nParams = m_ParameterMap.size();
  int nMasks  = ((nParams + sizeof(unsigned)*8 - 1)/(
		 sizeof(unsigned)*8));

  // We build up the placement vector from the bitmask.
  // the placement vector determines which parameter gets each of
  // the floats following the bitmask array.
  //
  int ParamNo  = 0;		// Filter parameter number.
  vector<int>  Placement;	// The placement array.

  for(int i =0; i < nMasks; i++) {
    int mask;
    pHere = GetInt(pHere, mask);
    for(int m = 1; m != 0; m = m << 1) {
      if(m & mask) {
	if(ParamNo < nParams) {
	  Placement.push_back(m_ParameterMap[ParamNo]);
	}
	else {			// Too high a bit!!!
	  throw 
	    CEventFormatError((int)CEventFormatError::knBadPacketContents,
			      "Bit mask Parameter id too big",
			      (UInt_t*)pEvent, 
			      15,
			      (uint8_t*)pHere - (uint8_t*)pEvent);
	  
	}
      }
      ParamNo++;
    }
  
  }
  // Now we can put the parameters of the event in their places:

  int nPars = Placement.size();
  for(int i =0; i < nPars; i++) {
    int nTarget = Placement[i];
    double parameter;
    pHere = GetFloat(pHere, parameter);
    if(nTarget >= 0) rEvent[nTarget] = parameter;

  }
  // Calcluate the size of the event:

  rAna.SetEventSize((uint8_t*)pHere - (uint8_t*)pEvent);

  return kfTRUE;
  
}
/*!
   Pull a string out of a buffer and update the pointer to the
   buffer.  A string is a null terminated series of bytes.

   \param p       (void* [in]):
      Pointer to the null terminated string.
   \param result  (string& [out]):
     Reference to the string to fill.  The string in the buffer
     is \em appended to the result string.
   \return void*
     Updated pointer.
*/
void*
CFilterEventProcessor::GetString(void* p, string& result)
{
  char* pc((char*)p);
  while(*pc) {
    result += *pc;
    pc++;
  }
  pc++;
  return (void*)pc;
}
/*!
   Get an integer value from an input buffer.
   \param p         (void* [in]):
        Pointer to the buffer from which the data are grabbed.
   \param result    (int& [out):
        Reference to the output integer.
   \return void*
        Pointer updated to past the input integer.

*/
void*
CFilterEventProcessor::GetInt(void* p, Int_t& result)
{
  memcpy(&result, p, sizeof(Int_t));
  
  return (void*)((uint8_t*)p + sizeof(Int_t));
}
/*!
  Get a float value from an input buffer.
  \param p      (void* [in]):
     Pointer to the buffer from which the data are grabbed.
  \param result (Float_t& [out]):
     Reference to the result float.
  \return void* 
     Updated input pointer.
*/
void*
CFilterEventProcessor::GetFloat(void* p, DFloat_t& result)
{
  memcpy(&result, p, sizeof(DFloat_t));
  return (void*)((uint8_t*)p + sizeof(DFloat_t));
}
