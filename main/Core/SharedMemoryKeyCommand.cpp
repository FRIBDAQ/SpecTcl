#include "SharedMemoryKeyCommand.h"
#include "SpecTcl.h"
#include "DisplayInterface.h"
#include "Display.h"
#include "NullDisplay.h"
#include "client.h"

#include "TCLInterpreter.h"
#include "TCLObject.h"

#include <stdexcept>
#include <iostream>

//
// Constructor
CSharedMemoryKeyCommand::CSharedMemoryKeyCommand(CTCLInterpreter &rInterp,
                                                 SpecTcl& rSpecTcl)
    : CTCLObjectProcessor(rInterp, "shmemkey", true),
      m_rSpecTcl(rSpecTcl)
{
}


CSharedMemoryKeyCommand::~CSharedMemoryKeyCommand()
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
CSharedMemoryKeyCommand::operator ()(CTCLInterpreter& interp,
                                     std::vector<CTCLObject>& objv)
{
    bindAll(interp, objv);
    requireAtMost(objv, 1, "Usage\n shmemkey");
    requireAtLeast(objv, 1, "Usage\n shmemkey");

    int status = TCL_ERROR;
    CDisplayInterface* pDisplayInterface = m_rSpecTcl.GetDisplayInterface();
    if (! pDisplayInterface )
        throw std::runtime_error("Cannot access shared memory, no displays exist.");

    auto pDisplay = pDisplayInterface->getCurrentDisplay();

    if (dynamic_cast<CNullDisplay*>(pDisplay) == nullptr) {
        CTCLObject result;

        char key[256];
        Xamine_GetMemoryName(key);
        result = key;

        interp.setResult(result);

        status = TCL_OK;
    }

    return status;
}
