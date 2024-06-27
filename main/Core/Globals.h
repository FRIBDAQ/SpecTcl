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
#include <config.h>
#include <EventUnpacker.h>
#include <BufferDecoder.h>
#include <EventSinkPipeline.h>
#include <EventSink.h>
#include <TCLInterpreter.h>
#include <File.h>
#include <RunControl.h>
#include <Analyzer.h>
#ifdef WITH_MPI
#include <mpi.h>
#endif

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
extern volatile void*               gpDisplayMemory;     // Display memory.
extern bool                gMPIParallel;       // TRUE if built with MPI and run with MPIRUN.

#ifdef WITH_MPI
// Communicator for event messages:

extern MPI_Comm                  gRingItemComm;      // Only defined in root and workers.
extern MPI_Comm                  gXamineGateComm;    // Defined in all processes.
#endif

// MPI constant definitions

// Fixed ranks in MPI

#define MPI_ROOT_RANK   0
#define MPI_EVENT_SINK_RANK 1
#define MPI_FIRST_WORKER_RANK 2

// Message type tags:

#ifndef MPI_TCL_TAG
#define MPI_TCL_TAG 1                   // Messages involving the TCL Pump.
#endif
#define MPI_GATE_TRACE_RELAY_TAG 2           // Gate Trace relay message tag.
#define MPI_SPECTRUM_TRACE_RELAY_TAG 3   // Spectrum tracde relay message tag.
#define MPI_BIND_TRACE_RELAY_TAG 4       // sbind trace relay message tag
#define MPI_RING_ITEM_TAG 5             // Ring items -> workers e.g.
#define MPI_STATE_CHANGE_TAG 6          // State changes -> event sink pipeline.

#endif

