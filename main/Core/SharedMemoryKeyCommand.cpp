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

CSharedMemoryKeyCommand::CSharedMemoryKeyCommand(CTCLInterpreter &rInterp,
                                                 SpecTcl& rSpecTcl)
    : CTCLObjectProcessor(rInterp, "shmemkey", true),
      m_rSpecTcl(rSpecTcl)
{
}


CSharedMemoryKeyCommand::~CSharedMemoryKeyCommand()
{
}

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
