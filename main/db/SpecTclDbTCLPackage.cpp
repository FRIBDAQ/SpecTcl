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

/** @file:  SpecTclDbTCLPackage.cpp
 *  @brief: Initializaion entries for the SpecTcl database API Tcl bindings.
 */
#include <tcl.h>
#include <TCLInterpreter.h>
#include <iostream>
#include "DBTcl.h"
#include "CDBCommands.h"

const char* version="1.0";

extern "C" {
    int  Spectcldbtcl_Init(Tcl_Interp* pRaw)
    {
        Tcl_PkgProvide(pRaw, "SpecTclDB", version);
        CTCLInterpreter* pInterp = new CTCLInterpreter(pRaw);

        new SpecTclDB::DBTcl(*pInterp);        
        return TCL_OK;
    }
}
