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

#include "GateEditRequest.h"

#include "GlobalSettings.h"
#include "GSlice.h"
#include "GGate.h"

#include <iostream>
#include <sstream>

namespace Viewer
{

GateEditRequest::GateEditRequest(const GGate& cut)
    : m_reqStr()
{
    QString server = GlobalSettings::getServer();

    m_reqStr = server + "/spectcl/gate/edit";
    m_reqStr += QString("?name=") + cut.getName();
    
    
    SpJs::GateType type = cut.getType();
    if ((type == SpJs::BandGate) || (type == SpJs::ContourGate)) { // 2d gate.
        if (type == SpJs::BandGate) {
          m_reqStr += QString("&type=b");
        } else {
          m_reqStr += QString("&type=c");
        }
        m_reqStr += QString("&xparameter=") + cut.getParameterX();
        m_reqStr += QString("&yparameter=") + cut.getParameterY();
    } else {                                                    // 2d gamma gate.
        if (type == SpJs::GammaContourGate) {
            m_reqStr += QString("&type=gc");
        } else {
            m_reqStr += QString("&type=gb");
        }
        // Now the parameters:
        
        std::set<QString> params = cut.getParameters();
        for (auto p = params.begin(); p != params.end(); p++) {
            m_reqStr += QString("&parameter=");
            m_reqStr += *p;
        }
        
    }
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
/**
 * Constructor for cuts.
 *   The gate type depends on the number of  parameters the gate has:
 *   *  1 parameter means this is a slice (s).
 *   *  2 parameters means this is a gamma slice (gs).
 */
GateEditRequest::GateEditRequest(const GSlice &slice)
    : m_reqStr()
{
    QString server = GlobalSettings::getServer();

    // This part of the URI is gate type independent:
    
    m_reqStr = server + "/spectcl/gate/edit";
    m_reqStr += QString("?name=") + slice.getName();
    m_reqStr += QString("&high=") + QString::number(slice.getXHigh());
    m_reqStr += QString("&low=")  + QString::number(slice.getXLow());
    
    // the rest depends on the # of parameters:
    
    if (slice.parameterCount() == 1) {
            // Slice
            
        m_reqStr += QString("&type=s");
        m_reqStr += QString("&parameter=") + slice.getParameter();
    } else {
        // Gamma slice. Has a parameter query param for each parameter:
        
        m_reqStr += QString("&type=gs");
        int np = slice.parameterCount();
        for (int i =0; i < np; i++) {
            m_reqStr += QString("&parameter=");
            m_reqStr += slice.getParameter(i);
        }
    }

}

/**
 * constructor (compound gates).
 *
 *   This construtor builds a URL to create a compound gate.  The form
 *   of that url is /spectcl/gate/edit?name=name&type=whatevber&gates={gate1 ... gaten}
 *
 *   Where whatever is one of +, * or - (or, and, negation).  Gate1..gaten are
 *   the component gates.
 *
 *   @note For - gates, no effort is made here to ensure there's only one
 *         component gate.
 *
 * @param name - Name of the gate.
 * @param type - Type of compound gate (no validity checking is done here).
 * @param components - vector of component gates.
 */
GateEditRequest::GateEditRequest(
    const std::string& name, const char* compoundType,
    const std::vector<std::string> components
)
{
    std::string t = compoundType;
    // + has to be encoded as it's otherwise treated as a space by URL decoders:
    
    if (t == "+") {
        t = "%2B";
    }
    
    std::stringstream urlStream;
    urlStream << GlobalSettings::getServer().toStdString()  << "/spectcl/gate/edit?name=" << name
                <<  "&type=" << t << "&gate=";
    for (int i = 0; i < components.size(); i++) {
        urlStream << components[i] << " ";
    }
    std::string urlString = urlStream.str();
    m_reqStr = urlString.c_str();
}
/**
 * Convert a gate edit request to a valid URL (% encodes the raw URL string as needed).
 */
QUrl GateEditRequest::toUrl()
{
    return QUrl(m_reqStr);
}

bool GateEditRequest::isBand(const GGate& cut)
{
    return (cut.getType() == SpJs::BandGate );
}

}
