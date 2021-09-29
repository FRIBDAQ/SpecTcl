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

/** @file:  CRemoteCommand.cpp
 *  @brief: Implement the isRemote command.
 */
#include "CRemoteCommand.h"
#include "TCLInterpreter.h"
#include "TCLObject.h"

/**
 * CRemoteCommand - constructor
 *    @param interp - interpreter on which the command will be registered.
 */
CRemoteCommand::CRemoteCommand(CTCLInterpreter& interp) :
    CTCLObjectProcessor(interp, "isRemote", kfTRUE)
{}

/**
 * CRemoteCommand - destructor.
 */
CRemoteCommand::~CRemoteCommand() {}

/**
 * operator()
 *    Executes the command.  In this case we just check the parameter count and
 *    return 0 in the result if all is well.
 *  @param interp - interpreter object running the command.
 *  @param objv   - The command words.
 *  @return int   - TCL_OK on success, TCL_ERROR if not.
 */
int
CRemoteCommand::operator()(CTCLInterpreter& interp, std::vector<CTCLObject>& objv)
{
    if (objv.size() != 1) {
        interp.setResult("Incorrect number of command line parameters");
        return TCL_ERROR;
    }
    
    interp.setResult("0");
    return TCL_OK;
}