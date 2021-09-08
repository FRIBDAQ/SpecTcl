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

/** @file:  Info.cpp
 *  @brief:  Provide implementations of the classes in Info.h
 */

#include "Info.h"
#include <TCLInterpreter.h>
#include <TCLObject.h>

///////////////////////////////////////////////////////////////////////////////
// GetHostCommand implementation.

/**
 *  GetHostCommand constructor
 *    @param interp - references the interpreter on which the command
 *                    is registered.
 *    @param host   - The --host parameter value.
 */
GetHostCommand::GetHostCommand(CTCLInterpreter& interp, const char* host) :
    CTCLObjectProcessor(interp, "Xamine::getHost", kfTRUE),
    m_host(host)
{}

/**
 * GetHostCommand destructor
 */
GetHostCommand::~GetHostCommand()
{}

/**
 * operator()
 *    - ensure there are no further command line parameters.
 *    - return m_host.
 */
int
GetHostCommand::operator()(CTCLInterpreter& interp, std::vector<CTCLObject>& objv)
{
    requireExactly(objv, 1, "No additional command parameters accepted by getInfo");
    interp.setResult(m_host);
    return TCL_OK;
}