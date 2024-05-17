#include "SharedMemoryKeyCommand.h"
#include "SpecTcl.h"
#include "DisplayInterface.h"
#include "Display.h"
#include "NullDisplay.h"
#include "client.h"
#include <Globals.h>
#include <TclPump.h>

#include "TCLInterpreter.h"
#include "TCLObject.h"

#include <stdexcept>
#include <iostream>

//
// Constructor
CSharedMemoryKeyCommandActual::CSharedMemoryKeyCommandActual(CTCLInterpreter &rInterp,
                                                 SpecTcl& rSpecTcl)
    : CTCLObjectProcessor(rInterp, "shmemkey", true),
      m_rSpecTcl(rSpecTcl)
{
}


CSharedMemoryKeyCommandActual::~CSharedMemoryKeyCommandActual()
{
}

/*! Call operator
 *
 *  The caller will be returned the  shared memory key in use by SPecTcl.
 *  This is only really useful if the caller wants to attach to the 
 *  shared memory themselves. 
 *
 *  This only succeeds if the display is not null. The reason for this is that
 *  there cannot be a shared memory key without a display. The shared memory is
 *  associated with spectcl.
 */
int
CSharedMemoryKeyCommandActual::operator ()(CTCLInterpreter& interp,
                                     std::vector<CTCLObject>& objv)
{
    if (gMPIParallel && (myRank() != MPI_EVENT_SINK_RANK)) {
        return TCL_OK;
    }
    bindAll(interp, objv);
    requireAtMost(objv, 1, "Usage\n shmemkey");
    requireAtLeast(objv, 1, "Usage\n shmemkey");

    int status = TCL_ERROR;
    CDisplayInterface* pDisplayInterface = m_rSpecTcl.GetDisplayInterface();
    if (! pDisplayInterface )
        throw std::runtime_error("Cannot access shared memory, no displays exist.");

    auto pDisplay = pDisplayInterface->getCurrentDisplay();


    CTCLObject result;

    char key[256];
    Xamine_GetMemoryName(key);
    result = key;

    interp.setResult(result);

    status = TCL_OK;

    return status;
}

// Construct the MPI wrapper:

CSharedMemoryKeyCommand::CSharedMemoryKeyCommand(CTCLInterpreter& rInterp, SpecTcl& rSpecTcl) :
    CMPITclCommand(rInterp, "shmemkey", new CSharedMemoryKeyCommandActual(rInterp, rSpecTcl)) {}