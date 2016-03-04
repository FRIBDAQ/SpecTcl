#ifndef SHAREDMEMORYKEYCOMMAND_H
#define SHAREDMEMORYKEYCOMMAND_H

#include <TCLObjectProcessor.h>

class SpecTcl;

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
