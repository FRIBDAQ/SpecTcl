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
#include <MPITclCommand.h>

class SpecTcl;
// In mpiSpecTcl, this runs in the event sink pipeline as that's where the
// shared memory is created and is the parent of any displayer.

/*! Provides the shmemkey command
 *
 * The shmemkey command is a simple utility that provides the user access
 * the key of the shared memory. It is originally intended to provide clients
 * of the REST server plugin the ability to attach to the shared memory for
 * histogram content retrieval.
 *
 */
class CSharedMemoryKeyCommandActual : public CTCLObjectProcessor
{
private:
    SpecTcl& m_rSpecTcl;

public:
    CSharedMemoryKeyCommandActual(CTCLInterpreter& rInterp, SpecTcl& rSpecTcl);
    CSharedMemoryKeyCommandActual(const CSharedMemoryKeyCommandActual& obj) = delete;
    virtual ~CSharedMemoryKeyCommandActual();

    int operator()(CTCLInterpreter& interp, std::vector<CTCLObject>& objv);
};

class CSharedMemoryKeyCommand : public CMPITclCommand {
public:
    CSharedMemoryKeyCommand(CTCLInterpreter& rInterp, SpecTcl& rSpecTcl);
    ~CSharedMemoryKeyCommand() {}
};

#endif // SHAREDMEMORYKEYCOMMAND_H
