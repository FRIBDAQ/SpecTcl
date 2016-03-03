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


#ifndef VIEWER_SPECTCLINTERFACECONTROL_H
#define VIEWER_SPECTCLINTERFACECONTROL_H

#include "SpecTclInterfaceObserver.h"

#include <list>
#include <memory>

namespace Viewer {

class SpecTclInterface;

/*!
 * \brief The SpecTclInterfaceControl class
 *
 *  Controls the unique instance of the SpecTclInterface for the program.
 *  The SpecTclInterface is not a singleton becuase the program supports
 *  switching between local and remote.
 */
class SpecTclInterfaceControl
{
public:

    SpecTclInterfaceControl(std::shared_ptr<SpecTclInterface> pInterface);

    /*!
     * \brief getInterface
     * \return pointer to the interface maintained by this
     */
    std::shared_ptr<SpecTclInterface> getInterface() const {
        return m_pInterface;
    }

    /*!
     * \brief Pass in a new interface
     *
     * \param pInterface the new interface
     *
     * The interface has two bits of state that are copied appropriately
     * to the new interface. The new interface is set up to have the same polling
     * state as the old interface.
     */
    void setInterface(std::shared_ptr<SpecTclInterface> pInterface);

    /*!
     * \brief addSpecTclInterfaceObserver
     *
     * \param pObserver  the observer
     *
     *  The ownership is transferred from the caller to this.
     */
    void addSpecTclInterfaceObserver(std::unique_ptr<SpecTclInterfaceObserver> pObserver);

    /*!
     * \brief Adds a generic spectcl interface observer
     *
     * \param obj   reference to object requiring notification
     *
     * This is really a convenience method for adding SpecTclInterfaceObservers.
     * It leverages the GenericSpecTclInterfaceObserver template to wrap the
     * object passed in. The only thing required of the object of type T is that
     * it defines a method setSpecTclInterface(std::shared_ptr<SpecTclInterface>).
     */
    template<class T> void addGenericSpecTclInterfaceObserver(T& obj);

    /*!
     * \brief Notify observers of a change in SpecTclInterface
     *
     * The update() method of the interface observers is called for all known
     * observers in the order they were added.
     */
    void notifyObservers();

private:
    std::shared_ptr<SpecTclInterface>                     m_pInterface;
    std::list<std::unique_ptr<SpecTclInterfaceObserver> > m_interfaceObservers;

};


// implementation of template functions
template<class T>
void SpecTclInterfaceControl::addGenericSpecTclInterfaceObserver(T& obj) {

    using GenericObserver = GenericSpecTclInterfaceObserver<T>;

    std::unique_ptr<SpecTclInterfaceObserver> pObserver(new GenericObserver(obj));

    addSpecTclInterfaceObserver(std::move(pObserver));

}

} // namespace Viewer

#endif // VIEWER_SPECTCLINTERFACECONTROL_H
