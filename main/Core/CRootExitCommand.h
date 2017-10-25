/*
    This software is Copyright by the Board of Trustees of Michigan
    State University (c) Copyright 2017.

    You may use this software under the terms of the GNU public license
    (GPL).  The terms of this license are described at:

     http://www.gnu.org/licenses/gpl.txt

     Authors:
             Ron Fox
             Jeromy Tompkins 
	     NSCL
	     Michigan State University
	     East Lansing, MI 48824-1321
*/

/** @file:  CRootExitCommand.h
 *  @brief: Replaces tcl exit command with one that terminates the root 
 */
#ifndef CROOTEXITCOMMAND_H
#define CROOTEXITCOMMAND_H
#include <TCLObjectProcessor.h>

class CTCLInterpreter;
class CTCLObject;

/**
 * @class CRootExitCommand
 *    This class is intended to replace the Tcl exit command with a command
 *    that does a gApplication->Terminate call.  This seems to exit in a
 *    saner manner than the Tcl exit which seems to yank some rugs out from underneath
 *    root in a manner that causes a segfault.
 *    Usage is identical to that of Tcl's exit:
 * \verbatim
 *    exit ?returnCode?
 *  \endverbatim
 *
 *    If not supplied the return code is 0 indicating normal exit.
 */
class CRootExitCommand : public CTCLObjectProcessor
{
public:
    CRootExitCommand(CTCLInterpreter& interp, const char* cmd);
    virtual ~CRootExitCommand();
    
    virtual int operator()(CTCLInterpreter& interp, std::vector<CTCLObject>& objv);
};

#endif