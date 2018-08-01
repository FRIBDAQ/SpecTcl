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
////////////////////////////////////////////////////////////////////////////

#ifndef GLOBALS_H
#define GLOBALS_H

#include <EventUnpacker.h>
#include <BufferDecoder.h>
#include <EventSink.h>
#include <TCLInterpreter.h>
#include <File.h>
#include <RunControl.h>
#include <Analyzer.h>

extern CEventUnpacker* gpUnpacker;     // Current event unpacker.
extern CBufferDecoder* gpBufferDecoder;      // Buffer decoder (could be kpNULL).
extern CEventSink*     gpEventSink;    // Analyzed event sink; could be kpNULL
extern CTCLInterpreter* gpInterpreter; // Current TCL Interpreter;
				     // can be null if not in TCL environment.
extern CFile*          gpEventSource;  // Source of data (can be kpNULL).
extern CRunControl*    gpRunControl;   // Run controlling object 
				       // (could be kpNULL). 

extern CAnalyzer*      gpAnalyzer;      // Points to event analyzer.

#endif
