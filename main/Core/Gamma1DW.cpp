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

static const char* Copyright = "(C) Copyright Michigan State University 2008, All rights reserved";
// CGamma1DW.cpp
// Encapsulates the prototypical 1-d Gamma spectrum
// Channel size is long.
// Data are dynamically allocated.
//
//
// Author:
//    Jason Venema
//    NSCL
//    Michigan State University
//    East Lansing, MI 48824-1321
//    mailto:venemaja@msu.edu
/*
  Change Log:
  $Log$
  Revision 5.3  2007/02/23 20:38:18  ron-fox
  BZ291 enhancement... add gamma deluxe spectrum type (independent x/y
  parameter lists).

  Revision 5.2  2005/06/03 15:19:22  ron-fox
  Part of breaking off /merging branch to start 3.1 development

  Revision 5.1.2.2  2005/05/27 17:47:37  ron-fox
  Re-do of Gamma gates also merged with Tim's prior changes with respect to
  glob patterns.  Gamma gates:
  - Now have true/false values and can therefore be applied to spectra or
    take part in compound gates.
  - Folds are added (fold command); and these perform the prior function
      of gamma gates.

  Revision 5.1.2.1  2004/12/21 17:51:24  ron-fox
  Port to gcc 3.x compilers.

  Revision 5.1  2004/11/29 16:56:06  ron-fox
  Begin port to 3.x compilers calling this 3.0

  Revision 4.6  2004/02/03 21:32:58  ron-fox
  Make definitions of spectra from resolutions consistent with those that have ranges.

  Revision 4.5.2.1  2004/02/02 21:47:08  ron-fox
  *** empty log message ***

  Revision 4.5  2003/10/24 14:43:29  ron-fox
  Bounds check parameter ids against the size of
  of the event.

  Revision 4.4  2003/06/19 18:56:13  ron-fox
  Fix parameter order error so that Scales are properly defined in the constructor.

  Revision 4.3  2003/04/01 19:53:12  ron-fox
  Support for Real valued parameters and spectra with arbitrary binnings.

*/
//
//  Header files:
//
#include <config.h>
#include "Gamma1DW.h"                               
#include "Parameter.h"
#include <Exception.h>
#include <RangeError.h>
#include "Event.h"
#include "GateContainer.h"
#include "Gate.h"
#include "CGammaCut.h"
#include "CGammaBand.h"
#include "CGammaContour.h"
#include "CParameterMapping.h"
#include "CAxis.h"
#include <assert.h>

#ifdef HAVE_STD_NAMESPACE
using namespace std;
#endif

// Functions for class CGamma1DW

//////////////////////////////////////////////////////////////////////////
//
//  Function:
//   CGamma1DW(const std::string& rname, UInt_t nId,
//             vector<CParameter> rrParameters,
//             UInt_t nScale)
// Operation Type:
//   Constructor
//

/*!
  Construct a 1d gamma spectrum with channels that contain
  longword values.  The axis is assumed to go from [0,nScale)
  in mapped parameter space.
  \param rName (const string& [in]) Name of the spectrum.
  \param nId   (UInt_t [in])        Id of the spectrum (supposedly
                                    unique).
  \param rrParameters (vector<CParameter> [in]) Vector containing
      descriptions of all the parameters that will be incremented
      into this histogram.  Note that parameters can have
      independent mappings between their raw values and spectrum
      coordinates.  Some parameters can be mapped and others
      umapped and others reals as well.
  \param nScale (UInt_t nScale) number of channels on the x axis.
      The X axis is assumed to cover the mapped coordinate space of
      [0.0, nScale - 1.0).  To use an arbitrary mapping between
      channels and mapped parameter space, see the next constructor.

*/
CGamma1DW::CGamma1DW(const std::string& rName, UInt_t nId,
		     vector<CParameter>& rrParameters,
		     UInt_t nScale) :
  CGammaSpectrum(rName, nId,
		 MakeAxesVector(rrParameters, nScale,
				0.0, (Float_t)(nScale)), rrParameters),
  m_nScale(nScale)
{
  // we asssume all parameters have the same units...
  AddAxis(nScale, 0.0, (Float_t)(nScale - 1), rrParameters[0].getUnits());
  CreateStorage();

}
/*!
   Construct a 1d Gamma spectrum with channels that contain 
   longwordd values.  The axis represents the interval
   [fLow,fHigh] in mapped parameter space.

  \param rName (const string& [in]) Name of the spectrum.
  \param nId   (UInt_t [in])        Id of the spectrum (supposedly
                                    unique).
  \param rrParameters (vector<CParameter> [in]) Vector containing
      descriptions of all the parameters that will be incremented
      into this histogram.  Note that parameters can have
      independent mappings between their raw values and spectrum
      coordinates.  Some parameters can be mapped and others
      umapped and others reals as well.
  \param nChannels (UInt_t [in]) number of channels on the x axis.
  \param fLow   (Float_t [in])  The mapped parameter space value
      represented by channel 0 on the x axis.
  \param fHigh  (Float_t [in]):  The mapped parameter space value
      represented by channel nChannels-1 on the x axis
*/
CGamma1DW::CGamma1DW(const string& rName, UInt_t nId,
		     vector<CParameter>& rrParameters,
		     UInt_t nChannels,
		     Float_t fLow, Float_t fHigh) :
  CGammaSpectrum(rName, nId,
	    MakeAxesVector(rrParameters, nChannels, fLow, fHigh), rrParameters),
  m_nScale(nChannels)
{
  AddAxis(nChannels, fLow, fHigh, rrParameters[0].getUnits());
  CreateStorage();
}


//////////////////////////////////////////////////////////////////////
//
// Function:
//   ULong_t operator[](const UInt_t* pIndices) const
// Operation:
//   Selector.
//

ULong_t
CGamma1DW::operator[] (const UInt_t* pIndices) const
{
  UShort_t* pStorage = (UShort_t*)getStorage();
  UInt_t n = pIndices[0];
  if (n >= Dimension(0)) {
    throw CRangeError(0, Dimension(0)-1, n, 
		      std::string("Indexing 1DL gamma spectrum"));
  }
  return (ULong_t)pStorage[n];
}

///////////////////////////////////////////////////////////////////////////
//
// Function:
//    virtual void CGamma1DW::set(const UInt_t* pIndices, ULong_t nValue)
//
// Operation Type:
//    Mutator.
//

void
CGamma1DW::set (const UInt_t* pIndices, ULong_t nValue)
{
  UShort_t* pStorage = (UShort_t*)getStorage();
  UInt_t n = pIndices[0];
  if (n >= Dimension(0)) {
    throw CRangeError(0, Dimension(0)-1, n,
		      std::string("Indexing 1DL gamma spectrum"));
  }
  pStorage[n] = (UShort_t)nValue;
}


///////////////////////////////////////////////////////////////////////
//
//
// Function:
//    void GetResolutions(vector<UInt_t>&  rvResolutions)
// Operation type:
//    Selector
//

void
CGamma1DW::GetResolutions (vector<UInt_t>& rvResolutions)
{
  rvResolutions.push_back (m_nScale);
}


/*!
    Create the storage needed to hold the spectrum channels.
    This utility is used by all of the constructors.
*/
void
CGamma1DW::CreateStorage()
{
  setStorageType(keWord);
  Size_t nBytes = StorageNeeded();
  UShort_t* pStorage = new UShort_t[nBytes/sizeof(UShort_t)];
  ReplaceStorage(pStorage);
  Clear();
  createStatArrays(1);
}

/*!
   Creates a vector of axis definitions.  This is needed
   because while the axis is a fixe cut in parameter space, if
   the user chose to use mapped parameters, the mapping between
   parameter and axis will be different from parameter to parameter.
   \param Params (vector<CParameter>  [in]) A vector of parameter
      descriptions.
   \param nChannels (UInt_t [in]) The number of channels on the X
      axis of the spectrum.
   \param fLow (Float_t [in]) The coordinate in mapped parameter
      space represented by channel 0 on the X axis.
   \param fHigh (Float_t [in] The coordiante in mapped parameter
      space represented by channel nChannels-1 on the X axis.

   \return CAxes - a vector of axes that is suitable to be used
     when constructing the CSpectrum base class.
*/
CSpectrum::Axes
CGamma1DW::MakeAxesVector(vector<CParameter> Params,
			  UInt_t             nChannels,
			  Float_t fLow, Float_t fHigh)
{
  Axes Scales;
  for(int i=0; i < Params.size(); i++) {
    Scales.push_back(CAxis( fLow, fHigh, nChannels,
			   CParameterMapping((Params[i]))));
  }
  return Scales;
}

/**
 * Increment the spectrum indirectly from a fold
 * @param rParams
 *    An array of parameter id/value pairs.
 */
void
CGamma1DW::Increment(vector<pair<UInt_t, Float_t> >& rParams)
{
  UShort_t* pStorage = (UShort_t*)getStorage();
  for(int i =0; i < rParams.size(); i++) {
    UInt_t   nParameter = rParams[i].first;
    Float_t  fValue     = rParams[i].second;
    Int_t   y          = (Int_t)ParameterToAxis(0, fValue);
  
    if(checkRange(y, m_nScale, 0)) {
      pStorage[y]++;
    }
    
  }
}

//! This is for g2dd only so we throw:

void
CGamma1DW::Increment(std::vector<std::pair<UInt_t, Float_t> >& xParameters,
			 std::vector<std::pair<UInt_t, Float_t> >& yParameters)
{
  throw CException("Gamma 2d Deluxe increment called for CGamma1DW");
}
