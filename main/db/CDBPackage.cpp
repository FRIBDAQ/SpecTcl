/*
    This software is Copyright by the Board of Trustees of Michigan
    State University (c) Copyright 2017.

    You may use this software under the terms of the GNU public license
    (GPL).  The terms of this license are described at:

     http://www.gnu.org/licenses/gpl.txt

     Authors:
             Ron Fox
             Giordano Cerriza
	     NSCL
	     Michigan State University
	     East Lansing, MI 48824-1321
*/

/** @file:  CDBPackage.cpp
 *  @brief: Provides the Tcl package initialization entry point.
 */
#include <tcl.h>
#include <TCLInterpreter.h>
#include "CDBCommands.h"


static const char* packageName = "SpecTclDb";
static const char* packageVersion = "1.0";

extern "C" {                 // Tcl expects unmangled entry.
    int Spectcldb_Init(Tcl_Interp* pInterp)
    {
        Tcl_PkgProvide(pInterp, packageName, packageVersion);
        CTCLInterpreter* p = new CTCLInterpreter(pInterp);
        
        new CDBCommands(*p);
        
        return TCL_OK;
    }
    
}                           // extern "C"