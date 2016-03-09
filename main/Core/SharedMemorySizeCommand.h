#ifndef SHAREDMEMORYSIZECOMMAND_H
#define SHAREDMEMORYSIZECOMMAND_H

#include <TCLObjectProcessor.h>

class CTCLInterpreter;

class CSharedMemorySizeCommand : public CTCLObjectProcessor
{
public:
    CSharedMemorySizeCommand(CTCLInterpreter& rInterp);

    int operator()(CTCLInterpreter& rInterp, std::vector<CTCLObject>& obj);

};

#endif // SHAREDMEMORYSIZECOMMAND_H
