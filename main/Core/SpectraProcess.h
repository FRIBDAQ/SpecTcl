//    This software is Copyright by the Board of Trustees of Michigan
//    State University (c) Copyright 2016.
//
//    This program is free software: you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation, either version 3 of the License, or
//    any later version.
//
//    This program is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License
//    along with this program.  If not, see <http://www.gnu.org/licenses/>.
//
//    Authors:
//    Jeromy Tompkins
//    NSCL
//    Michigan State University
//    East Lansing, MI 48824-1321

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
