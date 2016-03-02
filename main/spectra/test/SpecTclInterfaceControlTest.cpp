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


#ifndef SPECTCLINTERFACECONTROLTEST_H
#define SPECTCLINTERFACECONTROLTEST_H

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/Asserter.h>
#include "Asserts.h"

#include "SpecTclInterfaceControl.h"
#include "SpecTclInterfaceObserver.h"
#include "SpecTclRESTInterface.h"

#include <iostream>
#include <algorithm>

using namespace std;

namespace Viewer
{

class FakeObserver : public SpecTclInterfaceObserver {
private:
    bool m_updated;
    public:
    FakeObserver() : m_updated(false) {}

    void update(std::shared_ptr<SpecTclInterface> pInterface) {
        m_updated = true;
    }

    bool getUpdated() const { return m_updated; }
};

class Dummy {
public:
    void setSpecTclInterface(std::shared_ptr<SpecTclInterface> ) {}
};


class SpecTclInterfaceControlTest : public CppUnit::TestFixture
{
  public:
    CPPUNIT_TEST_SUITE( SpecTclInterfaceControlTest );
    CPPUNIT_TEST(notifyObservers_0);
    CPPUNIT_TEST(addGenericObserver_0);
    CPPUNIT_TEST_SUITE_END();

  public:
    void setUp() {
        }
    void tearDown() {
    }

  protected:
    void notifyObservers_0() {

        SpecTclInterfaceControl control(new SpecTclRESTInterface);

        unique_ptr<SpecTclInterfaceObserver> pObserver(new FakeObserver);

        FakeObserver* pObs = dynamic_cast<FakeObserver*>(pObserver.get());
        control.addSpecTclInterfaceObserver( move(pObserver) );

        control.notifyObservers();

        EQMSG("Registered observers should be notified", true, pObs->getUpdated());
    }

    void addGenericObserver_0() {
        SpecTclInterfaceControl control(new SpecTclRESTInterface);

        Dummy dumb;
        control.addGenericSpecTclInterfaceObserver(dumb);

        EQMSG("Registering generic observer succeeds",
              size_t(1), control.getObservers().size());
    }
};

CPPUNIT_TEST_SUITE_REGISTRATION(SpecTclInterfaceControlTest);

#endif


} // end of namespcae
