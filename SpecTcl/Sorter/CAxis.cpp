/* \class CAxis   
           CLASS_PACKAGE
           Describes the range and granularity of an axis.  If the parameter that 
           is displayed on this axis has limits or mapping, this information is also held.
           The purpose of CAxis is to provide transformations between spectrum and parameter
           coordinates.  In the case of mapped parameters, the transformation is between the
           spectrum coordinates and raw parameter value.
           There are three potential coordinate systems:
           - The axis coordinates: these are integers from 0 to m_nChannels - 1.
           - The Raw parameter coordinates: These are arbitrary reals, unless
              m_fMappedParam is true in which case they are reals between
              0 and 2^m_nParameterBits - 1.
           - The mapped parameter coordinates: Same as raw parameter
              coordinates unless m_fMappedParam is true.. in which case
              these are float values between m_fMappingLow and m_fMappingHigh.
*/

////////////////////////// FILE_NAME.cpp /////////////////////////////////////////////////////
#include "CAxis.h"    				

/*!
   Construct an axis from just its length.
   - The low limit will be 0.0
   - The high limit will be nChannels - 1.
   - The ParameterSpecification will be unmapped.
    
     \param nChannels (UInt_t [in]): Number of channels on the
        axis.
     \param sUnits (string [in] default = string("")):
         The units of the parameter.
*/
CAxis::CAxis(UInt_t nChannels, string sUnits) :
  m_fLow(0.0),
  m_fHigh((float)(nChannels -1)),
  m_nChannels(nChannels),
  m_ParameterSpecification(sUnits)	// Defaults to unmapped.
{
}
/*!
  Construct an axis for an umapped parameter that represents
  a range within that parameter.
  \param fLow (Float_t [in]) The floating point value represented
      by the axis origin.
  \param fHigh (Float_t [in]) The floating point value
     represented by the last channel on the axis.
  \param nChannels (UInt_t [in]) The number of channels in the
     axis.
   \param sUnits (string [in] default = string(""))
       The parameter units.
 */
CAxis::CAxis(Float_t fLow,
	     Float_t fHigh,
	     UInt_t  nChannels,
	     string  sUnits) :
  m_fLow(fLow),
  m_fHigh(fHigh),
  m_nChannels(nChannels),
  m_ParameterSpecification(sUnits)
{
}

/*!
    Construct an axis for a mapped parameter. The axis represents
    an arbitrary range from within the parameter.

    \param fLow  (Float_t [in]): Mapped parameter value
        represented by the axis origin.
    \param fHigh (Float_t [in]): Mapped parameter value
        represented by the end of the axis.
    \param nChannels (UInt_t [in]): Granularity (number of
      of channels) of the axis.
    \param Mapping (CParamterMapping [in]): Describes the mapping
      between raw and mapped values.
 */
CAxis::CAxis(Float_t fLow,
	     Float_t fHigh,
	     UInt_t  nChannels,
	     CParameterMapping Mapping) :
  m_fLow(fLow),
  m_fHigh(fHigh),
  m_nChannels(nChannels),
  m_ParameterSpecification(Mapping)
{
}
/*!
    Since no dynamic storage is required, the destructor
    does nothing.
 */
CAxis::~CAxis ( )  //Destructor - Delete dynamic objects
{
}


/*!
   Copy constructor: Used to create temporaries required e.g.
   in pass by value call semantics.
   \param aCAxis (const CAxis& [in]): The item to copy 
   construct to *this.
*/
CAxis::CAxis (const CAxis& aCAxis ) :
  m_fLow(aCAxis.m_fLow),
  m_fHigh(aCAxis.m_fHigh),
  m_nChannels(aCAxis.m_nChannels),
  m_ParameterSpecification(aCAxis.m_ParameterSpecification)
{
}
/*!
  Assignment:
  \param aCAxis (const CAxis& [in]) Refers to the rhs of the
     assignment.
  \return CAxis&  reference to *this; supports operator chaining.

 */
CAxis& CAxis::operator= (const CAxis& aCAxis)
{ 
  if (this != &aCAxis) {
    
    
    m_fLow = aCAxis.m_fLow;
    m_fHigh = aCAxis.m_fHigh;
    m_nChannels = aCAxis.m_nChannels;
    m_ParameterSpecification = aCAxis.m_ParameterSpecification;
  }
  
  return *this;
}
/*!
  Equality test.  If all members are the equal equality is
  maintained. 
  \param aCAxis (const CAxis& [in]): rhs of the == operator.
  \return int nonzero if equality is maintained.
 */
int CAxis::operator== (const CAxis& aCAxis) const
{ 
  return (
    (m_fLow             == aCAxis.m_fLow)            &&
    (m_fHigh            == aCAxis.m_fHigh)           &&
    (m_nChannels        == aCAxis.m_nChannels)       &&
    (m_ParameterSpecification == aCAxis.m_ParameterSpecification)
    );
}

/*!  
Converts a raw parameter value to axis coordinates. The parameter
is converted to a mapped value and then MappedParameterToAxis is
called since axes are a mapping between axis coordinates and
the mapped parameter.
*/
Float_t
CAxis::ParameterToAxis(Float_t fParameter)  
{ 
  Float_t mP = m_ParameterSpecification.RawToMapped(fParameter);
  return MappedParameterToAxis(mP);
}  

/*!  

Converts an axis coordinate to a raw parameter value.
Since axes map axis coordinates to mapped parameter coordinates
and back, we call AxisToMappedParameter and then convert that
to a raw parameter.

*/
Float_t 
CAxis::AxisToParameter(UInt_t nAxisValue)  
{ 
  Float_t mP = AxisToMappedParameter(nAxisValue);
  return m_ParameterSpecification.MappedToRaw(mP);
}  

/*!  


Converts an axis coordinate to the mapped value of a parameter.
If the parameter is not a mapped parameter, the raw parameter coordinates are
returned instead (easy since in that case there's no difference 
between mapped and unmaped).


*/
Float_t 
CAxis::AxisToMappedParameter(Int_t nAxisValue)  
{ 
  Float_t fsF;
  fsF = ((Float_t)(nAxisValue))*(m_fHigh - m_fLow)/
                                           ((Float_t)(m_nChannels-1));
  return fsF + m_fLow;
}  

/*!  Function: 	
   Int_t MappedParameterToAxis(Float_t fParameterValue) 
 Operation Type:
    
Purpose: 	

Converts a mapped parameter value to an axis coordinate.
If the parameter is not a mapped parameter, the input is treated
as a raw parameter.


*/
Float_t 
CAxis::MappedParameterToAxis(Float_t fParameterValue)  
{ 
  Float_t fsF = fParameterValue - m_fLow;
  fsF = fsF*((Float_t)(m_nChannels-1))/(m_fHigh - m_fLow);

  return (fsF);
}






