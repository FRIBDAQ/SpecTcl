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

/** @file:  CPythonCommand.h
 *  @brief: Provides a command that allows python scripts to be executed.
 */
#ifndef CPYTHONCOMMAND_H
#define CPYTHONCOMMAND_H
#include <TCLObjectProcessor.h>


/**
 * @class CPythonCommand
 *    Provides the python command with the following
 *    subcommands:
 *    *  exec  the next parameter is a script that's executed.
 *    *  source the next parameter is a filename and the script in that file
 *              is executed.
 *    @note
 *      The python support part of this command provides a python command:
 *      * spectcl script   The script is executed in the Tcl interpreter
 *        run by SpecTcl and therefore has available all of the SpecTcl
 *        tcl command extensions.
 */
class CPythonCommand : public CTCLObjectProcessor
{
private:
    CTCLInterpreter&   m_Interp;          // interpreter python can exec Tcl on.
public:
    CPythonCommand(CTCLInterpreter& interp, const char* name);
    virtual ~CPythonCommand();
    
    virtual int operator()(CTCLInterpreter& interp, std::vector<CTCLObject>& objv);
protected:
    void execute(CTCLInterpreter& interp, std::vector<CTCLObject>& objv);
    void source(CTCLInterpreter& interp, std::vector<CTCLObject>& objv);
};


#endif