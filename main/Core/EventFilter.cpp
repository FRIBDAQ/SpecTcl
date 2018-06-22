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


/*
  EventFilter.cpp
*/

// Copyright Notice.
static const char* Copyright = 
  "EventFilter.cpp: Copyright 2002 NSCL, All rights reserved\n";

// Header Files.
#include <config.h>
#include "EventFilter.h"
#include "Histogrammer.h"
#include "Parameter.h"
#include "Event.h"
#include "EventList.h"
#include "CFilterOutputStage.h"
#include "Globals.h"
#include "StateException.h"
#include "DictionaryException.h"

#include <vector>
#include <string>
#include <string.h>
#include <stdlib.h>

#include <iostream>

#ifdef HAVE_STD_NAMESPACE
using namespace std;
#endif


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
  m_pOutput(0)
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
  m_pOutput(0)
{
}

/*!
   Destroy an output event stream.  The following must be done:
    - Any existing output event file must be destroyed.

   note that destruction implies file flush and close.
*/
CEventFilter::~CEventFilter() {
     delete m_pOutput;
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
  // The format of the event stream must have been set (m_pOutput must not be null):

  if (!m_pOutput) {
    throw CStateException("not-set", "set",
			  "CEventFilter::Enable needs to have the formatter set");
  }

   if(m_fEnabled) {
     Disable();               // Close any existing output file and
   }                          // ensure we're disabled on failure.


   NamesToIds();              // Translate the names.
   m_pOutput->open(m_sFileName);
   m_pOutput->DescribeEvent(m_vParameterNames, m_vParameterIds);

   m_fEnabled = kfTRUE;
}

/*!
   Disable the filter.  This involves:
   - Closing the output file
   - Setting the enable flag false.

*/
void 
CEventFilter::Disable() { // Must be consecutively callable.
  if (!m_pOutput) {
    throw CStateException("not-set", "set",
			  "CEventFilter::Enable needs to have formatter set");
  }
  m_pOutput->close();
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
  Set the output format.. this can only be done when the filter
  is disabled.  Existing filter output stages will be destroyed.
  \param format - new output stage formatter.
  \throw CStateException  if the filter is enabled.
*/
void
CEventFilter::setOutputFormat(CFilterOutputStage* format)
{
  if (m_fEnabled) {
    throw CStateException("enabled", "disabled",
			  "CEventFileter::setOutputFormat - can only be done when filter is disabled");
  }
  delete m_pOutput;
  m_pOutput = format;
  m_pOutput->onAttach(*this);

}
/**
 *  setOutputStream:
 *  
 *  Alias for setOutputFormat:
 */
void
CEventFilter::setOutputStream(CFilterOutputStage* str)
{
     setOutputFormat(str);
}
/*!
  Return the output format .. if non is attached, the value "--" is returned.
*/
string
CEventFilter::outputFormat() const
{
  if (m_pOutput) {
    return m_pOutput->type();
  }
  else {
    return string("--");
  }
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

  if (m_fEnabled) {		// Don't execute if disabled!!
    CEventListIterator i;
    CEventListIterator e = rEvents.end();
    for(i = rEvents.begin(); i != e; i++) {
      CEvent* pEvent = *i;
      if(pEvent) {
	operator()(*pEvent);
      } else {
	break;
      }
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
    (*m_pOutput)(rEvent);
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
CEventFilter::IdsToNames() 
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
