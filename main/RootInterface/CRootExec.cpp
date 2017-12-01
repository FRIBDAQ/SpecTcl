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

/** @file:  CRootExec.cpp
 *  @brief: implement the rootexec command.
 */

#include "CRootExec.h"
#include <TCLInterpreter.h>
#include <TCLObject.h>
#include <TApplication.h>
#include <TInterpreter.h>
#include <Exception.h>
#include <sstream>

/**
 * constructor
 *    @param interp - references the interpreter on which we'll be registered.
 *    @param command - The command that we'll be registered as.
 */
CRootExec::CRootExec(CTCLInterpreter& interp, const char* command) :
    CTCLObjectProcessor(interp, command, true) {}

/**
 * destructor - let the base class take care of everything.
 */
CRootExec::~CRootExec()
{
    
}
/**
 * operator()
 *    Execute the command.
 *
 *  @param interp - interprter that's executing the command.
 *  @param objv   - command words.  need a filename.
 */
int
CRootExec::operator()(CTCLInterpreter& interp, std::vector<CTCLObject>& objv)
{
    // Use exception handling to simplify error handling
    
    try {
        bindAll(interp, objv);
        requireExactly(
             objv, 2,
            "Incorrect number of command parameters expecting only a root macro file"
        );
        std::string filename = objv[1];
        Int_t error;
        Long_t result = TApplication::ExecuteFile(filename.c_str(), &error);
        if (error != TInterpreter::kNoError) {
            std::string severity;
            switch (error) {
                case TInterpreter::kRecoverable:
                    severity = "recoverable";
                    break;
                case TInterpreter::kDangerous:
                    severity = "dangerous";
                    break;
                case TInterpreter::kFatal:
                    severity = "fatal";
                    break;
                default:
                    severity = "unrecognized";
            }
            std::string msg = "Root file execution resulted in an error of ";
            msg += severity;
            msg += " severity";
            throw msg;
        }
        std::stringstream r;
        r << result;
        interp.setResult(r.str());
    }
    catch (CException& e) {
        interp.setResult(e.ReasonText());
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
    catch (std::exception& e) {
        interp.setResult(e.what());
        return TCL_ERROR;
    }
    catch (...) {
        
    }
    return TCL_OK;
}