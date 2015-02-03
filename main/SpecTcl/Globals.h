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

//
//  Globals.h:
//     This file describes the set of global variables which are availble
//     to NSCLHistogramming clients.  
// Author:
//     Ron FOx
//     NSCL
//     Michigan State Unversity
//     East Lansing, MI 48824-1321
//
// (c) Copyright 1999 NSCL, All rights reserved."
//
//
//  Change Log:
//      July 12, 1999 Ron Fox
//         Added gpVersion.
//
////////////////////////////////////////////////////////////////////////////

#ifndef __GLOBALS_H
#define __GLOBALS_H

#ifndef __EVENTUNPACKER_H
#include <EventUnpacker.h>
#endif

#ifndef __BUFFERDECODER_H
#include <BufferDecoder.h>
#endif

#ifndef __EVENTSINKPIPELINE_H
#include <EventSinkPipeline.h>
#endif

#ifndef __EVENTSINK_H
#include <EventSink.h>
#endif

#ifndef __TCLINTERPRETER_H
#include <TCLInterpreter.h>
#endif

#ifndef __FILE_H
#include <File.h>
#endif

#ifndef __RUNCONTROL_H
#include <RunControl.h>
#endif

#ifndef __ANALYZER_H
#include <Analyzer.h>
#endif

extern CEventUnpacker*     gpUnpacker;          // Current event unpacker.
extern CBufferDecoder*     gpBufferDecoder;     // Buffer decoder (could be kpNULL).
extern CEventSinkPipeline* gpEventSinkPipeline; // Analyzed event sink pipeline; Could be kpNULL. (List of event sinks.)
extern CEventSink*         gpEventSink;         // Analyzed event sink; Could be kpNULL.
extern CTCLInterpreter*    gpInterpreter;       // Current TCL Interpreter. Can be null if not in TCL environment.
extern CFile*              gpEventSource;       // Source of data (can be kpNULL).
extern CRunControl*        gpRunControl;        // Run controlling object (could be kpNULL).
extern CAnalyzer*          gpAnalyzer;          // Points to event analyzer.
extern const char*         gpVersion;           // SpecTcl version text.

#endif
