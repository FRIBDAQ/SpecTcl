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

#include "CRWWrite.h"
#include "CRWRead.h"

#ifdef HAVE_STD_NAMESPACE
using namespace std;
#endif

static char* version = "0.1";
extern "C" {
  int Radwareio_Init(Tcl_Interp* pInterp) 
  {
    Tcl_PkgProvide(pInterp, "Radwareio", version );

    cerr << "\n\nradwareio incorporates software that is part of the Radware package\n";
    cerr << "written by David Radford at Oak Ridge National Laboratories\n";
    cerr << "This software is incorporated with permission and thanks\n";

    // Wrap the interpreter in an interpreter object and 
    // create the commands.

    CTCLInterpreter& interp(*(new CTCLInterpreter(pInterp)));
    CRWWrite*  pWrite = new CRWWrite(interp);
    CRWRead*   pRead  = new CRWRead(interp);

    return TCL_OK;

 
  }
}
