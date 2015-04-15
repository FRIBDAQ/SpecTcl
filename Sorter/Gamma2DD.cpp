/*
    This software is Copyright by the Board of Trustees of Michigan
    State University (c) Copyright 2005.

    You may use this software under the terms of the GNU public license
    (GPL).  The terms of this license are described at:

     http://www.gnu.org/licenses/gpl.txt

     Author:
             Ron Fox
	     NSCL
	     Michigan State University
	     East Lansing, MI 48824-1321
*/

// implementation file for CGamma2DD - the gamma 2d deluxe
// spectrum.  Note that since this is a template class,
// it will most likely be included in the header too:

#ifndef __GAMMA2DD_CXX
#define __GAMMA2DD_CXX

#include <config.h>

#ifndef __GAMMA2DD_H
#include "Gamma2DD.h"		// In case not in header.
#endif


#ifndef __PARAMETER_H
#include <CParameter.h>
#endif

#ifndef __RANGEERROR_H
#include <RangeError.h>
#endif

#ifdef HAVE_STD_NAMESPACE
using namespace std;
#endif




/*!
   Construct the spectrum, for 1:1 channel mapping:

   \param rName - The name of the spectrum.
   \param nId   - The unique id of the spectrum.
   \param xParameters - The STL vector of parameters on the x axis.
   \param yParameters - The STL vector of parameters on the y axis.
   \param xChannels   - Number of x channel bins.
   \param yChannels   - Number of y channel bins.
*/
template<class T>
CGamma2DD<T>::CGamma2DD(const std::string& rName, UInt_t nId,
			   std::vector<CParameter>&  xParameters,
			   std::vector<CParameter>&  yParameters,
			   UInt_t xChannels, UInt_t yChannels) :
  CGammaSpectrum(rName, nId,
		 CreateAxisVector(xParameters, yParameters,
				  xChannels, yChannels,
				  0.0,   (Float_t)(xChannels),
				  0.0,   (Float_t)(yChannels)),
		 xParameters, yParameters),
  m_nXscale(xChannels),
  m_nYscale(yChannels),
  m_xParameters(xParameters),
  m_yParameters(yParameters)
{
  AddAxis(xChannels, 0.0, (Float_t)(xChannels - 1), xParameters[0].getUnits());
  AddAxis(yChannels, 0.0, (Float_t)(yChannels - 1), yParameters[0].getUnits());
  CreateStorage();
}


/*!
   Construct the spectrum for an arbitrary axis binning/mapping:

   \param rName       - The name of the spectrum.
   \param nId         - The unque spectrum id.
   \param xParameters - The STL vector of X axis parameters.
   \param yParameters - The STL vector of Y axis parameters.
   \param nXChannels  - The number of bins on the x axis.
   \param nYChannels  - The number of bins on the y axis.
   \param xLow        - The low limit of the x axis.
   \param xHigh       - The high limit of the x axis.
   \param yLow        - The low limit of the y axis.
   \param yHigh       - The high limit of the y axis.

*/
template <class T>
CGamma2DD<T>::CGamma2DD(const std::string& rName, UInt_t nId,
			   std::vector<CParameter>& xParameters,
			   std::vector<CParameter>& yParameters,
			   UInt_t nXChannels, UInt_t nYChannels,
			   Float_t xLow, Float_t xHigh,
			   Float_t yLow, Float_t yHigh) :
  CGammaSpectrum(rName, nId,
		 CreateAxisVector(xParameters, yParameters,
				  nXChannels, nYChannels,
				  xLow, xHigh,
				  yLow, yHigh),
		 xParameters, yParameters),
  m_nXscale(nXChannels),
  m_nYscale(nYChannels),
  m_xParameters(xParameters),
  m_yParameters(yParameters)
{
  AddAxis(nXChannels, xLow, xHigh, xParameters[0].getUnits());
  AddAxis(nYChannels, yLow, yHigh, yParameters[0].getUnits());

  CreateStorage();
}


/*!
   Equality is just a shallow comparison of the member data.
   and the base class (actually not much point to this):
*/

template<class T>
int
CGamma2DD<T>::operator==(const CGamma2DD<T>& rhs)
{
  return (CGammaSpectrum::operator==(rhs)                 &&
	  m_nXscale      == rhs.m_nXscale                 &&
	  m_nYscale      == rhs.m_nYscale                 &&
	  m_xParameters  == rhs.m_xParameters             &&
	  m_yParameters  == rhs.m_yParameters);
}

/*!
   Get the number of channels in the xaxis:

*/
template<class T>
UInt_t
CGamma2DD<T>::getXScale() const 
{
  return m_nXscale;
}

/*
   Get the number of channels on the y axis:
*/
template<class T>
UInt_t
CGamma2DD<T>::getYScale() const
{
  return m_nYscale;
}

/*!
   Return the set of X axis parameters
*/
template<class T>
vector<CParameter>
CGamma2DD<T>::getXParameters() const 
{
  return m_xParameters;
}
/*!
   Return the set of y axis parameters.
*/
template<class T>
vector<CParameter>
CGamma2DD<T>::getYParameters() const
{
  return m_yParameters;
}
/*!
  The spectrum type is just keG2DD

*/

template<class T>
SpectrumType_t
CGamma2DD<T>::getSpectrumType()
{
  return keG2DD;
}


/*!
   Indexing operator returns the value of a channel in the spectrum.
   \param pIndices  - points to an array of indices (must be 2).
   \return ULong_t 
   \retval The value at the selected channel.
*/
template<class T>
ULong_t
CGamma2DD<T>:: operator[](const UInt_t* pIndices) const
{
  T* pStorage = static_cast<T*>(getStorage());
  return static_cast<ULong_t>(pStorage[pIndices[1]*m_nXscale + pIndices[0]]);
}

/*!
   set  - sets the value of a channel to a specific value:
   \param pIndices - points to an array of idices.
*/
template<class T>
void
CGamma2DD<T>::set(const UInt_t* pIndices, ULong_t value)
{
  T* pStorage = static_cast<T*>(getStorage());
  pStorage[pIndices[1]*m_nXscale + pIndices[0]] = static_cast<T>(value);
}


/*!
   Return a vector of the number of channels on each axis.
*/
template<class T>
void
CGamma2DD<T>::GetResolutions(std::vector<UInt_t>& rvResolutions)
{
  rvResolutions.push_back(m_nXscale);
  rvResolutions.push_back(m_nYscale);
}

/*!
   Get a specific dimension.  0, for x, 1 for y others are
   CRangeError exceptions... dimension means number of channels on that
   axis.
*/
template<class T>
UInt_t
CGamma2DD<T>::Dimension(UInt_t n) const
{
  switch (n) {
  case 0:
    return m_nXscale;
  case 1:
    return m_nYscale;
  default:
    throw CRangeError(0, 1, n, "Returning a Gamma 2d deluxe dimension");
  }
}

/*!
  Return the number of dimensions in the spectrum:

*/
template<class T>
UInt_t
CGamma2DD<T>::Dimensionality() const
{
  return 2;

}

/*!
   Improper increment results in an exception:
*/
template<class T>
void
CGamma2DD<T>::Increment(std::vector<std::pair<UInt_t, Float_t> >& rParameters)
{
  throw CException("Attempted a 'non-deluxe' increment of a gamma deluxe spectrum");
}

/*!
   Increment the spectrum.
   \param rXParameters - the xparameter number/value pairs.
   \param rYParameters - the yparameter number/value pairs.

*/
template<class T>
void
CGamma2DD<T>::Increment(std::vector<std::pair<UInt_t, Float_t> >& rXParameters,
			std::vector<std::pair<UInt_t, Float_t> >& rYParameters)
{
  if ((rXParameters.size() > 0) && (rYParameters.size() > 0)) {
    for (int i =0; i < rXParameters.size(); i++) {
      for (int j = 0; j < rYParameters.size(); j++) {
	Float_t xval = rXParameters[i].second;
	Float_t yval = rYParameters[j].second;

	// Transform x/yval -> Spectrum channel coordinates:

	UInt_t x  = (UInt_t)ParameterToAxis(0, xval);
	UInt_t y  = (UInt_t)ParameterToAxis(1, yval);

	// If the point is in the spectrum bounds, increment:

	if ((x < m_nXscale) && (y < m_nYscale)) {
	  T* pStorage = static_cast<T*>(getStorage());

	  pStorage[x + y*m_nXscale]++;
	}
      }
    }
  }
}
/*!
   Return a struct that describes the spectrum so that a definition
   can be automatically created:
*/
static CSpectrum::SpectrumDefinition definition;


//
static void
defClear()
{
  definition.sName.clear();
  definition.vParameters.clear();
  definition.vyParameters.clear();
  definition.nChannels.clear();
  definition.fLows.clear();
  definition.fHighs.clear();
}

template<class T>
CSpectrum::SpectrumDefinition&
CGamma2DD<T>::GetDefinition()
{
  defClear();
  definition.sName = getName();
  definition.nId   = getNumber();
  definition.eType = keG2DD;
  definition.eDataType = StorageType();
  for (int i = 0; i < m_xParameters.size(); i++) {
    definition.vParameters.push_back(m_xParameters[i].getNumber());
  }
  for (int i = 0; i < m_yParameters.size(); i++) {
    definition.vyParameters.push_back(m_yParameters[i].getNumber());
  }
  definition.nChannels.push_back(m_nXscale);
  definition.nChannels.push_back(m_nYscale);

  definition.fLows.push_back(GetLow(0));
  definition.fLows.push_back(GetLow(1));

  definition.fHighs.push_back(GetHigh(0));
  definition.fHighs.push_back(GetHigh(1));

  return definition;
}


////// Utility functions:


// Create the storage associated with the spectrum.
// The storage type will be determined by sizeof(T) checked against:
// sizeof(UChar_t)  -> keByte
// sizeof(UShort_t) -> keWord
// sizeof(UInt_t)  -> keLong
//
// Any other match is considered keUnknown_dt...which hopefully will cause
// all sorts of conniptions when used:

template<class T>
void
CGamma2DD<T>::CreateStorage()
{

  if (sizeof(T) == sizeof(UChar_t)) {
      setStorageType(keByte);
  }
  else if (sizeof(T) == sizeof(UShort_t)) {
      setStorageType(keWord);
  }
  else if (sizeof(T) ==  sizeof(UInt_t)) {
      setStorageType(keLong);
  }
  else {
    setStorageType(keUnknown_dt);
  }
  Size_t nBytes = StorageNeeded();
  T* pStorage = new T[nBytes/sizeof(T)];

  ReplaceStorage(pStorage);
  Clear();
}

// Create the axis vector for the spectrum given the parameters.
// we're going to assume that all x/y axis parameters have the same range
// characteristics.
//
template<class T>
CSpectrum::Axes
CGamma2DD<T>::CreateAxisVector(std::vector<CParameter> xParams,
			       std::vector<CParameter> yParams,
			       UInt_t nXchan, UInt_t nYchan,
			       Float_t xLow,  Float_t xHigh,
			       Float_t yLow,  Float_t yHigh)
{
  CSpectrum::Axes Result;
  Result.push_back(CAxis(xLow, xHigh, nXchan,
			 CParameterMapping(xParams[0])));
  Result.push_back(CAxis(yLow, yHigh, nYchan,
			 CParameterMapping(yParams[0])));
  return Result;
}

#endif
