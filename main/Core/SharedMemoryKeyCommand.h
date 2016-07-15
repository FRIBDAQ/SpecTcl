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

#ifndef SHAREDMEMORYKEYCOMMAND_H
#define SHAREDMEMORYKEYCOMMAND_H

#include <TCLObjectProcessor.h>

class SpecTcl;

/*! Provides the shmemkey command
 *
 * The shmemkey command is a simple utility that provides the user access
 * the key of the shared memory. It is originally intended to provide clients
 * of the REST server plugin the ability to attach to the shared memory for
 * histogram content retrieval.
 *
 */
class CSharedMemoryKeyCommand : CTCLObjectProcessor
{
private:
    SpecTcl& m_rSpecTcl;

public:
    CSharedMemoryKeyCommand(CTCLInterpreter& rInterp, SpecTcl& rSpecTcl);
    CSharedMemoryKeyCommand(const CSharedMemoryKeyCommand& obj) = delete;
    virtual ~CSharedMemoryKeyCommand();

    int operator()(CTCLInterpreter& interp, std::vector<CTCLObject>& objv);
};

#endif // SHAREDMEMORYKEYCOMMAND_H
