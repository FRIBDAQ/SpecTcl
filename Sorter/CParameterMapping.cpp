/*
           CLASS_PACKAGE
           Provides a mapping between raw parameter coordinates and potentially
           mapped coordinates.  There are two types of parameters:
           - Reals - these have no mappings. the coordinates of the parameter represent
             actual physical values.  The transformation functions are identity transforms.
           - Mapped - these parameters have raw values between 0 and 2^m_nBits - 1 
              which, upon transformation, represent real values between m_fLow and m_fHigh.
*/

////////////////////////// FILE_NAME.cpp /////////////////////////////////////////////////////
#include "CParameterMapping.h"    				
#include "Parameter.h"

/*!
  The default constructor creates a parameter mapping for a unit
transform.  That is the mapped parameter values are the same as the
raw parameter values.  This is indicated by a value of kfFALSE
for m_fMapped.
\param sUnits (const string& [in] default = ""): 
   The units of the parameter.
   If empty there are no units.
*/

CParameterMapping::CParameterMapping (const string& sUnits)
   : m_fMapped(kfFALSE),
   m_nBits(0),
   m_fLow(0),
   m_fHigh(0),
   m_sUnits(sUnits)
 
{   
    

    
}
/*!
   The parameterized constructor creates a mapping between an
   integer parameter with nBits of significant data and some
   arbitrary real [fLow, fHigh].
   \param nBits  (UInt_t in): Number of significant bits in the
           parameter.
   \param fLow   (Float_t in): Low value of the mapped range.
   \param fHigh  (Float_t in): High value of the mapped range.
   \param sUnits (const string& in default = "")
       The units of the parameter.  If empty, there are not units.
*/
CParameterMapping::CParameterMapping(UInt_t  nBits,
				     Float_t fLow,
				     Float_t fHigh,
				     const string& sUnits ) :
  m_fMapped(kfTRUE),
  m_nBits(nBits),
  m_fLow(fLow),
  m_fHigh(fHigh),
  m_sUnits(sUnits)
{
  if(fLow == fHigh) 
    m_fMapped = kfFALSE;  
}

/*!
  Construct a parameter mapping from a parameter definition.
  This is likely the most often form of construction.
  \param rParam (CParameter& [in]) The parameter to construct a
      mapping object for.
*/
CParameterMapping::CParameterMapping(const CParameter& rParam) :
  m_nBits(rParam.getScale()),
  m_fLow(rParam.getLow()),
  m_fHigh(rParam.getHigh()),
  m_sUnits(rParam.getUnits())
{
  m_fMapped = (m_fLow != m_fHigh) && (rParam.hasScale());
}
/*!
   Destructor requires no processing and is therefore empty.
*/
 CParameterMapping::~CParameterMapping ( )
{
}


/*!
  The copy constructor is used to construct temporaries in e.g. 
  pass by value semantic cases.
*/

CParameterMapping::CParameterMapping (const CParameterMapping& rhs):
  m_fMapped(rhs.m_fMapped),
  m_nBits(rhs.m_nBits),
  m_fLow(rhs.m_fLow),
  m_fHigh(rhs.m_fHigh),
  m_sUnits(rhs.m_sUnits)
{
  
} 
/*!
  Assignment operation.  Returns a reference to this in order to
  allow assignment/oeprator chaining.
*/
CParameterMapping& 
CParameterMapping::operator= (const CParameterMapping& aCParameterMapping)
{ 
  if (this != &aCParameterMapping) {
    
    
    m_fMapped = aCParameterMapping.m_fMapped;
    m_nBits = aCParameterMapping.m_nBits;
    m_fLow = aCParameterMapping.m_fLow;
    m_fHigh = aCParameterMapping.m_fHigh;
    m_sUnits = aCParameterMapping.m_sUnits;
  }
  return *this;
}

      //Operator== Equality Operator 
int 
CParameterMapping::operator== (const CParameterMapping& aCParameterMapping) const
{ 
  return (
	  (m_fMapped == aCParameterMapping.m_fMapped) &&
	  (m_nBits == aCParameterMapping.m_nBits) &&
	  (m_fLow == aCParameterMapping.m_fLow) &&
	  (m_fHigh == aCParameterMapping.m_fHigh) &&
	  (m_sUnits == aCParameterMapping.m_sUnits) 
	  );
}

// Functions for class CParameterMapping

/*!


Returns the mapped value corresponding to a
raw parameter value.  This is an identity transform if
m_fMapped is false.

*/
Float_t 
CParameterMapping::RawToMapped(Float_t fRaw)
{ 
  if(!m_fMapped) return fRaw;	// Unmapped parameters don't change
  Float_t RawMax = (Float_t)((1 << m_nBits) - 1);
  return m_fLow + (m_fHigh - m_fLow)*fRaw/RawMax; 
}  

/*!  

Returns the raw parameter value corresponding to a
mapped value.  The transformation is an identity if m_fMapped
is false.

*/
Float_t 
CParameterMapping::MappedToRaw(Float_t fMapped)  
{
  if(!m_fMapped) return fMapped; // Unmapped don't change.
  fMapped -= m_fLow;		// Translate to 0 - fullscale.
  Float_t RawMax = (Float_t)((1 << m_nBits) - 1);
  return fMapped*RawMax/(m_fHigh - m_fLow);
}
