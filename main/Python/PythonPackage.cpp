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

/** @file:  PythonPackage.cpp
 *  @brief: Package initialization for SpecTcl python package.
 */
#include <tcl.h>
#include <Python.h>
#ifdef HAVE_STAT             // Dueling configs
#undef HAVE_STAT
#endif
#include <TCLInterpreter.h>
#include <SpecTcl.h>

#include "CPythonCommand.h"

#include <Exception.h>           // Eval throws CTCLExceptions.
#include <string>

// Python extensions to interface back to Tcl.
// NOTE:  These assume python has been initialized already.


///////////////////////////////////////////////////////////////////////////////
// Tcl package initialization.

static const char* version("1.0");
extern "C" {
    int Spectclpython_Init(Tcl_Interp* interp)
    {
        Tcl_PkgProvide(interp, "python", version);
        SpecTcl* api = SpecTcl::getInstance();
        CTCLInterpreter* pSpecTclInterp = api->getInterpreter();
        
        // Load the Python command:
        
        new CPythonCommand(*pSpecTclInterp, "python");
        std:: cerr << " SpecTcl 'python' package " << version << " loaded\n";
        
        
        return TCL_OK;
    }
}