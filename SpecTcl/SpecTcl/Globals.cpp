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

// Globals.cpp:
//    This file defines the globals which are used by the NSCL histogramming
//    program.  All items in this file are available to clients in the
//    header file:  Globals.h
//
// Author:
//    Ron FOx
//    NSCL
//    Michigan State University
//    East Lansing, MI 48824-1321
//    
// Change Log:
//     July 12, 1999 Ron FOx
//        Added gpVersion and set it to be 0.2
//   $Log$
//   Revision 4.5.2.2  2007/04/02 15:46:45  ron-fox
//   Final commit prior to April shutdown distrobuild
//
//   Revision 4.5.2.1  2005/03/04 19:10:22  ron-fox
//   Set version to 2.1-017
//
//   Revision 4.5  2003/08/27 13:37:52  ron-fox
//   Create the event sink pipeline in the TclGrammerApp so that we have explicit
//   control over when it gets created and don't get any compiler dependencies on
//   when globals are constructed.
//


static char* pCopyright=
  "Globals.cpp (c) Copyright 1999 NSCL, All rights reserved.";

#include <EventUnpacker.h>
#include <BufferDecoder.h>
#include <EventSinkPipeline.h>
#include <EventSink.h>
#include <TCLInterpreter.h>
#include <File.h>
#include <RunControl.h>
#include <Analyzer.h>
#include <histotypes.h>
#include <NSCLBufferDecoder.h>

//  The following points to the current event unpacker.
//  This must be an object which is derived from the CEventUnpacker class.
//  The user, when tailoring this system, must create an event unpacker
//  named gUnpacker.
//    Our pointer by default points to that unpacker:
extern CEventUnpacker&  grUnpacker;
CEventUnpacker*         gpUnpacker = 0; //&grUnpacker;

//  The buffer decoder is responsible for knowing the global format of
//  event buffers.  This includes knowing about buffer headers, and event
//  types.  It is up to the formatter to, given a buffer, call the
//  Analyzer's appropriate callback virtual functions.
//  The default buffer decoder understands the NSCL buffer format.
//  If another buffer format is used, then the user must hook a formatter
//  for it into the gpBufferDecoder global.

// CNSCLBufferDecoder gNSCLDecoder;
CBufferDecoder*    gpBufferDecoder = 0; //gNSCLDecoder;

//  The event sink takes dumps of event lists and processes them.  The default
//  action is to process them through a CAnalyzer object.  The CAnalyzer object
//  is created dynamically and stored in this pointer in order to 
//  allow the size of the display memory to be gotten from the TCL .rc file.
//  If it is desired to modify the sink (e.g. to hook a different sink in,
//  the application initialization must be copied and initialized.  This 
//  initialization is in the file:  AppInit.cpp
CEventSinkPipeline* gpEventSinkPipeline = (CEventSinkPipeline*)kpNULL;
CEventSink* gpEventSink = (CEventSink*)kpNULL;

//   The entire system is 'command driven' using a CTCLInterpreterObject
//   to interface it to TCL.  It is useful to have such an object available
//   to allow user code to interface with it.  This pointer is also dynamically
//   set at an appropriate time in AppInit.cpp:
CTCLInterpreter* gpInterpreter = (CTCLInterpreter*)kpNULL;

//  The event source is set dynamically by appropriate TCL commands.
//  It is available for external manipulation here.
CFile* gpEventSource     = (CFile*)kpNULL;

//  The run control object is dynamically created and a pointer to it is
//  maintained here.  Setting this allows the analysis framework to run in 
//  non Tcl/TK environments.
CRunControl* gpRunControl = (CRunControl*)kpNULL;

//  The analyzer is responsible for taking a buffer and producing lists of
//  events which are shoved into the sink.
//  The analyzer is produced in the AppInit.cpp module.
CAnalyzer* gpAnalyzer   = (CAnalyzer*)kpNULL;

// SpecTcl also retains a version text string.
const char* gpVersion = "2.1-017"; 
