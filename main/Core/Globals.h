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

#ifndef GLOBALS_H
#define GLOBALS_H

#include <EventUnpacker.h>
#include <BufferDecoder.h>
#include <EventSinkPipeline.h>
#include <EventSink.h>
#include <TCLInterpreter.h>
#include <File.h>
#include <RunControl.h>
#include <Analyzer.h>

class CDisplayInterface;

extern CEventUnpacker*     gpUnpacker;          // Current event unpacker.
extern CBufferDecoder*     gpBufferDecoder;     // Buffer decoder (could be kpNULL).
extern CEventSinkPipeline* gpEventSinkPipeline; // Analyzed event sink pipeline; Could be kpNULL. (List of event sinks.)
extern CEventSink*         gpEventSink;         // Analyzed event sink; Could be kpNULL.
extern CTCLInterpreter*    gpInterpreter;       // Current TCL Interpreter. Can be null if not in TCL environment.
extern CFile*              gpEventSource;       // Source of data (can be kpNULL).
extern CRunControl*        gpRunControl;        // Run controlling object (could be kpNULL).
extern CAnalyzer*          gpAnalyzer;          // Points to event analyzer.
extern const char*         gpVersion;           // SpecTcl version text.
extern CDisplayInterface*  gpDisplayInterface;  // Display interface

#endif
