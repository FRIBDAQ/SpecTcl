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

#ifndef EVENTMESSAGE_H
#define EVENTMESSAGE_H

class CEventList;
/**
 * This file contains the code that sends events from the event pipeline to the histogramer.
 * If we are not in the MPI environment, we just get the histogram singleton and pass the event list
 * to its operator().   Otherwise, we marshall up the event list into MPI Messages which are sent to 
 * the histogram process.  The histogram process, presumably is running an event pump (also included here)
 * which will reconstruct event lists and queue them as an event to the intepreter in the histogramer
 * The event processor will then invoke the operator() on the histogrammer.
 * Sadly, I don't see a way to do this in fewer than 2 messages/event.  One for the # parameters,
 * the other for the parameters themselves. 
*/


// The API:

void HistogramEvents(CEventList& events);
void BeginRun(unsigned run, const char* title);
void EndRun(unsigned run, const char* title);
void startHistogramPump();
void stopHistogramPump();

void startStateChangePump();
void stopStateChangePump();


#endif