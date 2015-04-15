// Test suite for CAxis scaling object.

#include <config.h>
#include "CAxisTests.h"
#include "CParameterMapping.h"
#include <string>
#include <iostream>
#include <stdlib.h>
#ifdef HAVE_STD_NAMESPACE
using namespace std;
#endif
/*!
  Constructor.  Create:
  - m_Identity that is an identity transform from axis to an unmapped parameter
    for the length of the axis.
  - m_UnmappedUnit that takes the unit interval on an unmapped parameter.
  - m_MappedUnit   that takes the unit interval on a mapped parameter.
*/
CAxisTests::CAxisTests() :
  m_Identity(2048, string("identity")),
  m_UnmappedUnit(0.0,
		 1.0,
		 1000,
		 string("Unmappedunit")),
  m_MappedUnit(0.0,
	       1.0,
	       1000,
	       CParameterMapping(12,  -1.0, 1.0, string("MappedUnit")))
{
}
void
CAxisTests::testIdentityConstructor()
{
  cerr << "Testing CAxis identity constructor\n";

  // check out the construction of the object. itself...

  CPPUNIT_ASSERT(m_Identity.getLow() == 0.0);
  CPPUNIT_ASSERT(m_Identity.getHigh() == (Float_t)(m_Identity.getChannels()-1));

  // Check out construction of the parameter map:

  CParameterMapping map = m_Identity.getParameterMapping();
  CPPUNIT_ASSERT(!map.isMapped());

}


void CAxisTests::testUnmappedUnitConstructor()
{
  cerr << "Testing constructor for unmapped unit interval\n";

  // Check the values of the primitive attributes.

  CPPUNIT_ASSERT(m_UnmappedUnit.getLow() == 0.0);
  CPPUNIT_ASSERT(m_UnmappedUnit.getHigh() == 1.0);
  CPPUNIT_ASSERT(m_UnmappedUnit.getChannels() == 1000);

  // Check the setup of the parameter map.

  CParameterMapping mapping = m_UnmappedUnit.getParameterMapping();
  CPPUNIT_ASSERT(!mapping.isMapped());
}
void CAxisTests::testMappedUnitConstructor()
{
  cerr << "Testing constructor for [-1,1] mapped param -> [0,1]\n";

  CPPUNIT_ASSERT(m_MappedUnit.getLow() == 0.0);
  CPPUNIT_ASSERT(m_MappedUnit.getHigh() == 1.0);
  CPPUNIT_ASSERT(m_MappedUnit.getChannels() == 1000);

  // Now check the parameter mapping guy:

  CParameterMapping map = m_MappedUnit.getParameterMapping();

  CPPUNIT_ASSERT(map == CParameterMapping(12, -1.0, 1.0, string("MappedUnit")));
}
void
CAxisTests::testIdentityTransforms()
{
  cerr << "Testing transformations for the identity transform\n";
  
  // Test some easy to convert values for raw parameters.

  CPPUNIT_ASSERT(m_Identity.ParameterToAxis(0.0) == 0);
  CPPUNIT_ASSERT(m_Identity.ParameterToAxis(2047.0) == 2047);
  CPPUNIT_ASSERT(m_Identity.ParameterToAxis(1024.0) == 1024);

  // Exhaustively check axis -> parameter.

  for(Int_t i =0; i < 2048; i++) {
    CPPUNIT_ASSERT((Float_t)i == m_Identity.AxisToParameter(i));
  }

}
// Test the unit interval axis for an unmapped parameter.

void
CAxisTests::testUnmappedUnitTransforms()
{
  cerr << "Testing unmapped parameter to unit interval axis\n";
  // Check the end point and 1/2 way point.. as we know the answers there:

  CPPUNIT_ASSERT(m_UnmappedUnit.ParameterToAxis(0.0) == 0);
  CPPUNIT_ASSERT(m_UnmappedUnit.ParameterToAxis(0.5) == 499);
  CPPUNIT_ASSERT(m_UnmappedUnit.ParameterToAxis(1.0) == 999);

  // Go from axis to parameter:

  CPPUNIT_ASSERT(m_UnmappedUnit.AxisToParameter(0) == 0);
  CPPUNIT_ASSERT(m_UnmappedUnit.AxisToParameter(999) == 1.0);



  // Take a random sample of parameter values... Mapped->Axis
  // should be the same as Parameter->Axis ... same for the inverse.
  //

  for(Int_t i = 0; i < 100; i++) {
    Int_t n = rand();
    Float_t param = ((Float_t)n)/((Float_t)RAND_MAX); // range of [0,1.0).
    Int_t   axis  = (Int_t)(param * 1000.0);          // range of [0,999].

    CPPUNIT_ASSERT(m_UnmappedUnit.ParameterToAxis(param) ==
		   m_UnmappedUnit.MappedParameterToAxis(param));
    CPPUNIT_ASSERT(m_UnmappedUnit.AxisToParameter(axis) ==
		   m_UnmappedUnit.AxisToMappedParameter(axis));
  }


}
// Test transforms for a 12 bit parameter that represents [-1,1] displayed
// on a [0.0,1.0) axis.
void
CAxisTests::testMappedUnitTransforms()
{
  cerr << "Testing 12 bit parameter mapped to [-1,1] on [0,1) axis\n";

  // Endpoints of the axis: 0 is 0.0 and 999 is 1.0:

  CPPUNIT_ASSERT(m_MappedUnit.MappedParameterToAxis(0.0) == 0);
  CPPUNIT_ASSERT(m_MappedUnit.MappedParameterToAxis(1.0) == 999);
  CPPUNIT_ASSERT(m_MappedUnit.MappedParameterToAxis(-1.0) == -999);

  // The points above correspond to 2047 and 4095 respectively:

  CPPUNIT_ASSERT(m_MappedUnit.ParameterToAxis(2047.0) == 0);
  CPPUNIT_ASSERT(m_MappedUnit.ParameterToAxis(4095.0) == 999);
  CPPUNIT_ASSERT(m_MappedUnit.ParameterToAxis(0)      == -999);	// -1.0 mapped.

  // Now the other direction:

  CPPUNIT_ASSERT(m_MappedUnit.AxisToMappedParameter(0)   == 0.0);
  CPPUNIT_ASSERT((m_MappedUnit.AxisToMappedParameter(499) - 0.5)  < 0.01);
  CPPUNIT_ASSERT(m_MappedUnit.AxisToMappedParameter(999) == 1.0);
  CPPUNIT_ASSERT(m_MappedUnit.AxisToMappedParameter(-999) == -1.0);

  CPPUNIT_ASSERT(m_MappedUnit.AxisToParameter(0)    == 2047.5);
  CPPUNIT_ASSERT(m_MappedUnit.AxisToParameter(999)  == 4095.0);
  CPPUNIT_ASSERT(m_MappedUnit.AxisToParameter(-999) == 0.0);

}
