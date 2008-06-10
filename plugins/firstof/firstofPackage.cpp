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

#include <config.h>
#include <tcl.h>
#include <TCLInterpreter.h>
#include <Iostream.h>

#include "CFirstofCommand.h"

#ifdef HAVE_STD_NAMESPACE
using namespace std;
#endif

static char* version = "1.0";
extern "C" {
  int Firstof_Init(Tcl_Interp* pInterp) 
  {
    Tcl_PkgProvide(pInterp, "firstof", version );


    // Wrap the interpreter in an interpreter object and 
    // create the commands.

    CTCLInterpreter& interp(*(new CTCLInterpreter(pInterp)));
    CFirstofCommand* pAddedCommand   = new CFirstofCommand(interp, "firstof");
    CFirstofCommand* pBiggestCommand = new CFirstofCommand(interp, "biggestof");

    return TCL_OK;

 
  }
}
