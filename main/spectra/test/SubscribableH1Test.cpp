
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


#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/Asserter.h>

#include <Asserts.h>

#include "GlobalSettings.h"
#include "SubscribableH1.h"
#include "TestH1Subscriber.h"
#include <TH1.h>


using namespace std;

namespace Viewer
{

class SubscribableH1Test : public CppUnit::TestFixture
{

  public:
    CPPUNIT_TEST_SUITE( SubscribableH1Test );
    CPPUNIT_TEST( subscribe_0 );
    CPPUNIT_TEST( subscribe_1 );
    CPPUNIT_TEST( notify_0 );
    CPPUNIT_TEST( notify_1 );
    CPPUNIT_TEST( unsubscribe_0 );
    CPPUNIT_TEST_SUITE_END();

  public:
    void setUp() {
    }
    void tearDown() {
    }

  protected:
    void subscribe_0()
    {
        TestH1Subscriber sub;

        SubscribableH1<TH1D> hist("test", "test", 10, 0, 10);
        hist.subscribe(sub);

        EQMSG("subscription works", true, hist.isSubscribed(sub));

    }

    void subscribe_1()
    {
        TestH1Subscriber sub;

        SubscribableH1<TH1D> hist("test", "test", 10, 0, 10);
        EQMSG("subscription tells the truth about unsubscribed entities",
              false, hist.isSubscribed(sub));
    }

    void notify_0()
    {
        TestH1Subscriber sub;

        {
            SubscribableH1<TH1D> hist("test", "test", 10, 0, 10);
            hist.subscribe(sub);
        }
        EQMSG("subscriber was notified when hist left scope",
              true, sub.isNotified());
    }


    void notify_1()
    {
        TestH1Subscriber sub;

        SubscribableH1<TH1D> hist("test", "test", 10, 0, 10);
        hist.subscribe(sub);

        EQMSG("subscriber is not notified while hist in scope",
              false, sub.isNotified());
    }


    void unsubscribe_0() {
        TestH1Subscriber sub;

        {
            SubscribableH1<TH1D> hist("test", "test", 10, 0, 10);
            hist.subscribe(sub);
            hist.unsubscribe(sub);
        }

        EQMSG("entity doesn't get notified if unsubscribed",
              false, sub.isNotified());
    }
};

CPPUNIT_TEST_SUITE_REGISTRATION(SubscribableH1Test);

} // end of namespace
