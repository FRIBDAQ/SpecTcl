/*
    This software is Copyright by the Board of Trustees of Michigan
    State University (c) Copyright 2009.

    You may use this software under the terms of the GNU public license
    (GPL).  The terms of this license are described at:

     http://www.gnu.org/licenses/gpl.txt

     Author:
             Ron Fox
	     NSCL
	     Michigan State University
	     East Lansing, MI 48824-1321
*/

#include <tcl.h>
#include "CSpecTclInterpreter.h"
#include "CPipelineCommand.h"
#include "CDataSourceFactory.h"
#include "CFileDataSourceCreator.h"
#include "CAttachCommand.h"
#include "CTreeParameterCommand.h"
#include <TCLLiveEventLoop.h>



/**
 * @file PackageInit.cpp
 * @brief Load SpecTcl package into Tcl interpreter.
 */

static const char* packageVersion = "5.0";

/**
 * Spectcl_Init
 *
 *   'unsafe' interpreter initialization.
 *    * Provide the "spectcl" package
 *    * Create the "spectcl" namespace
 *    * Register the commands in the spectcl namespace.
 *
 * @param pInterp - pointer to the C/Tcl interpreter.
 *
 * @return int TCL_OK   if successful or TCL_ERROR if not.
 *
 * @note this must have C external bindings so that Tcl can find it
 *       undecorated.
 */
extern "C" {
  int
  Spectcl_Init(Tcl_Interp* pInterp)
  {
    int status;
    Tcl_Namespace* pNamespace;

    /* Provide the package: */

    status = Tcl_PkgProvide(pInterp, "spectcl", packageVersion);
    if (status != TCL_OK) {
      return status;
    }

    /* Create the spectcl namespace: */

    pNamespace = Tcl_CreateNamespace(pInterp, "::spectcl", NULL, NULL);
    if (!pNamespace) {
      return TCL_ERROR;
    }

    /* Create the SpecTcl commands */

    CSpecTclInterpreter::setInterp(pInterp); // So the interpreter object can wrap.
    CPipelineCommand::instance();	     // defines the pipe command.
    CAttachCommand::instance();		     // defines the attach command.
    CTreeParameterCommand::instance();	     // treeparameter command ensemble

    /* Add standard data sources to the factory: */

    CDataSourceFactory fact;
    fact.addCreator("file", new CFileDataSourceCreator);

    /* 
       We need to start an event loop for the interpreter as SpecTcl's
       file handling relies on that.
    */
    CTCLLiveEventLoop* pLoop = CTCLLiveEventLoop::getInstance();
    pLoop->start(CSpecTclInterpreter::instance());
  

    /* Return normal */

    return TCL_OK;
  }
}



void* gpTCLApplication(0);
