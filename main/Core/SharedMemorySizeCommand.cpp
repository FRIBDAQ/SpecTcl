#include "SharedMemorySizeCommand.h"

#include <TCLInterpreter.h>

#include <vector>
#include <iostream>
#include <unistd.h>

CSharedMemorySizeCommand::CSharedMemorySizeCommand(CTCLInterpreter& rInterp)
    : CTCLObjectProcessor(rInterp, "shmemsize", true)
{
}


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
