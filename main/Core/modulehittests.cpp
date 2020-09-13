/*
    This software is Copyright by the Board of Trustees of Michigan
    State University (c) Copyright 2017.

    You may use this software under the terms of the GNU public license
    (GPL).  The terms of this license are described at:

     http://www.gnu.org/licenses/gpl.txt

     Authors:
             Ron Fox
             Giordano Cerriza
	     NSCL
	     Michigan State University
	     East Lansing, MI 48824-1321
*/

/** @file:  modulehittests.cpp
 *  @brief: Tests for CAENModuleHits.cpp.
 */
#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/Asserter.h>
#include "Asserts.h"
#include "CAENHit.h"
#include "CAENModuleHits.h"
#include <stdint.h>

class modhittest : public CppUnit::TestFixture {
    CPPUNIT_TEST_SUITE(modhittest);
    CPPUNIT_TEST(construct_1);
    
    CPPUNIT_TEST(hit_1);
    CPPUNIT_TEST(hit_2);
    CPPUNIT_TEST(hit_3);
    
    CPPUNIT_TEST(clear_1);
    CPPUNIT_TEST(copycons_1);
    CPPUNIT_TEST(assign_1);
    CPPUNIT_TEST_SUITE_END();
    
private:
    CAENModuleHits* m_pHits;
public:
    void setUp() {
        m_pHits = new CAENModuleHits(12);  // sid = 12.
    }
    void tearDown() {
        delete m_pHits;
    }
protected:
    void construct_1();
    void hit_1();
    void hit_2();
    void hit_3();
    void clear_1();
    void copycons_1();
    void assign_1();
};

CPPUNIT_TEST_SUITE_REGISTRATION(modhittest);

void modhittest::construct_1()
{
    // At construction time hits are empty and source id initialized.
    
    EQ(12, m_pHits->getSourceId());
    EQ(size_t(0), m_pHits->getHits().size());
}
void modhittest::hit_1()
{
    // Inserting a PHA hit gives us one hit and it's a PHA:
    
    CAENPHAHit hit;
    m_pHits->addHit(&hit);
    
    EQ(size_t(1), m_pHits->getHits().size());
    EQ((&hit), dynamic_cast<CAENPHAHit*>((m_pHits->getHits())[0]));
}
void modhittest::hit_2()
{
    // Insert a PSD hit gives us one hit and it's our hit.
    
    CAENPSDHit hit(1);
    m_pHits->addHit(&hit);
    
    EQ(size_t(1), m_pHits->getHits().size());
    EQ((&hit), dynamic_cast<CAENPSDHit*>(m_pHits->getHits()[0]));
}
void modhittest::hit_3()
{
    // Multiple hits also work:
    //
    CAENPHAHit hit1;
    CAENPSDHit hit2(1);
    m_pHits->addHit(&hit1);
    m_pHits->addHit(&hit2);
    
    EQ(size_t(2), m_pHits->getHits().size());
    EQ(&hit1, dynamic_cast<CAENPHAHit*>(m_pHits->getHits()[0]));
    EQ(&hit2, dynamic_cast<CAENPSDHit*>(m_pHits->getHits()[1]));
    
}
void modhittest::clear_1()
{
    // Clear flushesc contents of vector.
    
    CAENPHAHit hit1;
    CAENPSDHit hit2(1);
    m_pHits->addHit(&hit1);
    m_pHits->addHit(&hit2);
    
    m_pHits->clear();
    EQ(size_t(0), m_pHits->getHits().size());
}
void modhittest::copycons_1()
{
    // COpy construction:
    
    CAENPHAHit hit1;
    CAENPSDHit hit2(1);
    m_pHits->addHit(&hit1);
    m_pHits->addHit(&hit2);
    CAENModuleHits copy(*m_pHits);
    m_pHits->clear();
    
    EQ(12, copy.getSourceId());
    EQ(size_t(2), copy.getHits().size());
    EQ(&hit1, dynamic_cast<CAENPHAHit*>(copy.getHits()[0]));
    EQ(&hit2, dynamic_cast<CAENPSDHit*>(copy.getHits()[1]));
}

void modhittest::assign_1()
{
    // Test assignment
    
     CAENPHAHit hit1;
    CAENPSDHit hit2(1);
    m_pHits->addHit(&hit1);
    m_pHits->addHit(&hit2);
    CAENModuleHits copy;
    copy = (*m_pHits);
    m_pHits->clear();
    
    EQ(12, copy.getSourceId());
    EQ(size_t(2), copy.getHits().size());
    EQ(&hit1, dynamic_cast<CAENPHAHit*>(copy.getHits()[0]));
    EQ(&hit2, dynamic_cast<CAENPSDHit*>(copy.getHits()[1]));
}