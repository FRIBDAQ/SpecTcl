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

/** @file:  TclXamine.cpp
 *
 *  @brief: Implement the TclXamine.cpp class.  See header for more.
 */
#include "TclXamine.h"
#include <TCLInterpreter.h>
#include <TCLObject.h>
#include <Exception.h>
#include <string>
#include <client.h>
#include <stdexcept>

/**
 * TclXamine constructor
 *
 * @param interp - interpreter on which the command is being registered.
 */
TclXamine::TclXamine(CTCLInterpreter& interp) :
    CTCLObjectProcessor(interp, "Xamine::Xamine", kfTRUE)
{}

/**
 * TclXamine destructor.
 */
TclXamine::~TclXamine()
{}

/**
 * operator()
 *    -  Ensure there's at least a subcommand then dispatch to the appropriate
 *       handler.
 *    - ALl errors are encapsulated in the try/catch block.
 *  @param interp - interpreter running the command.
 *  @param objv   - Command words.
 */
int
TclXamine::operator()(CTCLInterpreter& interp, std::vector<CTCLObject>& objv)
{
    bindAll(interp, objv);
    try {
        requireAtLeast(objv, 2, "Requires at least a subcommand");
        std::string sub = objv[1];
    }
    catch (std::string msg) {
        interp.setResult(msg);
        return TCL_ERROR;
    }
    catch (CException& e) {
        interp.setResult(e.ReasonText());
        return TCL_ERROR;
    }
    catch (std::exception& e) {
        interp.setResult(e.what());
        return TCL_ERROR;
    }
    catch (...) {
        interp.setResult("Unexpected exception type");
        return TCL_ERROR;
    }
    return TCL_OK;
}