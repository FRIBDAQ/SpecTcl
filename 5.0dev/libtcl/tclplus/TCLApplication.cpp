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

static const char* Copyright = "(C) Copyright Michigan State University 2015, All rights reserved";
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

#include <config.h>

#include "TCLApplication.h"
#include "TCLInterpreter.h"

#include <string>

#include <daqdatatypes.h>
#include <assert.h>
#include <tcl.h>
#include <tk.h>
#include <iostream>

using namespace std;


//
// Global/External declarations:
//
extern CTCLApplication* gpTCLApplication; // User's application.

CTCLInterpreter*        gpTCLInterpreter; // Globally available interpreter.


//
// Functions for TCLApplication.cpp:
//


static int    savedArgc;
static char** savedArgv;


/*!
  Construction of the application:
*/
CTCLApplication::CTCLApplication()
{
  m_pInit = (void*)Tcl_AppInit; 
  m_thread = Tcl_GetCurrentThread(); // hopefully we can call this before interp created.
} 


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
  savedArgc = argc;
  savedArgv = argv;


  try {
    Tcl_Main(argc, argv, Tcl_AppInit);
    return 0;			/* Needed only to prevent compiler warning. */
  }
  catch (string msg) {
    cerr << "An unhandled string exception was caught: " << msg << endl;
    return -1;
  }
  catch (const char* msg) {
    cerr << "An unhandled char* exception was caught: " << msg << endl;
    return -1;
  }
  catch(...) {
    cerr << "Unhandled exception\n";
    return -1;
  }
}

extern "C" {
  int
  Tcl_AppInit(Tcl_Interp* interp)
  {
    if(Tcl_Init(interp) == TCL_ERROR) // Initialize the interpreter.
      return TCL_ERROR;
  


    gpTCLInterpreter = new CTCLInterpreter(interp); 
    assert(gpTCLInterpreter != (CTCLInterpreter*)kpNULL);

    gpTCLApplication->Bind(*gpTCLInterpreter);

    return (*gpTCLApplication)();

  }
}

/*!
   Provide a hook for the application to get the program arguments.
   \param argc   - reference to integer that will get the argument count.
   \param argv   - reference to char** that will get the argument count.
*/
void
CTCLApplication::getProgramArguments(int& argc, char**& argv)
{
  argc = savedArgc;
  argv = savedArgv;
}

/*!
  Get the thread for the interpreter/application:
*/
Tcl_ThreadId
CTCLApplication::getThread() const
{
  return m_thread;
}
