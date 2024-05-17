/*
    This software is Copyright by the Board of Trustees of Michigan
    State University (c) Copyright 2014.

    You may use this software under the terms of the GNU public license
    (GPL).  The terms of this license are described at:

     http://www.gnu.org/licenses/gpl.txt

     Authors:
             Ron Fox
             Giordano Cerizza
             Simon Giraud
             Aaron Chester
             Jin Hee Chang
	     FRIB
	     Michigan State University
	     East Lansing, MI 48824-1321
*/
#include "MPITclPackagedCommandAll.h"
#include "TCLInterpreter.h"
#include "TCLObject.h"
#include "TclPump.h"

#include <string>

/** constructor
 *   @param interp - references the interpreter that we will register our command on.
 *   @param command - The command string that will invoke this command.
 *   @param actual - the actual command being wrapped.  This is assumed to have been 'new'ed into
 *       existance and ownership passes to this objects.
*/
CMPITclPackagedCommandAll::CMPITclPackagedCommandAll(CTCLInterpreter& interp, const char* command, CTCLPackagedObjectProcessor* pActual) :
    CTCLPackagedObjectProcessor(interp, command, true),
    m_pActualCommand(pActual) {}

/**
 * destructor
*/
CMPITclPackagedCommandAll::~CMPITclPackagedCommandAll() {
    delete m_pActualCommand;
}

/**
 * operator()
 *   - Always execute locally.
 *   - If in the MPI env , use ExecuteCommand to execute in all other ranks.
 *   - Figure out the status/result.
 * 
 *  @param interp - interpreter running the command.
 *  @param objv   - CTCLObject encapsulated command words.
 *  @return int (TCL_OK or TCL_ERROR most likely). 
*/
int
CMPITclPackagedCommandAll::operator()(CTCLInterpreter& interp, std::vector<CTCLObject>& objv) {
    int localStatus = (*m_pActualCommand)(interp, objv);
    std::string localResult = interp.GetResultString();

    int remoteStatus = TCL_OK;
    std::string remoteResult;
    if (isMpiApp() && (myRank() == 0)) {
        remoteStatus = ExecCommand(interp, objv);
        remoteResult = interp.GetResultString();
    }
    // Sort out the status and result:

    // Remote execution was not ok accept the result and status from it.
    if (remoteStatus != TCL_OK) {
        return remoteStatus;
    }
    // Remote status is ok, but local is not -- use the local status and result:
    if (localStatus != TCL_OK) {
        interp.setResult(localResult);
        return localStatus;
    }
    // All are ok .. return the longest result:

    if (localResult.size() > remoteResult.size()) {
        interp.setResult(localResult);
        return TCL_OK;
    } else {
        return TCL_OK;
    }
   
}