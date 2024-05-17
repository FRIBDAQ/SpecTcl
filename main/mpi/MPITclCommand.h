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
/**
 *  In mpiSpecTcl, a CMPITclCommand is a command that, in rank 0,
 *  sprays the command across the remainder of the application but, 
 *  in other ranks, runs an encapsulated CTCLObjectProcessor.
 *  
*/
#ifndef MPITCLCOMMAND_H
#define MPITCLCOMMAND_H
// A litte bit of dirt that's needed because
// operator() is protected in CTCLObjectProcessor and
// therefore command relaying can't be done unless we either do
// friendship (which is tough in a closed library) or:
#define protected public
#include "TCLObjectProcessor.h"
#undef protected
class CTCLObject;
class CTCLInterpreter;

/**
 * @class CMPITclCommand
 *    This class registers itself as a command in an interpreter _but_
 * it encapsualates another CTCLObjectProcessor.  If in the
 * mpi environment, it is rank 0, it pumps the command out to all other
 * processes for execution and captures the result.  If not rank 0,
 * it runs the encapsulated command (presumably as an event from the 
 * command pump).
 * 
*/
class CMPITclCommand : public CTCLObjectProcessor {
private:
    CTCLObjectProcessor* m_pActualCommand;
public:
    CMPITclCommand(CTCLInterpreter& interp, const char* command, CTCLObjectProcessor* pActual);
    virtual ~CMPITclCommand();
protected:
    int operator()(CTCLInterpreter& interp, std::vector<CTCLObject>& objv);

};


#endif