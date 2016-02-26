//    This software is Copyright by the Board of Trustees of Michigan
//    State University (c) Copyright 2015.
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
#include "GateEditRequest.h"

#include "GlobalSettings.h"
#include "GSlice.h"
#include "GGate.h"

#include <iostream>

namespace Viewer
{

GateEditRequest::GateEditRequest(const GGate& cut)
    : m_reqStr()
{
    QString server = GlobalSettings::getServer();
    std::cout << server.toStdString() << std::endl;

    m_reqStr = server + "/spectcl/gate/edit";
    m_reqStr += QString("?name=") + cut.getName();

    if (isBand(cut)) {
      m_reqStr += QString("&type=b");
    } else {
      m_reqStr += QString("&type=c");
    }
    m_reqStr += QString("&xparameter=") + cut.getParameterX();
    m_reqStr += QString("&yparameter=") + cut.getParameterY();

    auto points = cut.getPoints();

    size_t nPoints = points.size();
    for (size_t i=0; i<nPoints; ++i) {
        QString index = QString::number(i);
        QString x = QString::number(points.at(i).first);
        QString y = QString::number(points.at(i).second);
        m_reqStr += QString("&xcoord(%1)=%2").arg(index).arg(x);
        m_reqStr += QString("&ycoord(%1)=%2").arg(index).arg(y);
    }
}

GateEditRequest::GateEditRequest(const GSlice &slice)
    : m_reqStr()
{
    QString server = GlobalSettings::getServer();

    m_reqStr = server + "/spectcl/gate/edit";
    m_reqStr += QString("?name=") + slice.getName();
    m_reqStr += QString("&type=s&low=") + QString::number(slice.getXLow());
    m_reqStr += QString("&high=") + QString::number(slice.getXHigh());
    m_reqStr += QString("&parameter=") + slice.getParameter();

}

QUrl GateEditRequest::toUrl()
{
    return QUrl(m_reqStr);
}

bool GateEditRequest::isBand(const GGate& cut)
{
    return (cut.getType() == SpJs::BandGate );
}

}
