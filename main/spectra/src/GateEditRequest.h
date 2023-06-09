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

#ifndef GATEEDITREQUEST_H
#define GATEEDITREQUEST_H

#include <QString>
#include <QUrl>
#include<vector>
#include<string>

namespace Viewer
{

class GSlice;
class GGate;

/*!
 * \brief Logic for building an edit request
 *
 * This provides a centralized location for generating an edit
 * request for either a GGate or a GSlice
 */
class GateEditRequest
{
public:
    GateEditRequest(const GGate &cut);
    GateEditRequest(const GSlice& cut);
    GateEditRequest(
        const std::string& name, const char* compoundType,
        const std::vector<std::string> components
    );

    /*!
     * \brief Retrieve the url
     * \return the url
     */
    QUrl toUrl();

private:
    bool isBand(const GGate& cut);

private:
    QString m_reqStr;
};

} // end of namespace

#endif // GATEEDITREQUEST_H

