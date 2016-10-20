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

#include "CmdLineOptions.h"

namespace Viewer {

CmdLineOptions::CmdLineOptions() : m_args()
{
}

/*!
 * \brief Parse the command line options
 *
 * \param argc  size of argv
 * \param argv  array of command line option string (including name of program)
 *
 * The method basically just calls cmdline_parser from the gengetopt.
 */
void CmdLineOptions::parse(int argc, char *argv[]) {

    m_args.resize(argc);
    for (int i=0; i<argc; ++i) {
        m_args[i] = argv[i];
    }

    cmdline_parser(argc, argv, &m_info);
}

/*!
 * \return port number server is listening on
 */
int CmdLineOptions::getPort() {
    return m_info.port_arg;
}

/*!
 * \return name hostname of REST server
 */
QString CmdLineOptions::getHost()
{
    return QString(m_info.host_arg);
}

/*!
 * \return whether the true type fonts are enabled for use in ROOT
 */
bool CmdLineOptions::disableTrueTypeFonts() const
{
    return m_info.disable_truetype_flag;
}

} // namespace Viewer
