/*
    This software is Copyright by the Board of Trustees of Michigan
    State University (c) Copyright 2008

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
#include <tcl.h>
#include <TCLInterpreter.h>
#include "TclCreate.h"
#include "TclOpen.h"
#include "TclClose.h"
#include "TclParameter.h"
#include "TclRuns.h"
#include "TclUUID.h"
#include "TclEvtCreate.h"
#include "TclEvtClose.h"
#include "TclEvtOpen.h"
#include "TclAttach.h"
#include "TclDetach.h"

/** Include command processor headers here */

const static char* version="1.0";

/**
 ** @file packageStartup.cpp 
 **   This file contains the package initialization for the
 **   TCL Bindings to the SpecTcl experimentl library.
 */

extern "C" {
int 
Spectclexperiment_Init(Tcl_Interp* pInterp)
{
  Tcl_PkgProvide(pInterp, "SpecTcl", version);

  // Wrap the interpreter up in a CTCLInterpreter so that it can be used
  // to instantiate objects derived from CTCLObjectCommand.
  //

  CTCLInterpreter* interp = new CTCLInterpreter(pInterp);
  try {
    interp->Eval("namespace eval ::spectcl {}");
  }
  catch(...) {
    return TCL_ERROR;
  }

  // Create the commands:

  new CTclCreate(*interp);
  new CTclOpen(*interp);
  new CTclClose(*interp);
  new CTclParameter(*interp);
  new CTclRuns(*interp);
  new CTclUUID(*interp);
  new CTclEvtCreate(*interp);
  new CTclEvtClose(*interp);
  new CTclEvtOpen(*interp);
  new CTclAttach(*interp);
  new CTclDetach(*interp);
  
  return TCL_OK;
}
  void* gpTCLApplication(0);
}

