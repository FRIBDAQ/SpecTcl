#ifndef CSPECTRAPROCESS_H
#define CSPECTRAPROCESS_H

#include "Subprocess.h"

namespace Spectra
{

/*!
 * \brief The CSpectraProcess class
 *
 * Provides the logic for launching the spectra program.
 */
class CSpectraProcess : public Subprocess
{
private:
    int m_pid;

public:
    CSpectraProcess();
    virtual ~CSpectraProcess();

    /*!
     * \brief Fork and execute the program
     */
    void exec();

    /*!
     * \brief Kill off the program if it is running
     */
    void kill();

    /*!
     * \brief Retrieve the pid
     * \return pid
     * \retval 0    process is not running
     * \retval >0   process is running
     */
    int getPid() const;
    bool isRunning() const;

    /*!
     * \brief Generate the file path to the executable
     *
     * The default location that is used is prefix/bin/spectra.
     * If the SPECTRA_EXECUTABLE_PATH environment variable is set,
     * it overrides the default location.
     *
     * \return path to executable
     */
    std::string generatePath() const;

};

}
#endif // CSPECTRAPROGRAM_H
