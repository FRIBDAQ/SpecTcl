/*
    This software is Copyright by the Board of Trustees of Michigan
    State University (c) Copyright 2017.

    You may use this software under the terms of the GNU public license
    (GPL).  The terms of this license are described at:

     http://www.gnu.org/licenses/gpl.txt

     Authors:
             Ron Fox
             Jeromy Tompkins 
	     NSCL
	     Michigan State University
	     East Lansing, MI 48824-1321
*/

/** @file:  CRootExitCommand.cpp
 *  @brief: Implement exit command that does a TRint::Terminate.
 *
 */
#include "CRootExitCommand.h"
#include <TCLInterpreter.h>
#include <TCLObject.h>
#include <TApplication.h>             // Presumably defines gApplication
#include <Exception.h>
#include <exception>
#include <string>


/**
 * constructor
 *    Base class does all the work:
 *
 * @param interp - interpreter on which the command is getting registered.
 * @param cmd    - command word that invokes us.
 */
CRootExitCommand::CRootExitCommand(
    CTCLInterpreter& interp, const char* cmd
) : CTCLObjectProcessor(interp, cmd, true)
{
}
/**
 * destructor - let the base class do the work.
 */
CRootExitCommand::~CRootExitCommand(){}

/**
 * operator()
 *    Invoke the gApplication->Terminate method.  note that
 *    if there's no exit code, we just default to 0, normal exit
 *
 * @param interp - interpreter running the command.
 * @param objv   - Command words. (optional status integer).
 * @return - while we formally return an int, the only way we can actually
 *           return is on an error (e.g. non integer error code).
 */
int
CRootExitCommand::operator()(CTCLInterpreter& interp, std::vector<CTCLObject>& objv)
{
    try {
        bindAll(interp, objv);
        requireAtMost(objv, 2, "Incorrect number of parameters");
        
        int status = 0;                   // default value.
        if (objv.size() == 2) {
            status = objv[1];             // Overridden by user.
        }
        gApplication->Terminate(status);
    }
    catch (CException& e) {
        interp.setResult(e.ReasonText());
        return TCL_ERROR;
    }
    catch (std::exception& e) {
        interp.setResult(e.what());
        return TCL_ERROR;
    }
    catch (std::string msg) {
        interp.setResult(msg);
        return TCL_ERROR;
    }
    catch (const char* msg) {
        interp.setResult(msg);
        return TCL_ERROR;
    }
    catch (...) {
        interp.setResult("(root)exit - unanticipated exception type caught");
        return TCL_ERROR;
    }
    // We really should not wind up here:
    
    interp.setResult("Root didn't exit!!!");
    return TCL_ERROR;
}