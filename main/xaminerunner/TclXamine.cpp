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
#include <ErrnoException.h>
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
        if (sub == "genenv") {
            genenv(interp, objv);
        } else {
            std::string msg = "Unrecognized subcommand: " ;
            msg += sub;
            throw std::string(msg);
        }
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
///////////////////////////////////////////////////////////////////////////////
// Subcommand execution methods.  These all report errors via exceptions.

/**
 * genenv
 *   Sets up the Xamine environment variables.  Parameters are the shared memory
 *   key and size.  Note that on failure, Xamine_genenv is assumed to leave
 *   an errno with the proper values.
 *  @param interp - interpreter running the command.
 *  @param objv   - Command words.
 */
void
TclXamine::genenv(CTCLInterpreter& interp, std::vector<CTCLObject>& objv)
{
    requireExactly(objv, 4, "Xamine::Xamine genenv shmemname spectrumbytes");
    std::string name = objv[2];
    int         size = objv[3];
    
    if (!Xamine_genenv(name.c_str(), size)) {
        throw CErrnoException("Unable to set up Xamine environment variables");
    }
}