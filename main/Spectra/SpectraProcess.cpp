
#include "SpectraProcess.h"

#include <ErrnoException.h>

#include <string>
#include <cstdlib>

#include <unistd.h>
#include <fcntl.h>
#include <signal.h>


namespace Spectra
{

CSpectraProcess::CSpectraProcess()
{
}

CSpectraProcess::~CSpectraProcess() {
    if (m_pid != 0) {
        kill();
    }
}

void CSpectraProcess::exec()
{
    // a little bit of code to ensure that stdin, stdout, and stderr are
    // not closed that was stolen from the launch of Xamine in client.c.
    // It is not clear why it is needed. Effectively, it
    int inflg, errflg, outflg;

    inflg  = fcntl(0, F_GETFD, 0);
    outflg = fcntl(1, F_GETFD, 0);
    errflg = fcntl(2, F_GETFD, 0);
    fcntl(0, F_SETFD, 0);
    fcntl(1, F_SETFD, 0);
    fcntl(2, F_SETFD, 0);

    if (m_pid == 0) {
        m_pid = fork();
        if (m_pid == 0) {
            // child process
#ifdef SPECTRA_FILENAME
            std::string spectraPath = SPECTRA_FILENAME;
#else
            std::string spectraPath = getenv("SPECTRA_ENV_FILENAME");
#endif
            execv(spectraPath.c_str(), nullptr);
            return;
        } else {
            fcntl(0, F_SETFD, inflg);
            fcntl(1, F_SETFD, outflg);
            fcntl(2, F_SETFD, errflg);
        }
    }
}

void CSpectraProcess::kill()
{
    // don't try to kill a process that doesn't exist
    if (m_pid == 0) {
        return;
    }

    int status = ::kill(m_pid, SIGTERM);
    if (status < 0) {
        throw CErrnoException("CSpectraProcess::kill()");
    }

    m_pid = 0;

}

int CSpectraProcess::getPid() const
{
    return m_pid;
}

bool CSpectraProcess::isRunning() const
{
    return (m_pid != 0);
}

} // end namespace Spectra
