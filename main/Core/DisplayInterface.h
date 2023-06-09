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

#ifndef DISPLAYINTERFACE_H
#define DISPLAYINTERFACE_H

#include "DisplayCollection.h"
#include "DisplayFactory.h"

#include <map>
#include <string>

class CDisplay;

/*!
 * \brief Class that controls interaction with display system
 *
 *  The collection of displays that are in use, need to be owned by
 *  something and this class does that. It owns the actual
 */
class CDisplayInterface
{
public:
    enum Result { DISPLAY_EXISTS, NO_CREATOR, SUCCESS };

private:
    std::map<std::string, CDisplay*> m_displays;

    CDisplayCollection    m_collection;
    CDisplayFactory       m_factory;

public:
    CDisplayInterface();
    virtual ~CDisplayInterface();

    /*!
     * \brief Create a new display
     *
     * \param name    - name to associate with display
     * \param type    - type of display (creator must exist with this type)
     *
     * \return Result of operation
     * \retval DISPLAY_EXISTS   display has already been created
     * \retval SUCCESS          successful creation of display
     * \retval NO_CREATOR       no creator exists for the requested type
     */
    Result createDisplay(const std::string& name, const std::string& type);

    /*!
     * \brief Lookup a display by name
     * \param name  name of display
     *
     * \return CDisplay*
     *
     * \retval nullptr  if display not found
     * \retval pointer to display
     */
    CDisplay* getDisplay(const std::string& name);

    /*!
     * \return the current display
     * \retval nullptr is the current display has not be set
     */
    CDisplay* getCurrentDisplay();

    /*!
     * \brief Set current display by name
     *
     * \param name  name of display
     *
     * \return boolean
     * \retval true     display exists
     * \retval false    display not found
     */
    bool      setCurrentDisplay(const std::string& name);

    /*!
     * \brief Read-only access to the underlying display collection
     * \return
     */
    const CDisplayCollection& getCollection() const { return m_collection;}

    /*!
     * \brief Access the underlying factory
     * \return
     */
    CDisplayFactory& getFactory() { return m_factory;}

};

#endif // DISPLAYINTERFACE_H
