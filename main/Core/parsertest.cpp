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

/** @file:  parsertest.cpp
 *  @brief:  Test CAENParser class.
 */
#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/Asserter.h>
#include "Asserts.h"
#define private public
#include "CAENParser.h"
#undef private
#include "CAENHit.h"
#include "CAENModuleHits.h"



class parsertest : public CppUnit::TestFixture {
    
private:
    CAENParser* m_pParser;
public:
    void setUp() {
        m_pParser = new CAENParser;
    }
    void tearDown() {
        delete m_pParser;    
    }
private:
    CPPUNIT_TEST_SUITE(parsertest);
    CPPUNIT_TEST(construct_1);
    
    CPPUNIT_TEST(add_1);
    CPPUNIT_TEST(add_2);
    CPPUNIT_TEST(add_3);
    CPPUNIT_TEST_SUITE_END();

protected:
    void construct_1();
    
    void add_1();
    void add_2();
    void add_3();
};

CPPUNIT_TEST_SUITE_REGISTRATION(parsertest);

void parsertest::construct_1()
{
    // without something to parse there are no hit modules:
    
    EQ(size_t(0), m_pParser->getHitModules().size());
    EQ(size_t(0), m_pParser->m_modules.size());
}
void parsertest::add_1()
{
    // Add PSD and check structures:
    
    m_pParser->addModule(1, CAENHit::PSD);
    EQ(size_t(1), m_pParser->m_modules.size());
    const CAENParser::ModuleInfo& info = m_pParser->m_modules[1];
    
    EQ(CAENHit::PSD, info.s_hitType);
    EQ(0, info.s_multiplier);
    EQ(1, info.s_module.getSourceId());
    EQ(size_t(0), info.s_module.getHits().size());
    
}
void parsertest::add_2()
{
    // ADD PHA and check structures:
    
    m_pParser->addModule(2, CAENHit::PHA);
    
    EQ(size_t(1), m_pParser->m_modules.size());
    const CAENParser::ModuleInfo& info = m_pParser->m_modules[2];
    
    EQ(CAENHit::PHA, info.s_hitType);
    EQ(0, info.s_multiplier);
    EQ(2, info.s_module.getSourceId());
    EQ(size_t(0), info.s_module.getHits().size());
}
void parsertest::add_3()
{
    // Can add more than one and they're kept straight:
    
    m_pParser->addModule(1, CAENHit::PSD);
    m_pParser->addModule(2, CAENHit::PHA);
    
    EQ(size_t(2), m_pParser->m_modules.size());\
    // Using blocks like this allows a re-use of the assertions
    //  from add_1, add_3
    
    {
        const CAENParser::ModuleInfo& info = m_pParser->m_modules[1];
        EQ(CAENHit::PSD, info.s_hitType);
        EQ(0, info.s_multiplier);
        EQ(1, info.s_module.getSourceId());
        EQ(size_t(0), info.s_module.getHits().size());
    }
    {
        const CAENParser::ModuleInfo& info = m_pParser->m_modules[2];
        EQ(CAENHit::PHA, info.s_hitType);
        EQ(0, info.s_multiplier);
        EQ(2, info.s_module.getSourceId());
        EQ(size_t(0), info.s_module.getHits().size());
    }
}