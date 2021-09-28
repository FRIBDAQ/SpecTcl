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
#include "TclXamine.h"
#include "XamineGateHandlers.h"
#include <string>
#include <iostream>
#include <Exception.h>
#include <unistd.h>

const char* TclLibPath=SPECTCL_TCLLIBS;

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
    // Add the SpecTcl Library path to auto_path:
    
    Tcl_Obj* autopath = Tcl_ObjGetVar2(
        pInterp, Tcl_NewStringObj("auto_path", -1), NULL, TCL_GLOBAL_ONLY
    );
    if (!autopath) {
        pOInterp->setResult("Cant' read auto_path to include SpecTcl directories");
        return TCL_ERROR;
    }
    int status = Tcl_ListObjAppendElement(
        pInterp, autopath, Tcl_NewStringObj(TclLibPath, -1)
    );
    if (status != TCL_OK) {
        pOInterp->setResult("Failed to lappend Tcl Library path to auto_path");
    }
    Tcl_ObjSetVar2(
        pInterp, Tcl_NewStringObj("auto_path", -1), NULL, autopath,
        TCL_GLOBAL_ONLY
    );
    
    
    // Create the Xamine namespace:
    
    if (!Tcl_CreateNamespace(pInterp, "Xamine", nullptr, nullptr)) {
        return TCL_ERROR;
    }
    
    new GetHostCommand(*pOInterp, parsed.host_arg);
    new GetPortCommand(*pOInterp, parsed.port_arg);
    
    // The user passed into to GetUserCommand is
    //  -  The command line --user value if present or
    //  -  the output of getlogin if not.
    
    if (parsed.user_given) {
        
        new GetUserCommand(*pOInterp, parsed.user_arg);
    } else {
        new GetUserCommand(*pOInterp, getlogin());
    }
    
    new IsLocalCommand(*pOInterp);
    new TclXamine(*pOInterp);
    new XamineGateHandler(*pOInterp);
 
    // source bindir/xaminerunner.tcl  bindir is assumed to be
    //  TclLibPath../bin
    
    std::string startScript = TclLibPath;
    startScript += "/../bin/xaminerunner.tcl";
    try {
        pOInterp->EvalFile(startScript);
    }
    catch (CException& e) {
        std::cerr << "Failed to run " << startScript << " " << e.ReasonText()
            << std::endl;
    }
    
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