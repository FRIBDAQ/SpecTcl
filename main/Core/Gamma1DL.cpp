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
// CGamma1DL.cpp
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
   Revision 5.5  2007/02/23 20:38:18  ron-fox
   BZ291 enhancement... add gamma deluxe spectrum type (independent x/y
   parameter lists).

   Revision 5.4  2006/06/22 17:28:37  ron-fox
   Defect 209 : more 64bit unclean issues.

   Revision 5.2.2.1  2006/06/21 14:31:24  ron-fox
   Propagate fix for defect 200 to this version

   Revision 5.1.2.3  2006/06/21 14:02:14  ron-fox
   Defect 200 g1 spectrum only have counts in every other channel because
   storage is treated as a long which is 64 bits on 64bit systems, rather than
   an int which is 32 bits on both... note this can also cause spectrum
   increments to overwrite.

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

   Revision 4.6  2004/02/03 21:32:57  ron-fox
   Make definitions of spectra from resolutions consistent with those that have ranges.

   Revision 4.5.2.1  2004/02/02 21:47:07  ron-fox
   *** empty log message ***

   Revision 4.5  2003/10/24 14:43:29  ron-fox
   Bounds check parameter ids against the size of
   of the event.

   Revision 4.4  2003/06/19 18:56:13  ron-fox
   Fix parameter order error so that Scales are properly defined in the constructor.

   Revision 4.3  2003/04/01 19:53:11  ron-fox
   Support for Real valued parameters and spectra with arbitrary binnings.

*/

//
//  Header files:
//
#include <config.h>
#include "Gamma1DL.h"                               
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
#include <TH1I.h>
#include <TDirectory.h>

#ifdef HAVE_STD_NAMESPACE
using namespace std;
#endif

// Functions for class CGamma1DL

//////////////////////////////////////////////////////////////////////////
//
//  Function:
//   CGamma1DL(const std::string& rname, UInt_t nId,
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
CGamma1DL::CGamma1DL(const std::string& rName, UInt_t nId,
		     vector<CParameter>& rrParameters,
		     UInt_t nScale) :
  CGammaSpectrum(rName, nId,
	    MakeAxesVector(rrParameters, nScale,
			   0.0, (Float_t)(nScale)), rrParameters),
  m_nScale(nScale + 2)
{
  // The assumption is that all axes have the same units.

  AddAxis(nScale, 0.0, (Float_t)(nScale - 1), rrParameters[0].getUnits());
  std::string olddir = gDirectory->GetPath();
  gDirectory->Cd("/");
  m_pRootSpectrum = new TH1I(
    rName.c_str(), rName.c_str(),
    nScale, static_cast<Double_t>(0.0), static_cast<Double_t>(nScale)
  );
  m_pRootSpectrum->Adopt(0, nullptr);
  CreateStorage();
  setRootSpectrum(m_pRootSpectrum);
  gDirectory->Cd(olddir.c_str());
  
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
CGamma1DL::CGamma1DL(const string& rName, UInt_t nId,
		     vector<CParameter>& rrParameters,
		     UInt_t nChannels,
		     Float_t fLow, Float_t fHigh) :
  CGammaSpectrum(rName, nId,
		 MakeAxesVector(rrParameters, nChannels, fLow, fHigh), rrParameters),
  m_nScale(nChannels + 2)
{
  AddAxis(nChannels, fLow, fHigh, rrParameters[0].getUnits());
  std::string olddir = gDirectory->GetPath();
  gDirectory->Cd("/");
  m_pRootSpectrum = new TH1I(
    rName.c_str(), rName.c_str(),
    nChannels, static_cast<Double_t>(fLow), static_cast<Double_t>(fHigh)
  );
  m_pRootSpectrum->Adopt(0,nullptr);
  CreateStorage();
  setRootSpectrum(m_pRootSpectrum);
  gDirectory->Cd(olddir.c_str());
  
}
/**
 * destructor
 *   Do the fancy footwork needed to ensure Root doesn't try to manage
 *   our storage for us.
 */
CGamma1DL::~CGamma1DL()
{
  m_pRootSpectrum->fArray = nullptr;
}

//////////////////////////////////////////////////////////////////////
//
// Function:
//   ULong_t operator[](const UInt_t* pIndices) const
// Operation:
//   Selector.
//

ULong_t
CGamma1DL::operator[] (const UInt_t* pIndices) const
{
  UInt_t* pStorage = (UInt_t*)getStorage();
  Double_t n = pIndices[0];
  
  Int_t bin = m_pRootSpectrum->GetBin(n);
  return static_cast<ULong_t>(m_pRootSpectrum->GetBinContent(bin));
}

///////////////////////////////////////////////////////////////////////////
//
// Function:
//    virtual void CGamma1DL::set(const UInt_t* pIndices, ULong_t nValue)
//
// Operation Type:
//    Mutator.
//

void
CGamma1DL::set (const UInt_t* pIndices, ULong_t nValue)
{
  UInt_t* pStorage = (UInt_t*)getStorage();
  Double_t n = pIndices[0];
  Int_t  bin = m_pRootSpectrum->GetBin(n);
  m_pRootSpectrum->SetBinContent(n, static_cast<Double_t>(nValue));
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
CGamma1DL::GetResolutions (vector<UInt_t>& rvResolutions)
{
  rvResolutions.push_back (m_nScale);
}


/*!
    Create the storage needed to hold the spectrum channels.
    This utility is used by all of the constructors.
*/
void
CGamma1DL::CreateStorage()
{
  setStorageType(keLong);
  Size_t nBytes = StorageNeeded();
  UInt_t* pStorage = new UInt_t[static_cast<unsigned>(nBytes/sizeof(UInt_t))];
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
CGamma1DL::MakeAxesVector(vector<CParameter> Params,
			  UInt_t             nChannels,
			  Float_t fLow, Float_t fHigh)
{
  Axes Scales;
  for(int i=0; i < Params.size(); i++) {
    Scales.push_back(CAxis(fLow, fHigh, nChannels,
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
CGamma1DL::Increment(vector<pair<UInt_t, Float_t> >& rParams)
{
  UInt_t* pStorage = (UInt_t*)getStorage();

  for(int i =0; i < rParams.size(); i++) {
    UInt_t    nParameter = rParams[i].first;
    Double_t  fValue     = rParams[i].second;
    m_pRootSpectrum->Fill(fValue);
    
  }
}

/*
   This is only used by gamma 2d deluxe... so we throw an exception:

*/
void
CGamma1DL::Increment(std::vector<std::pair<UInt_t, Float_t> >& xParameters,
			 std::vector<std::pair<UInt_t, Float_t> >& yParameters)
{
  throw CException("Invalid gamma 2d deluxe increment call in CGamma1DL");
}
/**
 * setStorage
 *    Replace the spectrum storage with new storage.  The caller is responsible
 *    for actually managing the storage.
 *  @param pStorage - pointer to the storage to be used for the spectrum
 *       channels.
 */
void
CGamma1DL::setStorage(Address_t pStorage)
{
  m_pRootSpectrum->fArray = reinterpret_cast<Int_t*>(pStorage);
  m_pRootSpectrum->fN     = m_nScale;
}
/**
 * StorageNeeded
 *   @return Size_t number of bytes of spectrum storage required.
 */
Size_t
CGamma1DL::StorageNeeded() const
{
  return m_nScale * sizeof(Int_t);
}
