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


#ifndef MAIN_H
#define MAIN_H

#include <CmdLineOptions.h>

#include <QHostAddress>
#include <QList>

namespace Viewer {

/*!
 * \brief Encapsulation of the main method
 *
 * We encapsulate the main method so that we can do some testing of
 * functionality used at start up.
 */
class Main
{
private:
    CmdLineOptions m_opts;

public:
    Main();

    int operator()(int argc, char* argv[]);

    bool serverIsOnThisMachine(QString hostName);

    bool listsShareContent(QList<QHostAddress> list0, QList<QHostAddress> list1);
};


} // end Viewer namespace

#endif // MAIN_H
