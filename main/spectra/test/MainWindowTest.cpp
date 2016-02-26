
#ifndef GGATETEST_H
#define GGATETEST_H

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/Asserter.h>

#include "Asserts.h"

#include "mainwindow.h"
#include "SpecTclInterfaceObserver.h"

#include <iostream>
#include <algorithm>
#include <memory>

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

class MainWindowTest : public CppUnit::TestFixture
{
  private:

  public:
    CPPUNIT_TEST_SUITE( MainWindowTest );
    CPPUNIT_TEST( notifyObservers_0 );
    CPPUNIT_TEST_SUITE_END();

  public:
    void setUp() {

    }
    void tearDown() {
    }

    void notifyObservers_0() {

        MainWindow main;
        unique_ptr<SpecTclInterfaceObserver> pObserver(new FakeObserver);
        FakeObserver* pObs = dynamic_cast<FakeObserver*>(pObserver.get());
        main.addSpecTclInterfaceObserver( move(pObserver) );

        main.notifyObservers();

        EQMSG("Registered observers should be notified", true, pObs->getUpdated());
    }

};

CPPUNIT_TEST_SUITE_REGISTRATION(MainWindowTest);

#endif


} // end of namespace
