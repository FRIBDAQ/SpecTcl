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

/** @file:  CRemoteCommand.h
 *  @brief: Provides a command that can be used to test if a UI is remote or local.
 */

#ifndef CREMOTECOMMAND_H
#define CREMOTECOMMAND_H

#include <TCLObjectProcessor.h>

class CTCLInterpreter;
class CTCLObject;

// IN MPI SpecTcl this is only registered/run in the MPI_ROOT_RANK.

/**
 * @class CRemoteCommand
 *     This is a simple command that always sets the result to 0.
 *     The idea is that there will also be an isRemote command in the
 *     SpecTcl REST command emulator that will return 1.  This allows scripts to
 *     Determine if they are running locally or remotely and take any necessary
 *     action regarding differences in the environments.
 */
class CRemoteCommand : public CTCLObjectProcessor
{
public:
    CRemoteCommand(CTCLInterpreter& interp);
    virtual ~CRemoteCommand();
    
    virtual int operator()(CTCLInterpreter& interp, std::vector<CTCLObject>& objv);
};


#endif