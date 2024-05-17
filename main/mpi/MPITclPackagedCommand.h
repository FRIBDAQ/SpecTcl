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
 * In mpiSpecTcl this is exactly like MPITclCommand but derived
 * from CTCLObjectProcessor.
*/
#ifndef MPITCLPACKAGEDCOMMAND_H
#define MPITCLPACKAGEDCOMMAND_H

#define protected public
#include "TCLPackagedObjectProcessor.h"
#undef protected
class CTCLInterpreter;
class CTCLObject;

/**
 *  CMPITCLPackagedCommand
 *     A packaged command that delegates to all orther ranks but 0.  Note that the while
 *   we don't require the encapsulated packaged command be registered on the same package as us
 *   that's the normal use case.
*/
class CMPITclPackagedCommand : public CTCLPackagedObjectProcessor 
{
private:
    CTCLPackagedObjectProcessor* m_pActualCommand;
public:
    CMPITclPackagedCommand(CTCLInterpreter& interp, const char* command, CTCLPackagedObjectProcessor* pActual);
    virtual ~CMPITclPackagedCommand();
protected:
    virtual int operator()(CTCLInterpreter& interp, std::vector<CTCLObject>& objv);
};

#endif