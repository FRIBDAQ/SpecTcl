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
    try {
        requireExactly(objv, 1, "No additional command parameters accepted by getInfo");
        interp.setResult(m_host);
    } catch (std::string msg) {
        interp.setResult(msg);
        return TCL_ERROR;
    }
    catch (...) {
        interp.setResult("Unanticipated exception type");
        return TCL_ERROR;
    }
    return TCL_OK;
}
///////////////////////////////////////////////////////////////////////////////
// GetPortCommand

/**
 * GetPortCommand constructor
 *   @param interp - interpreter on which the command is being registered.
 *   @param port   - stringified port (could be a service).
 */
GetPortCommand::GetPortCommand(CTCLInterpreter& interp, const char* port) :
    CTCLObjectProcessor(interp, "Xamine::getPort", kfTRUE),
    m_port(port)
{}

/**
 * GetPortCommand - destructor.
 */
GetPortCommand::~GetPortCommand()
{}

/**
 * operator()
 *  - ensure there are no command line parameters.
 *  - Return the m_port value.
 */
int
GetPortCommand::operator()(CTCLInterpreter& interp, std::vector<CTCLObject>& objv)
{
    try {
        requireExactly(objv, 1, "Incorrect number of command parameters");
        interp.setResult(m_port);
    }
    catch (std::string msg) {
        interp.setResult(msg);
        return TCL_ERROR;
    }
    catch (...) {
        interp.setResult("Unanticipated exeption type");
        return TCL_ERROR;
    }
    return TCL_OK;
}