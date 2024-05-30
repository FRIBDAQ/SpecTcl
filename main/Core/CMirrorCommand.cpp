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

/** @file:  CMirrorCommand.cpp
 *  @brief: Implement the SpecTcl mirror command.
 */
#include "CMirrorCommand.h"
#include "MirrorDirectory.h"
#include <TCLInterpreter.h>
#include <TCLObject.h>
#include <Exception.h>
#include <TCLException.h>
#include <stdexcept>
#include <SpecTcl.h>
#include <Globals.h>
#include <TclPump.h>
#include <tcl.h>

/**
 * CMirrorCommandActual - constructor.
 *
 * @param interp - interpreter on which the command is being registered.
 */
CMirrorCommandActual::CMirrorCommandActual(CTCLInterpreter& interp) :
    CTCLObjectProcessor(interp, "mirror", TCLPLUS::kfTRUE)
{}

/**
 * CMirrorCommandActual - destructor.
 */
CMirrorCommandActual::~CMirrorCommandActual()
{}

/**
 * operator()
 *    Gets control when the command is issued.
 * @param interp - interpreter that's executing the command.
 * @param objv   - The command words (including the 'mirror' command)
 * @return int   - TCL_OK on success, TCL_ERROR if not.
 */
int
CMirrorCommandActual::operator()(CTCLInterpreter& interp, std::vector<CTCLObject>&objv)
{
    // If we're not in the event sink pipeline just return TCL_OK:

    
    if (gMPIParallel && (myRank() != MPI_EVENT_SINK_RANK)) {
        return TCL_OK;
    }
    bindAll(interp, objv);
    try {
        requireAtLeast(objv, 2, "Usage: mirror subcommand ...");
        std::string subcommand = objv[1];
        if (subcommand == "list") {
            list(interp, objv);
        } else {
            throw CTCLException(interp, TCL_ERROR, "mirror - invalid subcommand.");
        }
    }
    catch (CException& e) {
        interp.setResult(e.ReasonText());
        return TCL_ERROR;
    }
    catch (std::exception& e) {
        interp.setResult(e.what());
        return TCL_ERROR;
    }
    catch (std::string& msg) {
        interp.setResult(msg);
        return TCL_ERROR;
    }
    catch (const char* msg) {
        interp.setResult(msg);
        return TCL_ERROR;
    }
    catch (...) {
        interp.setResult("Unanticipated exception caught by mirror command");
        return TCL_ERROR;
    }
    
    return TCL_OK;
}
/////////////////////////////////////////////////////////////////////////////
// Subcommand executore (private)

/**
 * list
 *    List the current set of mirrors.
 * @param interp - interpreter that's executing the command.
 * @param objv   - The command words (including the 'mirror' command)
*/
void
CMirrorCommandActual::list(CTCLInterpreter& interp, std::vector<CTCLObject>& objv)
{
    requireAtMost(objv, 3, "Usage: mirror list ?pattern?");
    std::string pattern = "*";    // Default pattern
    if(objv.size() == 3) {
        pattern = std::string(objv[2]);
    }
    MirrorDirectorySingleton* pDir = MirrorDirectorySingleton::getInstance();
    auto listing = pDir->list();
    
    CTCLObject result;
    result.Bind(interp);
    for (auto item : listing) {
        std::string host = item.first;
        std::string key  = item.second;
        
        if (Tcl_StringMatch(host.c_str(), pattern.c_str())) {
            CTCLObject dict;
            dict.Bind(interp);
            addToDict(dict, interp, "host", host.c_str());
            addToDict(dict, interp, "shmkey", key.c_str());
            result += dict;
        }
    }
    interp.setResult(result);
}
//////////////////////////////////////////////////////////////////////////
// Utilities (private)

void
CMirrorCommandActual::addToDict(
    CTCLObject& dict, CTCLInterpreter& interp,
    const char* key, const char* value
)
{
    Tcl_Obj* pObj = dict.getObject();
    Tcl_Obj* keyObj = Tcl_NewStringObj(key, -1);
    Tcl_Obj* valObj = Tcl_NewStringObj(value, -1);
    
    Tcl_DictObjPut(interp.getInterpreter(), pObj, keyObj, valObj);
}

// MPI Wrapper:

CMirrorCommand::CMirrorCommand(CTCLInterpreter& rInterp) :
    CMPITclCommand(rInterp, "mirror", new CMirrorCommandActual(rInterp)) {}