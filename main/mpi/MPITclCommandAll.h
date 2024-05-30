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

/** CMPITclCommandAll - 
 *    This class is very much like CMPITclCommand The difference is that not only are
 * all other ranks told to execute the command, but so are we.  We, however execute the
 * command in the current scope rather than global scope.
*/
#ifndef MPITCLCOMMANDALL_H
#define MPITCLCOMMANDALL_H

/*  This dirty chicanery is because we want to directly call operator() of our
    encapsulated command but we can't do that since it's protected.  See
    MPITclCommand.h for more commentary.
*/
#define protected public
#include "TCLObjectProcessor.h"
#undef protected

class CMPITclCommandAll : public CTCLObjectProcessor {
private:
    CTCLObjectProcessor* m_pActualCommand;
public:
    CMPITclCommandAll(CTCLInterpreter& interp, const char* command, CTCLObjectProcessor* pActual);
    virtual ~CMPITclCommandAll();
protected:
    int operator()(CTCLInterpreter& interp, std::vector<CTCLObject>& objv);

};

#endif
