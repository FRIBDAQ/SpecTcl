/*
    This software is Copyright by the Board of Trustees of Michigan
    State University (c) Copyright 2014.

    You may use this software under the terms of the GNU public license
    (GPL).  The terms of this license are described at:

     http://www.gnu.org/licenses/gpl.txt

     Authors:
             Ron Fox
             Jeromy Tompkins
             NSCL
             Michigan State University
             East Lansing, MI 48824-1321
*/
#include <config.h>
#include "CParameterMappingTest.h"
#include <string>
#include <stdlib.h>
#include <iostream>
#include "Asserts.h"

#ifdef HAVE_STD_NAMESPACE
using namespace std;
#endif

CParameterMappingTest::CParameterMappingTest() :
  m_UnMapped(),			// Not mapped.
  m_UnitInterval(12, 0.0, 1.0),	// Mapped to unit interval.
  m_Displaced(12, 1.0, 4097.0, string("Units"))	// Map 0-4096 -> 1-4097.
{}

//
// Ensure that the constructors set member data appropriately.
//
void 
CParameterMappingTest::testConstructors() 
{

  CPPUNIT_ASSERT(!m_UnMapped.isMapped()); // Unmapped is unmapped.
  CPPUNIT_ASSERT(m_UnitInterval.isMapped()); // And the mapped ones are mapped
  CPPUNIT_ASSERT(m_Displaced.isMapped()); 

  // Check values of member data:

  CPPUNIT_ASSERT(m_UnitInterval.getBits() == 12);
  CPPUNIT_ASSERT(m_UnitInterval.getLow() == 0.0);
  CPPUNIT_ASSERT(m_UnitInterval.getHigh() == 1.0);
  CPPUNIT_ASSERT(m_UnitInterval.getUnits() == string(""));
  
  CPPUNIT_ASSERT(m_Displaced.getUnits() == string("Units"));
}
// Ensure that the identity transform transforms x->x
//
// For 100 random numbers x->x.
//
void
CParameterMappingTest::testIdentityTransform()
{

  for(int i = 0; i < 100; i++) {
    float f = (float)rand();
    CPPUNIT_ASSERT(f == m_UnMapped.RawToMapped(f));
    CPPUNIT_ASSERT(f == m_UnMapped.MappedToRaw(f));
  }
}

// Test the transform of [0,4095] -> [0.0, 1.0]
//
//  This is done for 100 random values.
//
void
CParameterMappingTest::testUnitInterval()
{
  for(int i =0; i < 100; i++) {
    float f = (float)rand();
    f = (f/(float)RAND_MAX)*4095.0;
    // Check forward transform is correct:
    float mapped = m_UnitInterval.RawToMapped(f);
    EQMSG("Forward transform is correct",
          (float)(f/4096.0), mapped);

    // Check transform is reversible:

    EQMSG("Check transform is reversible",
          f, m_UnitInterval.MappedToRaw(mapped));
  }

}

void CParameterMappingTest::testSlideTransform()
{
  // The m_Displaced parameter transform should just slide the raw value over
  // by +1 and the mapped value back by -1.
  // We'll test this with 100 random values as well
  
  for(int i = 0; i < 100; i++ ) {
    float f = (float)((float)rand()*4095.0/(float)RAND_MAX);

    EQMSG("RawToMapped should adjust +1",
          (float)(f+1.0), m_Displaced.RawToMapped(f));
    EQMSG("MappedToRaw should adjust -1",
          (float)(f-1.0), m_Displaced.MappedToRaw(f));
  }
}







