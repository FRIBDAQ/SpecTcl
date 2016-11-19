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

#ifndef SPECTCLINTERFACEFACTORY_H
#define SPECTCLINTERFACEFACTORY_H

#include <memory>

namespace Viewer {

// Forward declarations
class SpecTclInterface;


/*!
 * \brief Factory to produce SpecTclInterface instances
 *
 * This constructs only Hybrid and REST type interfaced. The C++ types that
 * pertain to this are:
 *  REST   -> SpecTclRESTInterface
 *  Hybrid -> SpecTclShMemInterface
 *
 */
class SpecTclInterfaceFactory
{
public:
    enum InterfaceType { REST, Hybrid };

public:
    // Canonicals
    SpecTclInterfaceFactory() = default;
    ~SpecTclInterfaceFactory() = default;
    SpecTclInterfaceFactory(const SpecTclInterfaceFactory& rhs) = default;

    /*!
     * \brief create a new SpecTclInterface
     *
     * \param type  type of interface to construct
     *
     * The caller clearly is passed ownership of the object that is created
     * by this.
     *
     * \return new SpecTclInterface
     */
    std::unique_ptr<SpecTclInterface> create(InterfaceType type);

    std::unique_ptr<SpecTclInterface> createShMemInterface();

    void setUpShMemEnv();
    void setUpShMemSizeEnv();
    void setUpShMemKeyEnv();
};

} // end of namespace
#endif // SPECTCLINTERFACEFACTORY_H
