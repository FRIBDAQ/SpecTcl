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
#include <SpecTcl.h>
#include "CMapValueCommand.h"


using namespace std;



static const char* version="1.0";

// This is the package initialization entry point.. .just add a creator to the
// list of creators that can build filter output stages:

extern "C" int Mapvalues_Init(Tcl_Interp* pInterp)
{
  Tcl_PkgProvide(pInterp, "MapValues", version);


  SpecTcl* api = SpecTcl::getInstance();
  CTCLInterpreter* pInt = api->getInterpreter();

  new CMapValueCommand(*pInt);

  return TCL_OK;

}
