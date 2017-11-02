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

// Implementation of the base class of 2d multiply incremented spectra.
//

#include <config.h>
#include "CSpectrum2Dm.h"
#include "CParameterMapping.h"
#include <RangeError.h>

#ifdef HAVE_STD_NAMESPACE
using namespace std;
#endif

///////////////////////////////////////////////////////////////////////////
/////////////////////  Constructors //////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

/*!
  Construct the spectrum.
  \param name   : std::string
     Name of the spectrum.
  \param id     : UInt_T
     unique id of the spect5rum.
  \param parameters : std::vector<CParameter>
     Set of parameters that make up the spectrums parameters...
     the caller must ensure that there are an even number of these.
  \param xscale, yscale : UInt_t
     Number of channels on the X axis and y axis respectively.
*/
     

CSpectrum2Dm::CSpectrum2Dm(string              name,
			   UInt_t              id,
			   vector<CParameter>&  parameters,
			   UInt_t              xscale,
			   UInt_t              yscale) :
  CSpectrum(name, id,
	    CreateAxisVector(parameters[0],
			     xscale, 0.0, static_cast<Float_t>(xscale),
			     parameters[1],
			     yscale, 0.0, static_cast<Float_t>(yscale))),
  m_xChannels(xscale + 2),
  m_yChannels(yscale + 2)
{


  AddAxis(xscale, 0.0, xscale, parameters[0].getUnits());
  AddAxis(yscale, 0.0, yscale, parameters[1].getUnits());

  CreateMappings(parameters, 0.0, static_cast<Float_t>(xscale),
		0.0, static_cast<Float_t>(yscale));
  
}


/*!
    An overloaded constructor for 2d multiply incremented spectra.
    \param name : std::string
       Name of the spectrum.
    \param id   : UInt_t
       Unique spectrum id.
    \param parameters :std::vector<CParameter>& parameters
       Parameter definitions for the histogram.
    \param xchans : UInt_t
      Channels on x axis.
    \param ychans : UInt_t ychans
      Channels on y axis.
    \param xlow,xhigh : Float_t
      Range covered by x axis.
    \param ylow,yhigh : Float_t
      Range covered by y axis.

*/
CSpectrum2Dm:: CSpectrum2Dm(std::string              name,
			    UInt_t                   id,
			    std::vector<CParameter>& parameters,
			    UInt_t                   xchans,
			    UInt_t                   ychans,
			    Float_t  xlow, Float_t   xhigh,
			    Float_t  ylow, Float_t   yhigh) : 
  CSpectrum(name, id,
	    CreateAxisVector(parameters[0], xchans, xlow, xhigh,
			     parameters[1], ychans, ylow, yhigh)),
  m_xChannels(xchans+2),
  m_yChannels(ychans+2)
{
  AddAxis(xchans, xlow, xhigh, parameters[0].getUnits());
  AddAxis(ychans, ylow, yhigh, parameters[1].getUnits());

  CreateMappings(parameters, xlow, xhigh, ylow, yhigh);
}

/*!
   Destroy us;
*/
CSpectrum2Dm::~CSpectrum2Dm() {}

/*!
   Determines if a specific parameter is used by this spectrum.

   \param id : UInt_t
     The parameter id.
   \return bool
   \retval true  - The parameter is used.
   \retval false - The parameter is not used.
*/
Bool_t
CSpectrum2Dm::UsesParameter(UInt_t nid) const
{
  for (int i =0; i < m_parameterList.size(); i++) {
    if (m_parameterList[i] == nid) return kfTRUE;
  }
  return kfFALSE;
} 
 
/*!
  Returns the set of paramter ids used by this spectrum:
  \param rvIds : std::vecto4r<UInt_t> [out]
     A vector to which are appended the axis dimensions.
  
*/
void
CSpectrum2Dm::GetParameterIds(vector<UInt_t>& rvIds) 
{
  rvIds = m_parameterList;
}

/*!
   Returns a vector consisting of the x/y dimensions of the spectrum.
   \param rvResolutions : std::vector<UInt_t>& [out]
       A vector to which are appended the axis dimensions.
*/
void
CSpectrum2Dm::GetResolutions(vector<UInt_t>& rvResolutions)
{
  rvResolutions.push_back(m_xChannels);
  rvResolutions.push_back(m_yChannels);
}

/*!
   Return a description of the spectrum.
*/
CSpectrum::SpectrumDefinition&
CSpectrum2Dm::GetDefinition()
{
  static CSpectrum::SpectrumDefinition def;
  def = GetCommonDefinition();
  def.eDataType  = StorageType();
  return def;
}
/*!
  Returns the type of the spectrum.
*/
SpectrumType_t
CSpectrum2Dm::getSpectrumType()
{
  return ke2Dm;
}


/////////////////////////////////////////////////////////////////////
///////////////////// protected utilities //////////////////////////
////////////////////////////////////////////////////////////////////

/**
 * canIncrement
 *    Determines if the spectrum can be incremented given a x/y
 *    channel coordinate pair.  If not, the appropriate over/underflow
 *    logging is done.
 *
 *    @param ix - integer x parameter
 *    @param iy - integer y parameter
 *    @param nx - # x channels.
 *    @param ny - # y channels.
 *
 *    @return bool - true if both parameters are in range false otherwise.
 */
bool
CSpectrum2Dm::canIncrement(Int_t ix, Int_t iy, int nx, int ny)
{

  bool xok = checkRange(ix, nx, 0);
  bool yok = checkRange(iy, ny, 1);
  
  return xok && yok;
}


/*!
  This function fills in a spectrum definition with the exception
  of the spectrum data type... which is not known to this base class.
*/

CSpectrum::SpectrumDefinition
CSpectrum2Dm::GetCommonDefinition()
{
  CSpectrum::SpectrumDefinition def;
  def.sName       = getName();
  def.nId         = getNumber();
  def.eType       = ke2Dm;
  def.vParameters = m_parameterList;

  def.nChannels.clear();
  def.nChannels.push_back(m_xChannels - 2);
  def.nChannels.push_back(m_yChannels - 2);

  def.fLows.clear();
  def.fLows.push_back(GetLow(0));
  def.fLows.push_back(GetLow(1));
  
  def.fHighs.clear();
  def.fHighs.push_back(GetHigh(0));
  def.fHighs.push_back(GetHigh(1));

  return def;			// Caller fills in def.eDataType.
}

//////////////////////////////////////////////////////////////////////
/////////////////// Private utilities ////////////////////////////////
/////////////////////////////////////////////////////////////////////

/*
   Convenience routine used by constructor initializers to create the
   axis vector needed by the base class.
*/

CSpectrum::Axes
CSpectrum2Dm::CreateAxisVector(const CParameter&      xParam,
			       UInt_t                 nxChannels,
			       Float_t fxLow, Float_t fxHigh,
			       const CParameter&      yParam,
			       UInt_t                 nyChannels,
			       Float_t fyLow, Float_t fyHigh)
{
  
 CSpectrum:Axes aMappings;
 
 CAxis xMap(fxLow, fxHigh, nxChannels, 
	    CParameterMapping(xParam));
 CAxis yMap(fyLow, fyHigh, nyChannels,
	    CParameterMapping(yParam));
 
 
 aMappings.push_back(xMap);
 aMappings.push_back(yMap);
 
 return aMappings;
}

/*
  Create the various mappings we need to function.
  We need the list of parameter ids (mappings into rEvent for increment
  and also the parameter -> axis mappings.
  This code is common code used by the constructors.

*/
void 
CSpectrum2Dm::CreateMappings(vector<CParameter>& parameters,
			    Float_t xlow, Float_t xhigh,
			    Float_t ylow, Float_t yhigh)
{
  // Create our parameter list of parametr ids

  for (int i = 0; i < parameters.size(); i++) {
    m_parameterList.push_back(parameters[i].getNumber());
  }
  // Create our axis mappings...
  
  for(int i =0; i < parameters.size(); i+= 2) {
    // X axis:

    CAxis xmap(xlow, xhigh,  m_xChannels,
	       CParameterMapping(parameters[i]));
    m_axisMappings.push_back(xmap);

    CAxis ymap(ylow, yhigh, m_yChannels,
	       CParameterMapping(parameters[i+1]));
    m_axisMappings.push_back(ymap);
  }
  createStatArrays(2);
}
/*!
   This spectrum should not be classified as needing
   parmeter[0] or else it will underincrement.
*/
Bool_t
CSpectrum2Dm::needParameter() const
{
  return kfFALSE;

}
/**
 * Dimension
 *    @param axis  - axis selector 0 - x, 1 - y other CRangeError is thrown.
 *    @return Size_t Number of bins on the specified axis.
 */
Size_t
CSpectrum2Dm::Dimension(UInt_t axis) const
{
  if (axis == 0) {
    return m_xChannels;
  } else if (axis == 1) {
    return m_yChannels;
  } else {
    throw CRangeError(0, 1, axis, std::string("Getting number of bins on an axis"));
  }
}