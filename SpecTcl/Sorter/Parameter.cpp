// Implements nontrivial functions of CParameter.


#include "Parameter.h"

// Class operations:

/*!
  Convert a raw parameter value to a mapped parameter value.
  This is an identity transform if the paramter has no scale info
  as is the case if it is a 'real' parameter.
  
  \param Raw (Float_t [in]): Input raw value.
  \return Float_t - The converted value.
*/
Float_t
CParameter::RawToMapped(Float_t fRaw)
{
  if(m_fScaled && (m_nHigh != m_nLow)) {
    Float_t frac = fRaw/(Float_t)(1 << m_nScale);
    return m_nLow + frac * (m_nHigh - m_nLow);
      
  }
  else {			//  No scaling information.
    return fRaw;
  }
}
/*!
  Convert a mapped parameter value to its corresponding raw value.
  This transformation is an identity if the parameter has no
  scale information (as is the case if it is a `real' parameter).

  \param fMapped (Float_t [in]): The mapped parameter to convert.
  \return Float_t the converted value.
*/
Float_t
CParameter::MappedToRaw(Float_t fMapped)
{
  if(m_fScaled & (m_nLow != m_nHigh)) {
    Float_t frac = (fMapped - m_nLow)/(m_nHigh - m_nLow);
    return frac * (Float_t)(1 << m_nScale);
  }
  else {			// No scaling information.
    return fMapped;
  }
}
