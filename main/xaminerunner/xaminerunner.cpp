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

/** @file:  xaminerunner.cpp
 *  @brief: Main program for the Xamine Runner.
 */

/**
 * Processes command line parameters,
 * starts the intepreter.
 * registers all those nice commands the main depends on.
 * Finds and Sources in the script that executes the application.
 */
#include <tcl.h>

#include <TCLInterpreter.h>
#include "cmdline.h"

#include "Info.h"

// Put the parsed command junk here.

static struct gengetopt_args_info parsed;

/**
 * Application initialization.
 */

static int AppInit(Tcl_Interp* pInterp)
{
    if (Tcl_Init(pInterp) != TCL_OK) {
        return TCL_ERROR;
    }
    
    // Wrap the interpreter in a CTCLInterpreter:
    
    CTCLInterpreter* pOInterp = new CTCLInterpreter(pInterp);
    
    // Set the initialization script for now just ~/.tclshrc -
    // TODO: set it to the application script.
    
    Tcl_ObjSetVar2(
        pInterp, Tcl_NewStringObj("tcl_rcFilename", -1), NULL,
        Tcl_NewStringObj("~/.tclshrc", -1), TCL_GLOBAL_ONLY
    );
    // Create the Xamine namespace:
    
    if (!Tcl_CreateNamespace(pInterp, "Xamine", nullptr, nullptr)) {
        return TCL_ERROR;
    }
    
    new GetHostCommand(*pOInterp, parsed.host_arg);
    new GetPortCommand(*pOInterp, parsed.port_arg);
    
    return TCL_OK;

}

//----------------------------------------------------------------------------
// Entry point.


/**
 * main
 *   Entry point.
 */
int main(int argc, char** argv)
{
    cmdline_parser(argc, argv, &parsed);
    
    // Start up Tcl, our App init will take over from here:
    
    Tcl_Main(argc, argv, AppInit);
}