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
#include <CConstCommand.h>

using namespace std;


static char*  version = "0.1";

/*!
   The package inintialization is done here.
   - provide the constparam package.
   - Add the const command.
   @param pInterp - Tcl_Interp that is requiring this package.
   @return int
   @retval TCL_OK - most likely this all just works/
   
*/
extern "C" {
  int Constparam_Init(Tcl_Interp* pInterp) 
  {
    Tcl_PkgProvide(pInterp, "constparam", version);

    CTCLInterpreter& interp(*(new CTCLInterpreter(pInterp)));
    CConstCommand* pCommand = new CConstCommand(interp);

    return TCL_OK;
  }
}

