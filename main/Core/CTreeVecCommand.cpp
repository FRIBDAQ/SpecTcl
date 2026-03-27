/*
    This software is Copyright by the Board of Trustees of Michigan
    State University (c) Copyright 2026
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
 * @file CTreeVecCommand.cpp
 * @brief implement5 the "treeparamvec" command class to query about CTreeParameterVector objects.
 * @author Ron Fox <fox at frib dot msu dot edu>
 */

#include "CTreeVecCommand.h"
#include "CTreeParameterVector.h"

#include <TCLInterpreter.h>
#include <TCLObject.h>
#include <TclDict.h>
#include <stdexcept>
#include <Exception.h>




/**
 * constuctor
 *    Constructs our implementation command which is namespaced as 
 * [spectcl::serial::treeparamvec] 
 * 
 * @param rInterp - the interpreter running the command.
 * 
 */
CTreeVecActual::CTreeVecActual(CTCLInterpreter& rInterp) :
    CTCLObjectProcessor(rInterp, "spectcl::serial::treeparamvec", TCLPLUS::kfTRUE)
{}

/**
 * destructor
 * 
 */
CTreeVecActual::~CTreeVecActual() {}

/**
 *  operator()]
 *     Dispacteste subcomman to the appropriate processor.
 * 
 * @param interp - references the interpreter running the command.
 * @param objv   - the command words.
 * @return int - TCL_OK on success. The result may or may not be set.
 */
 int
 CTreeVecActual::operator()(CTCLInterpreter& interp, std::vector<CTCLObject>& objv) {
    try {

    }
    catch (std::string msg) {
        interp.setResult(msg);
        return TCL_ERROR;
    }
    catch (CException& e) {
        interp.setResult(e.ReasonText());
    }
    catch (std::exception& e) {
        interp.setResult(e.what());
    }

    return TCL_OK;
 }
