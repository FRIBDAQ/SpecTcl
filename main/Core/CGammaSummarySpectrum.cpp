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

// Note that since this is an implementation of template classes, 
// inlcudes here are treated as if this were an include file (well it is).
//


#ifndef __CGAMMASUMMARYSPECTRUM_CXX
#define __CGAMMASUMMARYSPECTRUM_CXX
#include <config.h>

#ifndef __CGAMMASUMMARYSPECTRUM_H
#include "CGammaSummarySpectrum.h"
#endif

#ifndef __PARAMETER_H
#include "Parameter.h"
#endif

#ifndef __RANGEERROR_H
#include <RangeError.h>
#endif

#ifndef __EVENT_H
#include <Event.h>
#endif

#ifndef __STL_SET
#include <set>
#ifndef __STL_SET
#define __STL_SET
#endif
#endif

#ifndef __CRT_LIMITS_H
#include <limits.h>
#ifndef __CRT_LIMITS_H
#define __CRT_LIMITS_H
#endif
#endif

#include <TH2I.h>
#include <TH2S.h>
#include <TH2C.h>

using namespace std;

/*!
   Construct a gamma summary spectrum that's not mapped.. that is one where the
   value of the parameters in the spectrum goes from 0 to nYchannels -1.

   \param name  - Name of the new spectrum.
   \param id    - Id number of the new spectrum.
   \param nYchannels - Number of y channels
   \param nxChannels - Number of gamma spectrum (x channels).
   \param pParameters - A pointer to an array of vectors.  Each vector is the
                        list of parameter for an x channel of the spectrum.

*/
template <typename T, typename R>
CGammaSummarySpectrum<T,R>::CGammaSummarySpectrum(string              name, 
						UInt_t              nId,
						UInt_t              nYChannels,
						vector<vector <CParameter> >*  pParameters) :
  CSpectrum(name, nId),
  m_nXChannels(pParameters->size() + 2),
  m_nYChannels(nYChannels + 2)
{

  UInt_t nXChannels = pParameters->size();
 
  fillParameterArray(*pParameters,
		     nXChannels);
  CreateAxes(*pParameters,
	     nXChannels,
	     nYChannels,
	     0, nYChannels - 1);
  
  R* pRootSpectrum = new R(
    name.c_str(), name.c_str(),
    nXChannels, static_cast<Double_t>(0), static_cast<Double_t>(nXChannels),
    nYChannels, static_cast<Double_t>(0.0), static_cast<Double_t>(nYChannels)
  );
  pRootSpectrum->Adopt(0, nullptr);
  setRootSpectrum(pRootSpectrum);
  CreateStorage();
}
/*!
   Create a gamma summary spectrum that is mapped, that is some parameter range,
   ylow - yhigh is mapped to the set of y axis channels.

   \param name   - Name of the ne spectrum.
   \param id     - Id number of the new spectrum.
   \param nXChannels - Number of x channels.
   \param pParameters - A pointer to an array of vectors.  Each vector is the
                        list of parameter for an x channel of the spectrum.
   
   \param nYChannels - Number of Y channels.
   \param fYLow     - Y channel zero means this value of the parameter.
   \param fYHigh    - Y channel nYChannels means this value of the parameter.

*/
template <typename T, typename R>
CGammaSummarySpectrum<T,R>::CGammaSummarySpectrum(const std::string name, 
						UInt_t            nId,
						std::vector<std::vector<CParameter> >* pParameters,
						UInt_t            nYChannels,
						Float_t           fYLow,
						Float_t           fYHigh) :
  CSpectrum(name, nId),
  m_nXChannels(pParameters->size() + 2),
  m_nYChannels(nYChannels + 2)

{
  
  UInt_t nXParameters = pParameters->size();
  fillParameterArray(*pParameters, nXParameters);
  CreateAxes(*pParameters,
	     nXParameters,
	     nYChannels,
	     fYLow, fYHigh);
  
  R* pRootSpectrum = new R(
    name.c_str(), name.c_str(),
    nXParameters, static_cast<Double_t>(0.0), static_cast<Double_t>(nXParameters),
    nYChannels, static_cast<Double_t>(fYLow), static_cast<Double_t>(fYHigh)
  );
  pRootSpectrum->Adopt(0, nullptr);
  setRootSpectrum(pRootSpectrum);
  CreateStorage();
}

/*!
  Destructor must play some fancy footwork to continue to manage root's storage:
*/
template <typename T, typename R>
CGammaSummarySpectrum<T,R>::~CGammaSummarySpectrum()
{
  R* pRootSpectrum = reinterpret_cast<R*>(getRootSpectrum());
  pRootSpectrum->fArray = nullptr;
  
}  

/*!
   Increment the spectrum.  Iterating through each x channel and its associated
   axis mapping, treat that x channel strip as a 1-d spectrum and increment
   it.

   \param rEvent  - The event for which we must increment the spectrum.

*/
template <typename T, typename R>
void
CGammaSummarySpectrum<T,R>::Increment(const CEvent& e)
{
  CEvent& event((CEvent&)e);


  for (UInt_t x = 0; x < m_nXChannels - 2; x++) {
    vector<UInt_t>&      params(m_Parameters[x]);
    // 
    // Iterate over all parameters in a y stripe:
    //
    for(int i =0; i < params.size(); i++) {
      UInt_t paramId = params[i];
      if (paramId < event.size() && event[paramId].isValid()) {
        getRootSpectrum()->Fill(
          static_cast<Double_t>(x), static_cast<Double_t>(event[paramId])
        );
      }
    }
  }
}
/*!
    Retrieve a channel from the spectrum..given an array of indices.
    \param pIndices - array of indices.  pIndices[0] x, pIndices[1] y.
    \return ULong_t
    \retval the channel value defined at the indices,.
 
*/
template <typename T, typename R>
ULong_t
CGammaSummarySpectrum<T,R>::operator[](const UInt_t* pIndices) const
{
  Int_t x = pIndices[0];
  Int_t y = pIndices[1];

  const TH1* pRootSpectrum = getRootSpectrum();
  Int_t bin = pRootSpectrum->GetBin(x + 1, y+1);
  return static_cast<ULong_t>(pRootSpectrum->GetBinContent(bin));
}
/*!
  Sets a channel value in a spectrum
  \param pIndices  - Pointer to the index array as for operator[].
  \param nValue    - Value to use.
*/
template <typename T, typename R>
void
CGammaSummarySpectrum<T,R>::set(const UInt_t* pIndices, ULong_t nValue)
{
  Int_t x = pIndices[0];
  Int_t y = pIndices[1];
  TH1* pRootSpectrum = getRootSpectrum();
  Int_t  bin = pRootSpectrum->GetBin(x + 1, y + 1);
  pRootSpectrum->SetBinContent(bin, static_cast<Double_t>(nValue));
}

/*!
     \parm id  - The id of a parameters.
     \return bool
     \retval true - the parameter is in the set of parameters used by this spectrum.
     \retval false - The parameter isn't used by the spectrum.

*/
template <typename T, typename R>
Bool_t
CGammaSummarySpectrum<T,R>::UsesParameter(UInt_t nId) const
{
  for (int i =0; i < m_Parameters.size(); i++) {
    const std::vector<UInt_t>& p(m_Parameters[i]);
    for(int j = 0; j < p.size(); j++) {
      if (p[j] == nId) return kfTRUE;
    }
  }
  return kfFALSE;
}
/*!
   Returns a list of the parameter ids the spectrum uses.
   This list is 'uniquified' that is duplicates (which may exist in this sort of spectrum)
   are removed.  The parameters are not returned in any specific order.
   \parameter std::vector<UInt_t>& ids - Parameters are returned in this vector.
*/
template <typename T, typename R>
void
CGammaSummarySpectrum<T,R>::GetParameterIds(vector<UInt_t>& ids)
{
  std::set<UInt_t> resultSet;	// Used to uniquify.
  for (int i =0; i < m_Parameters.size(); i++) {
    std::vector<UInt_t>& p(m_Parameters[i]);
    for (int j =0; j < p.size(); j++) {
      resultSet.insert(p[j]);
    }
  }
  // Iterate through the set, pulling keys out and stuffing them
  // sequentially in ids:

  std::set<UInt_t>::iterator p = resultSet.begin();
  while (p != resultSet.end()) {
    ids.push_back(*p);
    p++;
  }
}
/*!
  Get the number of channels on the Y axis.
*/
template <typename T, typename R>
void
CGammaSummarySpectrum<T,R>::GetResolutions(vector<UInt_t>& rvResolutions)
{
  rvResolutions.push_back(m_nYChannels);
}
/*!
   Return a description of the spectrum.  Note that there will be some kludging
   required given how our spectrum works:
   - vParameters - is a list of the parameters for each stripe of the spectrum.
                   each stripe is separated by a parameter id UINT_MAX
   - vyParameters - is empty.
   - nChannels - [0] - The number of strips.
                 [1] - the number of y channels.
   - fLows/fHighs    - The low/hi limits for each strip.

   \return CSpectrum::SpectrumDefinition&
   \retval Definition of the spectrum.. see above, khowever.

   \note Not threadsafe as the definition is built uip in a static structure.

*/
template <typename T, typename R>
CSpectrum::SpectrumDefinition& 
CGammaSummarySpectrum<T,R>::GetDefinition()
{
  static SpectrumDefinition def;
  def.fLows.clear();
  def.fHighs.clear();
  def.nChannels.clear();
  def.vParameters.clear();


  def.sName = getName();
  def.nId   = getNumber();
  def.eType = keGSummary;
  def.eDataType = StorageType();
  
  // Fill in the parameters:

  for (int i =0; i < m_Parameters.size(); i++) {
    vector<UInt_t>& p(m_Parameters[i]);
    for (int j = 0; j < p.size(); j++) {
      def.vParameters.push_back(p[j]);
    }
    def.vParameters.push_back(UINT_MAX); // end of row sentinel.

  }

  def.fLows.push_back(0);
  def.fHighs.push_back(m_nXChannels-1);
  def.nChannels.push_back(m_nXChannels);

  def.nChannels.push_back(m_nYChannels);
  def.fLows.push_back(m_Axes[0].getLow());
  def.fHighs.push_back(m_Axes[0].getHigh());


  return def;
}
/*!
   Determines if the spectrum must have a specific parameter to be incremented.
  This determines where the spectrum will wind up in the speed organization
  for histogramming purposes. 
  \retval false
*/
template <typename T, typename R>
Bool_t
CGammaSummarySpectrum<T,R>::needParameter() const
{
  return kfFALSE;
}

/*!
    Returns the number of dimensions (2) in the spectrum.
*/
template <typename T, typename R>
UInt_t
CGammaSummarySpectrum<T,R>::Dimensionality() const
{
  return 2;
}

/*!
  Returns the number of channels in any of the two dimensions.
*/
template <typename T, typename R>
Size_t
CGammaSummarySpectrum<T,R>::Dimension(UInt_t n) const
{
  if (n == 0) {
    return m_nXChannels;
  }
  if (n == 1) {
    return m_nYChannels;
  }
  throw CRangeError(0,1, n, "Gamma summary spectrum dimension call");
}
/*!
  \return SpectrumType_t
  \retval keGSummary
*/

template <typename T, typename R>
SpectrumType_t
CGammaSummarySpectrum<T,R>::getSpectrumType()
{
  return keGSummary;
}

/*!
  Return a low limit on an axis.
*/
template <typename T, typename R>
Float_t
CGammaSummarySpectrum<T,R>::GetLow(UInt_t n) const
{
  if (n == 0) {
    return 0.0;
  }
  else if (n == 1) {
    return m_Axes[0].getLow();
  }
  else {
    throw CRangeError(0,1,n, "Getting low limit of a gamma summary spectrum axis");
  }
}

/*!
  Return high limit on an axis:
*/
template<typename T, typename R>
Float_t
CGammaSummarySpectrum<T,R>::GetHigh(UInt_t n) const
{
  if (n == 0) {
    return static_cast<Float_t>(m_nXChannels - 1);
  }
  else if (n == 1) {
    return m_Axes[0].getHigh();
  }
  else {
    throw CRangeError(0,1,n, "Getting high limit of a gamma summary spectrum axis");
  }
}

/*!
  Return the units for an axis:
*/
template<typename T, typename R>
string
CGammaSummarySpectrum<T,R>::GetUnits(UInt_t n) const
{
  if (n == 0 ) {
    return string("channels");
  }
  else if (n == 1) {
    return m_Axes[0].getParameterMapping().getUnits();
  } 
  else {
    throw CRangeError(0,1,n, "Getting units of a gamma summary spectrum axis");
  }
}

///////////////////////////////////////////////////////////////////////////////
//  Utility methods.
//

/*
 * create the storage needed for the spectrum.   This also sets the storage type
 * Implicit inputs:
 *     m_nXChannels  - Number of Xchannels.
 *     m_nYChanhnels - NUmber of Y CHannels.
 *     T             - Type of channel.
 */
template <typename T, typename R>
void
CGammaSummarySpectrum<T,R>::CreateStorage()
{
  switch (sizeof(T)) {
  case sizeof(UChar_t):
    setStorageType(keByte);
    break;
  case (sizeof(UShort_t)): 
    setStorageType(keWord);
    break;
  case (sizeof(UInt_t)):
    setStorageType(keLong);
    break;
  default:
   setStorageType(keUnknown_dt);
   
  }

  T* pStorage = new T[static_cast<unsigned>(StorageNeeded()/sizeof(T))];
  ReplaceStorage(pStorage);
  Clear();
  
  createStatArrays(1);
}

/*
 *  Fill the parameter array, from the array of pararmeter vectors:
 */
template <typename T, typename R>
void
CGammaSummarySpectrum<T,R>::fillParameterArray(vector<vector<CParameter> >& params,
					  UInt_t xChannels)
{
  for (int i=0; i < xChannels; i++) {
    vector<CParameter>& p(params[i]);
    vector<UInt_t> v;
    for (int j = 0; j < p.size(); j++) {
      v.push_back(p[j].getNumber());
    }
    m_Parameters.push_back(v);
  }

}
/*
** Create the axis vector that determines how paramters map to channels in each
** of the strips:
*/
template <typename T, typename R>
void
CGammaSummarySpectrum<T,R>::CreateAxes(vector<vector<CParameter> >& params,
				    UInt_t              nx,
				    UInt_t              ny,
				    Float_t             low,
				    Float_t             hi)
{
  for (int i=0; i < nx; i++) {
    vector<CParameter>& p(params[i]);
    m_Axes.push_back(CAxis(low, hi, ny, p[0].getUnits()));
  }
}
/**
 * setStorage
 *    Set new storage location.
 *
 *  @param pStorage - pointer to new spectrum storage.
 */
template <typename T, typename R>
void
CGammaSummarySpectrum<T,R>::setStorage(Address_t pStorage)
{
  R* pRootSpectrum = reinterpret_cast<R*>(getRootSpectrum());
  pRootSpectrum->fArray = reinterpret_cast<T*>(pStorage);
  pRootSpectrum->fN     = m_nXChannels * m_nYChannels;
}
/**
 * StorageNeeded
 *
 * @return Size_t - number of bytes of storage required for spectrum.
 */
template <typename T, typename R>
Size_t
CGammaSummarySpectrum<T, R>::StorageNeeded() const
{
  return static_cast<Size_t>(m_nXChannels * m_nYChannels * sizeof(T));
}
#endif
