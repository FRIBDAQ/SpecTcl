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

/** @file:  Mirror.cpp
 *
 *  @brief:  Implement the Xamine::startMirrorIfNeeded command
 */
#include "Mirror.h"
#include "SpecTclMirrorClient.h"
#include <TCLInterpreter.h>
#include <TCLObject.h>
#include <Exception.h>
#include <stdexcept>
#include <string>

/**
 * constructor
 *   @param interp - references the interpreter the command is registered on.
 */
CTCLMirrorSetup::CTCLMirrorSetup(CTCLInterpreter& interp) :
    CTCLObjectProcessor(interp, "Xamine::startMirrorIfNeeded", TCLPLUS::kfTRUE)
{}

/**
 * destructor
 */
CTCLMirrorSetup::~CTCLMirrorSetup()
{}

/**
 * operator()
 *   Perform the command.
 * @param interp - interpreter executing the command.
 * @param objv   - wrapped command words.
 * @return int   - TCL_OK on success, TCL_ERROR on failure.
 */
int
CTCLMirrorSetup::operator()(
    CTCLInterpreter& interp, std::vector<CTCLObject>& objv
)
{
    try {
        bindAll(interp, objv);
        requireExactly(objv, 5);
        
        std::string host = objv[1];
        std::string rest = objv[2];
        std::string mirror = objv[3];
        std::string user = objv[4];
        if (!getSpecTclMemory(host.c_str(), rest.c_str(), mirror.c_str(), user.c_str())) {
            throw Mirror_errorString(Mirror_errorCode());
        }
        
    }
    catch (std::exception& e) {
        interp.setResult(e.what());
        return TCL_ERROR;
    }
    catch (CException& e) {
        interp.setResult(e.ReasonText());
        return TCL_ERROR;
    }
    catch (std::string msg) {
        interp.setResult(msg);
        return TCL_ERROR;
    }
    catch (...) {
        interp.setResult("Unanicipated exception type");
        return TCL_ERROR;
    }
    return TCL_OK;
}