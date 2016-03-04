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

#ifndef DISPLAYFACTORY_H
#define DISPLAYFACTORY_H

#include <map>
#include <string>

class CDisplay;

/*!
 * \brief Base class for display creators
 *
 * These can be loaded into the CDisplayFactory to extend
 * the types of displays that can be created.
 */
class CDisplayCreator {
 public:
    virtual CDisplay* create() = 0;
};

//-----------------------------------------------------------------------
/** Extensible factory for displays
 *
 *  Users can load up a display factory with any type of display creator.
 *  The creators are uniquely identified by a string key. In other words,
 *  you can store multiple creators for the same type of display so long
 *  as they are stored under different keys. Creators can be added or
 *  removed from the factory.
 *
 *  Creators that are registered to the factory are owned by the factory.
 *
 * \todo Make this store function objects rather than CDisplayCreator
 *       so that users can define a creator. i.e.
 *  \code
 *      CDispay* myfunc() { return new MyDisplay(); }
 *      CDisplayFactory factory;
 *      factory.addCreator("mydisp", function<CDisplay*(void)>(myfunc));
 *  \endcode
 */
class CDisplayFactory
{
    std::map<std::string, CDisplayCreator*> m_creators;

public:
    CDisplayFactory();
    ~CDisplayFactory();

    /*!
     * \brief Create a display using specific creator
     *
     * Ownership of the created object is transferred to the caller.
     * If there is no creator for the display, nullptr is returned.
     *
     * \param type  name of creator to use
     * \return object newed into existence if creator exists, nullptr otherwise
     */
    CDisplay* create(const std::string& type);

    /*!
     * \brief Access a specific creator
     *
     * One could use this method to adjust settings on a specific creator.
     * User is returned a pointer to object (if it is found). There is no
     * ownership transfer.
     *
     * \param type  name of creator to use
     * \return pointer to creator if it exists, nullptr otherwise
     */
    CDisplayCreator* getCreator(const std::string& type);

    /*!
     * \brief Insert a new creator
     *
     * Ownership of the object transfers to manager. The creator must have
     * been created on the heap so that it does not go out of scope. We store
     * the instance passed in.
     *
     * \param type      name of creator
     * \param rCreator  reference to a dynamically allocated creator object
     * \return boolean
     * \retval false - a creator already exists with specified type
     * \retval true  - creator was successfully inserted.
     */
    bool addCreator(const std::string& type, CDisplayCreator& rCreator);

    /*!
     * \brief Remove a specific creator
     *
     *  Ownership of the creator returned is passed to the caller.
     *
     * \param type  name of creator to remove
     * \return pointer to creator if it exists, nullptr otherwise
     */
    CDisplayCreator* removeCreator(const std::string& type);

};

#endif // DISPLAYFACTORY_H
