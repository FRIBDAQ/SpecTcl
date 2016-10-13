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

/**
 * @file VersionCommand.cpp
 * @brief implement a command to return the SpecTcl version.
 * @author Ron Fox<fox@nscl.msu.edu>
 */

#include "VersionCommand.h"
#include "TCLObject.h"
#include "TCLInterpreter.h"


/**
 * constructor
 *    Use base class constructor to register the command.
 *
 *   @param interp - References the interpreter on which the command is
 *                   registered.
 *   @param command - Command keyword (defaults to version).
 */
CVersionCommand::CVersionCommand(CTCLInterpreter& interp, const char* command) :
    CTCLObjectProcessor(interp, command, true)
{}

/**
 * destructor
 */
CVersionCommand::~CVersionCommand() {}

/**
 * operator()
 *    The actual command processor.  Our job is trivial since the VERSION
 *    preprocessor string already carries the Spectcl version string.
 *
 *   @param interp - Interpreter running the commnand.
 *   @param objv   - The encapsulated Tcl_Obj's that make up the command.
 *   @return int   - TCL_OK on success, TCL_ERROR on failure
 *                   The only possible failure is to have additional words in
 *                   the command list after the keyword.
 */
int CVersionCommand::operator()(CTCLInterpreter& interp, std::vector<CTCLObject>& objv)
{
    if (objv.size() != 1) {
        interp.setResult("Incorrect number of parameters");
        return TCL_ERROR;
    }
    
    interp.setResult(VERSION);
    return TCL_OK;
}