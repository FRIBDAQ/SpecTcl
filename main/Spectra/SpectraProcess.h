#ifndef CSPECTRAPROCESS_H
#define CSPECTRAPROCESS_H

namespace Spectra
{

class CSpectraProcess
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

};

}
#endif // CSPECTRAPROGRAM_H
