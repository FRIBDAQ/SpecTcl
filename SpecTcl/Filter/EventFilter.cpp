
/*
  EventFilter.cpp
*/

// Copyright Notice.
static const char* Copyright = 
  "EventFilter.cpp: Copyright 2002 NSCL, All rights reserved\n";

// Header Files.
#include "EventFilter.h"
#include "Histogrammer.h"
#include "Parameter.h"
#include "Event.h"
#include "EventList.h"
#include "CXdrOutputStream.h"
#include "Globals.h"
#include "DictionaryException.h"

#include <vector>
#include <string>
#include <string.h>
#include <stdlib.h>


static const char* headerlabel = "header";
static const char* eventlabel  = "event";

// Constructors.

/*! 
   Construct an event filter:
   -  Enable is set to false.
   -  The file name is set to a 'suitable default'.
   -  The output stream is set to null initially.
   -  Parameter names and id arrays are initially clear (this is done by
      the vector contructors).
      
   Output stream binding is done at enable time.
*/
CEventFilter::CEventFilter() :
  m_fEnabled(kfFALSE),
  m_sFileName(CEventFilter::DefaultFilterFilename()), // Initialized to default.
  m_pOutputEventStream((CXdrOutputStream*)kpNULL)
{

}
/*!
   Construct an event filter:
   - Enable is set false.
   - The filename is set to the user specified filename.
   - The output stream is set to null initially.
   - Parameter names and ID arrays are initially clear (done by vector
     constructors).

   Output stream binding is done at enable time.

   \param rFilename (in):
      The name of the file to use for the output event stream.
*/
CEventFilter::CEventFilter(string& rFileName) :
  m_fEnabled(kfFALSE),
  m_sFileName(rFileName),
  m_pOutputEventStream((CXdrOutputStream*)kpNULL)
{
}

/*!
   Destroy an output event stream.  The following must be done:
    - Any existing output event file must be destroyed.

   note that destruction implies file flush and close.
*/
CEventFilter::~CEventFilter() {
  if(m_pOutputEventStream) {
      
     delete m_pOutputEventStream;
  }
}


/*!
   Enable the filter.  This does  a few things:
   - Translate parameters from names to ids.
   - Open the output filter file.
   - Set the enable flag true.
   
   \note If the filter is enabled already, it is disabled to close
         any existing output file etc.

   \note Parameter names with no mapping to a parameter id are
      silently discarded from the output stream, but remain in the
      list of named parameters in case they re-appear later in the
      lifetime of SpecTcl.
         
*/
void 
CEventFilter::Enable() {

   if(m_fEnabled) {
     Disable();               // Close any existing output file and

   }                          // ensure we're disabled on failure.

   NamesToIds();              // Translate the names.
   m_pOutputEventStream = new CXdrOutputStream(m_sFileName);
   FormatEventDescription();  // First record of each filter file.
   m_fEnabled = kfTRUE;
}

/*!
   Disable the filter.  This involves:
   - Destroying any existing filter file stream.
   - Setting the filter file stream pointer to null.
   - Setting the enable flag false.

*/
void 
CEventFilter::Disable() { // Must be consecutively callable.
   delete m_pOutputEventStream;              // No-OP if pointer is already null
   m_pOutputEventStream = (CXdrOutputStream*)kpNULL;  // Mark no stream.
   m_fEnabled = kfFALSE;                     // We're disabled.
}

/*!
   Change the name of the output stream.  
   If the filter is disable, this is just a matter of setting
   the appropriate member function.  Otherwise, this means
   that the active filter stream will be re-directed to another
   file target.  
*/
void 
CEventFilter::setFileName(string& rFileName) {
   if(m_fEnabled) {
      Disable();              // Close the old stream.
   }
   m_sFileName = rFileName;	// Set the new filename.

}

/*!
   Function call operator with a list of events.
   This overridable member provides the default behavior
   for a filter on a list of events.  For each event in the
   event list, the single event function call operator is called.
   
   \param rEvents (in)
     The list of events to process.  Note that the event list is usually
     a fixed size entity that is terminated by a null pointer to an event.
*/
void
CEventFilter::operator()(CEventList& rEvents) 
{
   CEventListIterator i;
   CEventListIterator e;
   for(i = rEvents.begin(); i != e; i++) {
      CEvent* pEvent = *i;
      if(pEvent) {
         operator()(*pEvent);
      } else {
	break;
      }
   }
}
/*!
   Function Call operator.  This function call operator processes a single event.
   Processing is simple, 
   - Check the filter condition (CheckCondition).
   - If the filter condition is true, format the output event (FormatOutputEvent).

   \note  For this ABC, CheckCondition is pure virtual.  There is, however
     a concept of a default output event format, which is implemented by our
     FormatOutputEvent member function.

   \param rEvent (in):
      Reference to the event to process.
*/
void
CEventFilter::operator()(CEvent& rEvent)
{
   if(CheckCondition(rEvent)) {
      FormatOutputEvent(rEvent);
   }
}
/*!
    Format the description of an event.  This overridable interacts with
   the output event stream and produces the event description record.
   The event Description record describes the contents of events in the output
   file.  This record can be used to drive a general purpose unpacker for
   filtered data.
     Note that we can only format the information for parameters that exist.
   The format of a header is the following:

   \verbatim
      +---------------------+
      | "header"            |
      +---------------------+
      | NumParams           |
      +---------------------+
      | Name string(1)      |
      +---------------------+
      | ...                 |
      +---------------------+
      |Name string(NumParams| 
      +---------------------+
   \endverbatim
*/
void
CEventFilter::FormatEventDescription()
{
   if(!m_pOutputEventStream) return;     // No stream so give up.
   vector<string> Names = IdsToNames();  // Get names for ids that exist.
   *m_pOutputEventStream << headerlabel; // Indicate this is a header.
   *m_pOutputEventStream  << (Names.size());
   for(int i =0; i < Names.size(); i++) {
      *m_pOutputEventStream << (Names[i]); // The name to match with inbound.
   }  
}
/*!
   Format an event into the output stream.  This overridable interacts with the
   output event stream and produces an event record.  The Event record
   contains a single event.  It has the following format:

   \verbatim
      +-------------------------+
      |   "event"               |
      +-------------------------+
      |NumParams bits of bitmask|
      +-------------------------+
      |Param for lowest setbit  |
      +-------------------------+
      |        ...              |
      +-------------------------+
      |Param for highest setbit |
      +-------------------------+

   \endverbatim

   \param rEvent (in):
      Reference to the event to output.
*/
void
CEventFilter::FormatOutputEvent(CEvent& rEvent)
{
   if(!m_pOutputEventStream) return;
   
   int nParams = m_vParameterIds.size();
   int nBitmaskwords = ((nParams + sizeof(unsigned)*8 - 1) /
			(sizeof(unsigned)*8)); // Assumes 8 bits/byte
   unsigned Bitmask[nBitmaskwords];

   for(int i = 0; i < nBitmaskwords; i++) {
     Bitmask[i] = 0;
   }
   
   // Figure out the bit mask:  A bit is set for each valid parameter:
   
   int nValid = 0;
   for(int i =0; i < nParams; i++) {
      if(rEvent[m_vParameterIds[i]].isValid()) {
         setBit(Bitmask, i);
         nValid++;
      }
   }
   // Declare required freespace to allow the output stream to close:
   // the buffer if this event doesn't fit.
   
   size_t intsize   = m_pOutputEventStream->sizeofInt();
   size_t floatsize = m_pOutputEventStream->sizeofFloat();
   size_t hdrsize   = m_pOutputEventStream->sizeofString(eventlabel);


   m_pOutputEventStream->Require(nBitmaskwords*intsize +
                                 nValid*floatsize       +
                                 hdrsize);
   
   // Write the header:
   
   *m_pOutputEventStream << (eventlabel);
   
   // Write the bitmask:
   
   for(int i =0; i < nBitmaskwords; i++) {
      *m_pOutputEventStream << Bitmask[i];
   }
   
   // Write the valid parameters:
   
   for(int i =0; i < nParams; i++) {
      if(rEvent[m_vParameterIds[i]].isValid()) {
         *m_pOutputEventStream << rEvent[m_vParameterIds[i]];
      }
   }
}
/*!
   Return the default name of a filter file.  This is ~/filter.flt
   Since we are not a shell, ~ must be obtained by translating HOME
   if HOME doesn't translate, we use . instead.

   \return string  containing the default filename.
*/
string
CEventFilter::DefaultFilterFilename()
{
   string Filename;
   char* pHome = getenv("HOME");
   if(pHome) {
      Filename = pHome;
   } else {
      Filename = ".";
   }
   Filename += "/filter.flt";
   return Filename;
}
/*!
  Translates parameter names to parameter ids. 
   - Clear the m_vParameterIds member.
   - For each parameter in m_vParameterNames that is valid, add it's id to
     m_vParameterIds.  Validity means in this case that a parameter by this
     name exists in the histogrammer's parameter dictionary.
*/
void
CEventFilter::NamesToIds()
{
   CHistogrammer* pHistogrammer = (CHistogrammer*)gpEventSink;
   if(!m_vParameterIds.empty()) m_vParameterIds.clear();
   
   int nParams = m_vParameterNames.size();
   for(int i = 0; i < nParams; i++) {
      CParameter* pParam = pHistogrammer->FindParameter(m_vParameterNames[i]);
      if(pParam) {
         m_vParameterIds.push_back(pParam->getNumber());
      }
   }
}
/*!
   Returns a string of parameter names that corresponds to the set of 
   parameter ids in m_vParameterIds this is not the same as m_vParameterNames
   as that vector is allowed to have elements that are deleted parameters.
   Note that it is an error for the backward translation to fail, and this
   results in an exception.

   \return vector<string>  
      The vector of parameter names corresponding to the
      ids in the m_vParameterIds member.

   \throw CDictionaryException which will have the reasdon knNoSuchid.

*/
vector<string>
CEventFilter::IdsToNames() throw (CDictionaryException)
{
   CHistogrammer* pHistogrammer = (CHistogrammer*)gpEventSink;
   vector<string> names;
   for(int i = 0; i < m_vParameterIds.size(); i++) {
      CParameter* pParam = pHistogrammer->FindParameter(m_vParameterIds[i]);
      if(!pParam) {
         throw CDictionaryException(CDictionaryException::knNoSuchId,
                                    "CEventFilter::IdsToNames id -> name xlation",
                                    "-unknown-");
      }
      names.push_back(pParam->getName());
   }
   
   return names;
}
/*!
   Set a bit in a bit vector.  The bit is assumed to fit in the vector.
*/
inline void
CEventFilter::setBit(unsigned* bits, unsigned offset)
{
   int element = offset/sizeof(unsigned);
   int mask    = 1 << (offset % sizeof(unsigned));
   bits[element] |= mask;
}
/*!
   Set the parameter names:
   - Replace the vector m_vParameterNames 
   Note that this only takes effect when the filter is enabled
   next time.
   \param names (const vector<string>& in):
      Set of parameter names.
*/
void
CEventFilter::setParameterNames(const vector<string>& rNames)
{
  m_vParameterNames = rNames;
}
