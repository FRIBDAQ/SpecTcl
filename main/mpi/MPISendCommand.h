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
#ifndef MPISENDCOMMAND_H
#define MPISENDCOMMAND_H

/** @file MPISendCOmmand.h
 *  @brief - header for implementing the mpi::send command class.
 */

#include "TCLObjectProcessor.h"

/**
 * @class CMPISendCommand
 *    This command is responsible for sending scripts to differing sets of ranks.
 * To be lazy, it's expected, that this command will be wrapped by an MPITclCommand all
 * and, then, in each of the actual ranks it's broadcast or executed in, decide if the script
 * shouild be executed or simple return an empty result and TCL_OK.
 * 
 * The basic form of the command is:
 * \verbatim
 *  mpi::send where script
 * \endverbatim
 * 
 * 
 * What makes implementation interesting are the large variety of 'where' formats that are supported:
 * 
 * -  ranks - a list of numeric ranks.
 * -  roles - a list of textual roles ('root', 'event-sink' and 'worker').
 * 
 * @note - if the environment is _not_ mpi, the script _is_ executed.
 * 
 */
class CMPISendCOmmand : public CTCLObjectProcessor {
public:
    CMPISendCommand(CTCLInterpreter& interp);
    virtual ~CMPISendCommand();

    int operator()(CTCLInterpreter& interp, std::vector<CTCLObject>& objv);
private:
    bool executeScript(CTCLObject& roleList);
    std::set<int> computeRoles(CTCLObject& roleList);
}

#endif