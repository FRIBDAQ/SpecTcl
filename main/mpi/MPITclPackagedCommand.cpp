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
#include "MPITclPackagedCommand.h"
#include "TclPump.h"
#include "TCLInterpreter.h"
#include "TCLObject.h"

/**
 *  constructor
 *  @param interp - references the interpreter.
 *  @param command - command that the object is registered as.
 *  @param pActual  - Pointer to the actual command.
 * 
 * @note it's perfectly fine to construct pActual with the same command
 *       string as we'll override that registration.
 *  @note ownership of pActual passes to this object and pActual must have been
 * created using **new**.
*/
CMPITclPackagedCommand::CMPITclPackagedCommand(CTCLInterpreter& interp, const char* command, CTCLPackagedObjectProcessor* pActual) :
    CTCLPackagedObjectProcessor(interp, command, true),
    m_pActualCommand(pActual)
    {
    }
/***
 *  destructor:
 *    In addition to unregistering us we destroy the actual command
 * which is assumed to have been instantiated via **new**.
*/
CMPITclPackagedCommand::~CMPITclPackagedCommand() {
    delete m_pActualCommand;
}
/**
 *  operator()  
 *    If not an mpi app, just pass on to the actual command,
 * otherwise, if rank 0, use the TclPump module's ExecCommand
 * method to pass the command data on.
 * 
 * @param interp - interpreter 
 * @param objv   - The command words.
 * @return int - command status (e.g. TCL_OK)
*/
int
CMPITclPackagedCommand::operator()(CTCLInterpreter& interp, std::vector<CTCLObject>& objv) {
    if (isMpiApp() && (myRank() == 0)) {
        // Mater in MPI Environment
        return ExecCommand(interp, objv);
    } else {
        // Either not an MPI app or i'm a slave processor
        return (*m_pActualCommand)(interp, objv);
    }
}

