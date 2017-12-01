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
#ifndef CROOTEXEC_H
#define CROOTEXEC_H
/** @file:  CRootExec.h
 *  @brief: Command handler for rootexec - execute root macro file.
 */

#include <TCLObjectProcessor.h>

class CTCLInterpreter;
class CTCLObject;

/**
 * @class CRootExec
 *   Provides the rootexec command.  This command is of the form:
 *
 *\verbatim
 *   rootexec RootMacroFile
 *
 *   Where:
 *       RootMacroFile - is a filename containing root Macro(s).
 */
class CRootExec : public CTCLObjectProcessor
{
public:
    CRootExec(CTCLInterpreter& interp, const char* command);
    virtual ~CRootExec();
    
    virtual int operator()(CTCLInterpreter& interp, std::vector<CTCLObject>& objv);
};


#endif