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

/** @file:  CmdInfo.cpp
 *  @brief:  Implement the CMDInfo class.
 */

#include "CmdInfo.h"
#include <TCLObject.h>
#include <TCLInterpreter.h>

/**
 * CmdInfo constructor
 *   @param interp -interpreter on which the command is registered.
 *   @param command -the command verb.
 *   @param value   - the value returned by the command.
 */
CmdInfo::CmdInfo(CTCLInterpreter& interp, const char* command, const char* value) :
    CTCLObjectProcessor(interp, command, TCLPLUS::kfTRUE),
    m_value(value)
{}

/**
 * CmdInfo destructor
 */
CmdInfo::~CmdInfo()
{
    
}

/**
 * operator()
 *    Command processor:
 *    - Verify there are no parameters.
 *    - return the value as the result.
 * @param interp - interpreter executing the command.
 * @param objv   - Command parameters.
 * @return int - TCL_OK unless there are errors in which case TCL_ERROR
 */
int
CmdInfo::operator()(CTCLInterpreter& interp, std::vector<CTCLObject>& objv)
{
    try {
        requireExactly(objv, 1, "No command parameters are allowed");
        interp.setResult(m_value);
    }
    catch (std::string& msg) {
        interp.setResult(msg);
        return TCL_ERROR;
    }
    catch (...) {
        interp.setResult("Unexepcted exception type");
        return TCL_ERROR;
    }
    return TCL_OK;
}

