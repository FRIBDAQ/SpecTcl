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
// Encapsulates the prototypical 2-d Gamma spectrum
// Channel size is word.
// Data are dynamically allocated.
//
//
// Author:
//    Jason Venema
//    NSCL
//    Michigan State University
//    East Lansing, MI 48824-1321
//    mailto:venemaja@msu.edu

/*!
  Change log:
    $Log$
    Revision 5.4  2007/02/23 20:38:18  ron-fox
    BZ291 enhancement... add gamma deluxe spectrum type (independent x/y
    parameter lists).

    Revision 5.3  2005/09/22 12:40:00  ron-fox
    Fix defects in gamma 2d spectrum increment.  When there are no valid
    parameters the outer loop limits are bad and eventually lead to segflt

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

    Revision 4.5.2.1  2004/02/02 21:47:08  ron-fox
    *** empty log message ***

    Revision 4.5  2003/10/24 14:43:29  ron-fox
    Bounds check parameter ids against the size of
    of the event.

    Revision 4.4  2003/04/01 19:53:12  ron-fox
    Support for Real valued parameters and spectra with arbitrary binnings.

*/


//
// Header Files:
//
#include <config.h>
#include "Gamma2DW.h"
#include "Parameter.h"
#include "RangeError.h"
#include "Event.h"
#include "GateContainer.h"
#include "Gate.h"
#include "CGammaCut.h"
#include "CGammaBand.h"
#include "CGammaContour.h"
#include <assert.h>
#include <TH2S.h>


#ifdef HAVE_STD_NAMESPACE
using namespace std;
#endif

// Functions for class CGamma2DW

//////////////////////////////////////////////////////////////////////////
//
//  Function:
//   CGammaDW(const std::string& rname, UInt_t nId,
//            vector<CParameter>& rParameters,
//            UInt_t nxScale, UInt_t nYScale)
// Operation Type:
//   Constructor
//

CGamma2DW::CGamma2DW(const std::string& rName, UInt_t nId,
		     vector<CParameter>& rParameters,
		     UInt_t nXScale, UInt_t nYScale) :
  CGammaSpectrum(rName, nId,
	    CreateAxisVector(rParameters,
			     nXScale, nYScale,
			     0.0,    (Float_t)(nXScale ),
			     0.0,    (Float_t)(nYScale)),
		 rParameters),
  m_nXScale(nXScale + 2),
  m_nYScale(nYScale + 2)
{

  // The assumption is all parameters have the same units.

  AddAxis(nXScale, 0.0, (Float_t)(nXScale - 1), rParameters[0].getUnits());
  AddAxis(nYScale, 0.0, (Float_t)(nYScale - 1), rParameters[0].getUnits());
  
  m_pRootSpectrum = new TH2S(
    rName.c_str(), rName.c_str(),
    nXScale, static_cast<Double_t>(0), static_cast<Double_t>(nXScale),
    nYScale, static_cast<Double_t>(0), static_cast<Double_t>(nYScale)
  );
  m_pRootSpectrum->Adopt(0, nullptr);
  setRootSpectrum(m_pRootSpectrum);
  CreateStorage();
  
}
/*!
  Construct a gamma spectrum where the axis is an arbitrary
  transformation of the parameters.
  \param rName  (const std::string& [in]) The name of the 
  spectrum.
  \param nId    (UInt_t [in]) The integer id of the spectrum.
  \param rParameters (vector<CParameter>& [in]) A vector of 
  parameters on which to increment the spectrum.
  \param nXScale (UInt_t [in]) The number of channels on the X
  axis of the spectrum.
  \param nYScale (UInt_t [in]) The number of channels on the Y 
  axis of the spectrum.
  \param xLow (Float_t [in])  The parameter coordinate corresponding
  to channel 0 of the x axis.
  \param xHigh (Float_t [in]) The parameter coordinate corresponding
  to channel nXScale - 1 on the spectrum.
  \param yLow  (Float_t [in]) The parameter coordinate corresponding
  to channel 0 of the y axis.
  \param yHigh (Float_t [in]) The parameter coordinate corresponding
  to channel (nYScale) of the y-axis.
  
*/
CGamma2DW::CGamma2DW(const std::string& rName, UInt_t nId,
		     vector<CParameter>& rParameters,
		     UInt_t nXScale, UInt_t nYScale,
		     Float_t xLow, Float_t xHigh,
		     Float_t yLow, Float_t yHigh) :
  CGammaSpectrum(rName, nId,
	    CreateAxisVector(rParameters, nXScale, nYScale,
			     xLow, xHigh, yLow, yHigh), rParameters),
  m_nXScale(nXScale + 2),
  m_nYScale(nYScale + 2)
{
  // The assumption is all paramters have the same units.
  AddAxis(nXScale, xLow, xHigh, rParameters[0].getUnits());
  AddAxis(nYScale, yLow, yHigh, rParameters[0].getUnits());

  m_pRootSpectrum = new TH2S(
    rName.c_str(), rName.c_str(),
    nXScale, static_cast<Double_t>(xLow), static_cast<Double_t>(xHigh),
    nYScale, static_cast<Double_t>(yLow), static_cast<Double_t>(yHigh)
  );
  m_pRootSpectrum->Adopt(0, nullptr);
  setRootSpectrum(m_pRootSpectrum);
  CreateStorage();
  
}

/**
 * destructor
 */
CGamma2DW::~CGamma2DW()
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
CGamma2DW::operator[] (const UInt_t* pIndices) const
{
  
  Double_t nx = pIndices[0];
  Double_t ny = pIndices[1];
  if (nx >= Dimension(0)) {
    throw CRangeError(0, Dimension(0)-1, nx,
		      std::string("Indexing 2DW gamma spectrum x axis"));
  }
  if (ny >= Dimension(1)) {
    throw CRangeError(0, Dimension(1)-1, ny,
		      std::string("Indexing 2DW gamma spectrum y axis"));
  }
  Int_t bin  = m_pRootSpectrum->FindBin(nx, ny);
  return static_cast<ULong_t>(m_pRootSpectrum->GetBinContent(bin));
}

///////////////////////////////////////////////////////////////////////////
//
// Function:
//    virtual void CGamma2DW::set(const UInt_t* pIndices, ULong_t nValue)
//
// Operation Type:
//    Mutator.
//

void
CGamma2DW::set (const UInt_t* pIndices, ULong_t nValue)
{
  
  Double_t nx = pIndices[0];
  Double_t ny = pIndices[1];
  if (nx >= Dimension(0)) {
    throw CRangeError(0, Dimension(0)-1, nx,
		      std::string("Indexing 2DW gamma spectrum x axis"));
  }
  if (ny >= Dimension(1)) {
    throw CRangeError(0, Dimension(1)-1, ny,
		      std::string("Indexing 2DW gamma spectrum y axis"));
  }
  Int_t bin = m_pRootSpectrum->FindBin(nx, ny);
  m_pRootSpectrum->SetBinContent(bin, static_cast<Double_t>(nValue));
}




////////////////////////////////////////////////////////////////////////
//
// Function:
//    void GetResolutions(vector<UInt_t>&  rvResolutions)
// Operation type:
//    Selector
//

void
CGamma2DW::GetResolutions (vector<UInt_t>& rvResolutions)
{
  rvResolutions.clear();
  rvResolutions.push_back(m_nXScale);
  rvResolutions.push_back(m_nYScale);
}



/*!
   Create the storage needed for the spectrum.  The storage
   is allocated from the heap:
*/
void
CGamma2DW::CreateStorage()
{
  setStorageType(keWord);
  Size_t nBytes = StorageNeeded();
  UShort_t* pStorage = new UShort_t[static_cast<unsigned>(nBytes/sizeof(UShort_t))];

  ReplaceStorage(pStorage);
  Clear();
  createStatArrays(2);
}

/*!
   Creates an axis transformation vector for the gamma spectrum.
   The transformation vector will contain two entries for each 
   parameter.  One for its Xaxis transformation and one for its
   y axis transformation.  All x transforms will come first and
   then all y axis transforms... in the order in which the parameters
   appear in the parameter array passed to us.

   \param rParams (vector<CParameter>& [in]): The set of parameters
       that can increment this spectrum.
   \param nXchan (UInt_t [in]): The number of channels on the x
       axis of the spectrum.
   \param nYchan (UInt_t [in]): The number of channels on the y
       axis of the spectrum.
   \param xLow (Float_t [in]):  The parameter value that transforms
       to channel 0 of the x axis.
   \param xHigh (Float_t [in]):  The parameter value that transforms
       to channel nXchan-1 of the x axis.
   \param yLow  (Float_t [in]):  The parameter value that transforms
       to channel 0 of the y axis.
   \param yHigh (Float_t [in]):  The parameter value that transforms
       to channel nYchan-1 of the y axis.

*/
CSpectrum::Axes 
CGamma2DW::CreateAxisVector(vector<CParameter>& rParams,
			    UInt_t nXchan, UInt_t nYchan,
			    Float_t xLow, Float_t xHigh,
			    Float_t yLow, Float_t yHigh)
{
  Axes Result;			// Build up the axis vector here.

  // Fill in the x transforms...

  vector<CParameter>::iterator i = rParams.begin();
  while(i != rParams.end()) {
    Result.push_back(CAxis(xLow, xHigh, nXchan,
			   CParameterMapping(*i)));
			   
    i++;
  }

  // And then the y transforms..

  i = rParams.begin();
  while(i != rParams.end()) {
    Result.push_back(CAxis(yLow, yHigh, nYchan,
			   CParameterMapping(*i)));

    i++;
  }

  return Result;
}
/*!
   Returns the number of channels in a particular axis:
   \param <TT>n (UInt_t [in]) </TT>
   Axis number 0: x 1: y 

   \retval UInt_t
      Number of channels on the axis or 0 if the axis selector
      was not valid.
*/
Size_t 
CGamma2DW::Dimension(UInt_t n) const
{
  switch(n) {
  case 0:
    return m_nXScale;
  case 1:
    return m_nYScale;
  default:
    return 0;
  }
}
/*!
   Increment callback from the fold.  
*/
void
CGamma2DW::Increment(vector<pair<UInt_t, Float_t> >& rParameters)
{
 
  
  if(rParameters.size() > 0) {

    for(int i = 0; i < rParameters.size() - 1; i++) {
      for(int j = i+1; j < rParameters.size(); j++ ) {

        Double_t xval = rParameters[i].second;
        Double_t yval = rParameters[j].second;
        
        m_pRootSpectrum->Fill(xval, yval);
      }
    }
  }
}
//! deluxe increment on 2dw spectrum is an error:

void
CGamma2DW:: Increment(std::vector<std::pair<UInt_t, Float_t> >& xParameters,
			 std::vector<std::pair<UInt_t, Float_t> >& yParameters)
{
  throw CException("2d deluxe increment called on CGamma2dW");
}
/**
 * setStorage
 *     Called to change where spectrum storage is.
 * @param pStorage - pointer to the new spectrum storage.
 */
void
CGamma2DW::setStorage(Address_t pStorage)
{
  m_pRootSpectrum->fArray = reinterpret_cast<Short_t*>(pStorage);
  m_pRootSpectrum->fN     = m_nXScale * m_nYScale;
}
/**
 * StorageNeeded
 *    @return Size_t - Number of bytes of spectrum storage required.
*/
Size_t
CGamma2DW::StorageNeeded() const
{
  return static_cast<Size_t>( m_nXScale * m_nYScale * sizeof(Short_t));
}