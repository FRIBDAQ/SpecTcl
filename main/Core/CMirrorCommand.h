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

/** @file:  CMirrorCommand.h
 *  @brief: SpecTcl mirror command - list mirrors.
 */
#ifndef _CMIRRORCOMMAND_H
#define _CMIRRORCOMMAND_H

#include <TCLObjectProcessor.h>

class CTCLInterpreter;
class CTCLObject;

/**
 * @class CMirrorCommand
 *     Provides the Tcl 'mirror' command that lists the mirrors
 *     currently being maintained.   The command has the form of an ensemble
 *     in case later capabilities are required.  Currently the subcommands are
 *     only:
 *     -   list ?host-pattern?  lists the mirrors that are maintained
 *          that match the host-pattern (glob).  Note that if omitted,
 *          host_pattern defaults to * which matches all hosts.  The
 *          return from this subcommand is a list of dicts (possibly empty).
 *          Each dict contains the keys
 *          *  host  - the host on which the mirror is being maintained.
 *          *  shmkey - The shared memory key in which the mirror is maintained.
 */
class CMirrorCommand : public CTCLObjectProcessor
{
public:
    CMirrorCommand(CTCLInterpreter& interp);
    virtual ~CMirrorCommand();
    
    virtual int operator()(CTCLInterpreter& interp, std::vector<CTCLObject>&objv);
    
    // Subcommand executors:
private:
    void list(CTCLInterpreter& interp, std::vector<CTCLObject>&objv);

    // Private Utilities:

private:
    void addToDict(
        CTCLObject& dict, CTCLInterpreter& interp,
        const char* key, const char* value
    );
    
};


#endif