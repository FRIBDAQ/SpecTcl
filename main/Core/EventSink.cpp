
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



static const char* Copyright = "(C) Copyright Michigan State University 2006, All rights reserved";
#include <config.h>
#ifdef HAVE_STD_NAMESPACE
using namespace std;
#endif

#include "EventSink.h"


// Base class methods are trivial but we supply them so we can document them with nice comments.

/// Public interface:

/**
 *  OnAattach
 *    Called when an event sink has been attached to the event sink pipeline.
 * 
 * @param rAnalyzer - references the analyzer object being used for this data source.
 */
void 
CEventSink::OnAttach(CAnalyzer& rAnalyzer) {}

/**
 *  OnDetach
 *     Called when an event sink has been detached from the pipeline.
 * 
 * @param rAnalyzer - References the analyzer object that was used to analyze the source
 *          being detached.
 */
void 
CEventSink::OnDetach(CAnalyzer& rAnalyzer) {}

/**
 * OnBegin
 *    Called to provide information about a run when a begin has been detected in the data source.
 * 
 * @param runNumber - Run number of the new run.
 * @param title     - Title of the run.
 * 
 * @note - If the data source (DAQ system) does not support titles, probably best to supply
 * a blank string or the string "No title".  If the data source does not support run numbers,
 * I recommend supplying a number that increments each time this is called to simulate sequential
 * run numbers.  This is because at least one filter type uses the run number to name its output file(s).
 * 
 */
void 
CEventSink::OnBegin(unsigned runNumber, const char* title) {}

/**
 * OnEnd
 *    Called to provide information about the run that just ended.
 * 
 * @param runNumber - number of the run that just ended.
 * @param title     - Title of the run that just ended.
 * 
 * @note however they are gotten, the runNumber and title parameters should  normally be the
 * same as for the most recent call to OnBegin.
 * 
 */
void 
CEventSink::OnEnd(unsigned runNumber, const char* title) {}