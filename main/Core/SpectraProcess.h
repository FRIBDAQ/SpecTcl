#ifndef CSPECTRAPROCESS_H
#define CSPECTRAPROCESS_H

#include "Subprocess.h"

namespace Spectra
{

class CSpectraProcess : public Subprocess
{
private:
    int m_pid;

public:
    CSpectraProcess();
    virtual ~CSpectraProcess();

    void exec();
    void kill();

    int getPid() const;
    bool isRunning() const;

    std::string generatePath() const;

};

}
#endif // CSPECTRAPROGRAM_H
