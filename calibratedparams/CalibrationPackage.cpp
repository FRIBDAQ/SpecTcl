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
#include <SpecTcl.h>
#include <tcl.h>
#include <TCLInterpreter.h>
#include "CFitCommand.h"
#include "CCalibratedParameterCommand.h"

/*! \file CalibrationPackage.cpp
       This file contains the package initialization for the calibrated
       parameter package.  This package consists of the following:
       -# A set of commands for creating calibration fits and defining
          parameters, each of which will be computed by applying a
          fit to a raw parameter.
       -# An event processor which actually computes the calibrated
          parameters on an event by event basis.
       -# A set of scripts that:
          - provide support for saving/restoring fit definitions and
            calibrated parameter definitions.
          - provide a GUI for creating and manipulating fits and calibrated
            parameter definitions.

  The entry point is called as a Tcl Package initialization function.
  It must:
  -# Register the commands with the interpreter.
  -# Instantiate and register the event processor
  -# Make the GUI available to be instantiated and pasted into any container
     widget (toplevel or frame depending on how the person who loads the 
     plugin wants to integrte the GUI with an existing GUI.

*/
Calibrationpackage_Init
