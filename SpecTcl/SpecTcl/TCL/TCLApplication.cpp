//
//  TCLApplication.cpp
//     This implementation file bridges between TCL/TK and the C++
//     TCL/TK object framework.  The idea is that the user supplies a 
//     derivation of CTCLApplication, and places a pointer to it in:
//      gpTCLApplication
//     This file contains a Tcl_AppInit() which is called by either tclsh or
//     wish depending on how we're linked.  Tcl_AppInit does the following:
//     1. Initialize the interpreter, and TK.
//     2. Create a CTCLInterpreter and store it's pointer in gpTCLInterpreter
//     3. Bind the Application pointed to by gpTCLApplication to the 
//        interpreter and finally:
//     4. Invoke that application's operator() function to allow it to
//        initialize any commands which are application specific.
//
// Author:
//    Ron Fox
//    NSCL
//    Michigan State University
//    East Lansing, MI 48824-1321
//    mailto: fox@nscl.msu.edu
//
// (c) Copyright NSCL 1999, all rights reserved
//
///////////////////////////////////////////////////////////////////////////


#include "TCLApplication.h"
#include "TCLInterpreter.h"
#include <histotypes.h>
#include <assert.h>
#include <tcl.h>
#include <tk.h>

static char* pCopyright =
"TCLApplication.cpp - (c) Copyright NSCL 1999, all rights reserved\n";

//
// Global/External declarations:
//
extern CTCLApplication* gpTCLApplication; // User's application.

CTCLInterpreter*        gpTCLInterpreter; // Globally available interpreter.


//
// Functions for TCLApplication.cpp:
//


#ifdef TK_TEST
EXTERN int		Tktest_Init _ANSI_ARGS_((Tcl_Interp *interp));
#endif /* TK_TEST */

/*
 *----------------------------------------------------------------------
 *
 * main --
 *
 *	This is the main program for the application.
 *
 * Results:
 *	None: Tk_Main never returns here, so this procedure never
 *	returns either.
 *
 * Side effects:
 *	Whatever the application does.
 *
 *----------------------------------------------------------------------
 */

int
main(int argc,char** argv)
{
    Tk_Main(argc, argv, Tcl_AppInit);
    return 0;			/* Needed only to prevent compiler warning. */
}

extern "C" {
  int
  Tcl_AppInit(Tcl_Interp* interp)
  {
    if(Tcl_Init(interp) == TCL_ERROR) // Initialize the interpreter.
      return TCL_ERROR;
  
    if(Tk_Init(interp) == TCL_ERROR)
      return TCL_ERROR;
    Tcl_StaticPackage(interp, "Tk", Tk_Init, Tk_SafeInit);

#ifdef TK_TEST
    if (Tktest_Init(interp) == TCL_ERROR) {
	return TCL_ERROR;
    }
    Tcl_StaticPackage(interp, "Tktest", Tktest_Init,
            (Tcl_PackageInitProc *) NULL);
#endif /* TK_TEST */

    gpTCLInterpreter = new CTCLInterpreter(interp); 
    assert(gpTCLInterpreter != (CTCLInterpreter*)kpNULL);

    gpTCLApplication->Bind(*gpTCLInterpreter);

    return (*gpTCLApplication)();

  }
}
