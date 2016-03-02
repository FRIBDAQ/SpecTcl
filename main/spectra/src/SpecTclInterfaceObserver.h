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


#ifndef SPECTCLINTERFACEOBSERVER_H
#define SPECTCLINTERFACEOBSERVER_H

#include <memory>

namespace Viewer {

// Forward declarations
class SpecTclInterface;

/// \class SpecTclInterfaceObserver
///
/// There should be be a single instance of the SpecTclInterface that exists at
/// any given time in the program. This instance can be swapped out though with a
/// different instance. When that happens the SpecTclInterface is replaced,
/// all of the dependent objects (HistogramView, ControlPanel, etc.) need to be passed
/// the new SpecTclInterface so that they can set up their respective signals and slots.
/// Derived classes of this interface can be registered to the MainWindow to be
/// notified when a change happens.
class SpecTclInterfaceObserver {
public:
    virtual ~SpecTclInterfaceObserver() {}
    virtual void update(std::shared_ptr<SpecTclInterface> pInterface ) = 0;
};

/// \class GenericSpecTclInterfaceObserver
///
/// A generic wrapper to implement the observer dependencies for the
/// SpecTclInterface. This is simply an adaptor class that delegates to an object
/// it maintains a reference to.
template<class T>
class GenericSpecTclInterfaceObserver : public SpecTclInterfaceObserver {
private:
    T* m_pObj; ///< the object to delegate to

public:
    GenericSpecTclInterfaceObserver(T& obj) : m_pObj(&obj) {}
    GenericSpecTclInterfaceObserver(const GenericSpecTclInterfaceObserver& rhs)
        : m_pObj(rhs.m_pObj) {}
    ~GenericSpecTclInterfaceObserver() {}

    GenericSpecTclInterfaceObserver& operator=(const GenericSpecTclInterfaceObserver& rhs) {
        if (this != &rhs) {
            m_pObj = rhs.m_pObj;
        }
        return *this;
    }

    /*! Pass the SpecTclInterface to the object
     *
     *  \param pInterface   the new instance of SpecTclInterface
     */
    void update(std::shared_ptr<SpecTclInterface> pInterface) {
        m_pObj->setSpecTclInterface(pInterface);
    }
};

} // end of namespace

#endif // SPECTCLINTERFACEOBSERVER_H
