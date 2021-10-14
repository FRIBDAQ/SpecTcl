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

/** @file:  mirror.cpp
 *  @brief: Entry point for mirror.
 */

/**
 * processes command line parameters,
 * starts up the Tcl intepreter
 * and transfers control to the mirror.tcl startup script.
 */
#include "cmdline.h"
const char* TclLibPath = SPECTCL_TCLLIBS;


static struct gengetopt_args_info parsed;


/**
 * main
 *    Program entry point
 *    - Parse the command arguments
 *    - start up the interpreter where we pick up again with AppInit.
 *
 */
int
main(int argc, char** argv)
{
    cmdline_parser(argc, argv, &parsed);
    
    // Start up teh TCLInterpreter
}