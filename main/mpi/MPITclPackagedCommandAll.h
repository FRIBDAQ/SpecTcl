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
#ifndef MPITCLPACKAGEDCOMMANDALL_H
#define  MPITCLPACKAGEDCOMMANDALL_H
#define protected public
#include "TCLPackagedObjectProcessor.h"
#undef protected
class CTCLInterpreter;
class CTCLObject;
/**
 * In mpiSpecTcl an CMPITclCommandAll is like a CMPITclCommandAll that's in a package.
 * The encpasualted command must be a CTCLPackagedObjectProcessor.  While there's no
 * restriction on the package the encapsulated command is defined/registered on,
 * In general, it is registered on the same package as this. That's the normal use case.
 * Note that to be used properly, since the instantiation in the package must look something
 * like this:
 * \verbatim
 *    // package constructor
 * {
 *      auto pEncpasulated = new EnacpsulatedCommane(...);
 *      addCommand(pEncapsulated);
 *      auto pMpiCommand = new CMPITclPackagedCommandAll(rInterp, somecommand, pEncapsulated);
 *      addCommand(pMpiCommand);
 * ...
 * }
 * \endverbatim
 * 
 * and if the program registers the MPI command with the same name as the encapsulated command
 * (overriding it), this is the normal case and scripters only see the MPI command.
 * 
*/
class CMPITclPackagedCommandAll : public CTCLPackagedObjectProcessor {
private:
    CTCLPackagedObjectProcessor* m_pActualCommand;
public:
    CMPITclPackagedCommandAll(
        CTCLInterpreter& interp, const char* command, CTCLPackagedObjectProcessor* pActual
    );
    virtual ~CMPITclPackagedCommandAll();
    int operator()(CTCLInterpreter& interp, std::vector<CTCLObject>& objv);

};

#endif