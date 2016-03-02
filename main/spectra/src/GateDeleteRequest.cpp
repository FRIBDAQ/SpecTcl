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

#include "GateDeleteRequest.h"

#include "GlobalSettings.h"
#include "GSlice.h"
#include "GGate.h"
#include <TCutG.h>

#include <iostream>

namespace Viewer
{

GateDeleteRequest::GateDeleteRequest(const QString& name) 
 : m_reqStr()
{
    QString server = GlobalSettings::getServer();

    m_reqStr = server + "/spectcl/gate/delete?name=";
    m_reqStr += name;
}

GateDeleteRequest::GateDeleteRequest(const GGate& cut)
    : m_reqStr()
{
    QString server = GlobalSettings::getServer();

    m_reqStr = server + "/spectcl/gate/delete";
    m_reqStr += QString("?name=") + cut.getName();
}

GateDeleteRequest::GateDeleteRequest(const GSlice &slice)
    : m_reqStr()
{
    QString server = GlobalSettings::getServer();

    m_reqStr = server + "/spectcl/gate/delete";
    m_reqStr += QString("?name=") + slice.getName();
}

QUrl GateDeleteRequest::toUrl()
{
    return QUrl(m_reqStr);
}

} // end of namespace
