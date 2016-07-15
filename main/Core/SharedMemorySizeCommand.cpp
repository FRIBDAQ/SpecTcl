/*
    This software is Copyright by the Board of Trustees of Michigan
    State University (c) Copyright 2016.

    You may use this software under the terms of the GNU public license
    (GPL).  The terms of this license are described at:

     http://www.gnu.org/licenses/gpl.txt

     Author:
       Scientific Software Team
	     NSCL
	     Michigan State University
	     East Lansing, MI 48824-1321
*/

#include "SharedMemorySizeCommand.h"

#include <TCLInterpreter.h>

#include <vector>
#include <iostream>
#include <unistd.h>

//
//
CSharedMemorySizeCommand::CSharedMemorySizeCommand(CTCLInterpreter& rInterp)
    : CTCLObjectProcessor(rInterp, "shmemsize", true)
{
}


/*! \brief Call operator
 *
 * The entry point of the shmemsize command logic. This will return the value of 
 * the DisplayMegabytes tcl variable regardless of whether or not a display exists.
 *
 * \param rInterp   the interpreter
 * \param objv      a list containing "shmemsize" as its sole element
 */
int CSharedMemorySizeCommand::operator()(CTCLInterpreter& rInterp,
                                        std::vector<CTCLObject>& objv)
{
    bindAll(rInterp, objv);
    requireAtMost(objv, 1, "Usage\n shmemsize");
    requireAtLeast(objv, 1, "Usage\n shmemsize");

    CTCLObject result;
    std::string sizeStr = rInterp.GlobalEval("set DisplayMegabytes");
    int size = std::atoi(sizeStr.c_str()) * 1024 * 1024;

    result = std::to_string(size);

    rInterp.setResult(result);

    return TCL_OK;
}
