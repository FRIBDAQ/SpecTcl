/*
    This software is Copyright by the Board of Trustees of Michigan
    State University (c) Copyright 2017.

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

/** @file:  rootPackage.cpp
 *  @brief: provides package initialization code for root interface package.
 */

/**
 *  A bit about what's going on here.  Bits and pieces of SpecTcl (histograms)
 *  are now cast in terms of root objects.  SpecTcl's initialization and
 *  shutdown are Root aware and furthermore the Tcl event loop time slices
 *  with the root event loop.
 *
 *  SpecTcl, however also provides some Tcl commands that further integrate
 *  SpecTcl and Root (root exec and roottree for example).
 *  These are extracted into a dynamically loadable root package.
 *
 *   This code is the initialization code for that package.
 */

#include <tcl.h>
#include <SpecTcl.h>
#include <TCLInterpreter.h>
#include "TreeCommand.h"
#include "CRootExec.h"

static const char* version("1.0");
extern "C" {
    int Rootinterface_Init(Tcl_Interp* interp)
    {
        Tcl_PkgProvide(interp, "rootinterface", version);
        
        SpecTcl* api = SpecTcl::getInstance();
        CTCLInterpreter* pInterp = api->getInterpreter();
        
        new TreeCommand(*pInterp, "roottree");
        new CRootExec(*pInterp, "rootexec");

        return TCL_OK;
    }
}