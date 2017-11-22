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
//  CSummarySpectrumB.cpp
// Encapsulates the prototypical 1-d Spectrum.  
// For the purposes of the functional prototype,
// the spectrum is a singly incremented
// 1-d spectrum with longword channel size
//  data are dynamically allocated initially (self owned).
// 
//
//   Author:
//      Ron Fox
//      NSCL
//      Michigan State University
//      East Lansing, MI 48824-1321
//      mailto:fox@nscl.msu.edu
//
//////////////////////////.cpp file/////////////////////////////////////////////////////

/*
  Change Log:
  $Log$
  Revision 5.2  2005/06/03 15:19:24  ron-fox
  Part of breaking off /merging branch to start 3.1 development

  Revision 5.1.2.1  2004/12/21 17:51:26  ron-fox
  Port to gcc 3.x compilers.

  Revision 5.1  2004/11/29 16:56:09  ron-fox
  Begin port to 3.x compilers calling this 3.0

  Revision 4.6  2004/02/03 21:32:58  ron-fox
  Make definitions of spectra from resolutions consistent with those that have ranges.

  Revision 4.5.2.1  2004/02/02 21:47:08  ron-fox
  *** empty log message ***

  Revision 4.5  2003/11/07 21:48:30  ron-fox
  Fix error in CSummarySpectrmB.cpp' s
  increment.  It would sometimes not increment
  when it should

  Revision 4.4  2003/10/24 14:43:29  ron-fox
  Bounds check parameter ids against the size of
  of the event.

  Revision 4.3  2003/04/19 00:11:13  ron-fox
  Fix a nasty issue with GetDefinition() that was causing death due to a number of problems with the static output struct.  For later: change the struct to a class so that it can be returned by value rather than by reference.. then it wouldn't have to be static.

  Revision 4.2  2003/04/01 19:53:46  ron-fox
  Support for Real valued parameters and spectra with arbitrary binnings.

*/

//
// Header Files:
//

#include <config.h>
#include "SummarySpectrumB.h"                               
#include "Parameter.h"
#include "RangeError.h"
#include "Event.h"
#include <algorithm>
#include <assert.h>
#include <TH2C.h>
#include <TDirectory.h>

#ifdef HAVE_STD_NAMESPACE
using namespace std;
#endif

// Functions for class CSummarySpectrumB

/*!
  Construct a summary spectrum.   This constructor creates a
  summary spectrum with a Y axis that has an identity coordinate
  transform: Y axis represents the mapped parameter interval
  [0.0, nYScale).

  \param rName   (const string& [in]) Name of the spectrum.
  \param nId     (UInt_t [in]) Id to associate with the spectrum.
  \param rrParameters (vector<CParameter> [in]) an array of parameter
      definitions that describe the parameters to associate with 
      each X-axis column.  Each parameter will be individually 
      scaled to the Y axis on the basis of its mapping information
      and the common Y axis range information.
  \param nYScale (UInt_t [in]) Number of y axis channels.  The 
      number of X axis channels is just rrParameters.size().


*/
CSummarySpectrumB::CSummarySpectrumB(const std::string& rName, 
				     UInt_t nId,
				     vector<CParameter> rrParameters,
				     UInt_t nYScale) :

  CSpectrum(rName, nId,
	   CreateAxes(rrParameters, nYScale, 
		      0.0, (Float_t)(nYScale - 1)) ),
  m_nYScale(nYScale + 2),
  m_nXChannels(rrParameters.size() + 2)
{
  // The assumption is that all parameters have the same units.
  AddAxis(rrParameters.size(), 0.0, 
	  (Float_t)(rrParameters.size())); // Unitless
  AddAxis(nYScale, 0.0, (Float_t)(nYScale),
	  rrParameters[0].getUnits());
  FillParameterArray(rrParameters);
  std::string olddir = gDirectory->GetPath();
  gDirectory->Cd("/");
  TH2C* pRootSpectrum = new TH2C(
    rName.c_str(), rName.c_str(),
    rrParameters.size(), static_cast<Double_t>(0), static_cast<Double_t>(rrParameters.size()),
    nYScale, static_cast<Double_t>(0), static_cast<Double_t>(nYScale)
  );
  pRootSpectrum->Adopt(0, nullptr);
  setRootSpectrum(pRootSpectrum);
  gDirectory->Cd(olddir.c_str());
  
  CreateStorage();

}
/*!
   Create a summary spectrum with a y axis that represents an 
   arbitrary cut from mapped parameter space.

   \param rName (const string& [in]):
       Name of the spectrum.
   \param nId  (UInt_t [in]):
       Spectrum id.
   \param rrParameters (vector<CParameter> [in]):
       Vector of parameter definitions that describe what increments
       the histogram.
   \param nYScale     (UInt_t [in]):
       Number of channels on the y axis.  The x axis size is
       determined by the size of the parameter vector.
   \param Float_t fYLow (Float_t [in]):
       Low limit of the y axis.
   \param Float_t fYHigh (Float_t [in]):
       High limit of the y axis.
*/
CSummarySpectrumB::CSummarySpectrumB(const std::string& rName, 
				     UInt_t nId,
				     vector<CParameter> rrParameters,
				     UInt_t nYScale,
				     Float_t fYLow,
				     Float_t fYHigh) :
  CSpectrum(rName, nId,
	    CreateAxes(rrParameters, nYScale, fYLow, fYHigh)),
  m_nYScale(nYScale + 2),
  m_nXChannels(rrParameters.size() + 2)

{
  AddAxis(rrParameters.size(), 0.0, 
	  (Float_t)(rrParameters.size() - 1)); // Unitless
  AddAxis(nYScale, fYLow, fYHigh,
	  rrParameters[0].getUnits());
  FillParameterArray(rrParameters);
  std::string olddir = gDirectory->GetPath();
  gDirectory->Cd("/");
  TH2C* pRootSpectrum  = new TH2C(
    rName.c_str(), rName.c_str(),
    rrParameters.size(), static_cast<Double_t>(0), static_cast<Double_t>(rrParameters.size()),
    nYScale, static_cast<Double_t>(fYLow), static_cast<Double_t>(fYHigh)
  );
  pRootSpectrum->Adopt(0, nullptr);
  setRootSpectrum(pRootSpectrum);
  gDirectory->Cd(olddir.c_str());
  
  CreateStorage();

}

/**
 * Destructor - unmanage root spectra.
 */
CSummarySpectrumB::~CSummarySpectrumB()
{
  TH2C* pRootSpectrum = reinterpret_cast<TH2C*>(getRootSpectrum());
  pRootSpectrum->fArray  = nullptr;
  
}
//////////////////////////////////////////////////////////////////////////
//
//  Function:   
//    void Increment ( const CEvent& rE )
//  Operation Type:
//     mutator
//
void 
CSummarySpectrumB::Increment(const CEvent& rEv) 
{
// Increments channel number rEvent[m_nParameter] >> m_nScaleDifference
// Formal Parameters:
//
//          const CEvent& rEvent:
//               Event which drives the histogramming
//

  CEvent& rEvent((CEvent&)rEv);
  int      nParams  = rEvent.size();

  for(UInt_t xChan = 0; xChan < m_vParameters.size(); xChan++) {
    if(m_vParameters[xChan] < nParams) {
      if(rEvent[m_vParameters[xChan]].isValid()) {
        Double_t rawParam = rEvent[m_vParameters[xChan]];
        getRootSpectrum()->Fill(
          static_cast<Double_t>(xChan), static_cast<Double_t>(rawParam)
        );
      }
    }
  }
}
//////////////////////////////////////////////////////////////////////////
//
//  Function:   
//    Boolt_t UsesParameter (UInt_t nId)
//  Operation Type:
//     Selector
     
//
Bool_t 
CSummarySpectrumB::UsesParameter(UInt_t nId) const
{
// Returns nParameter == m_nParameter
//
// Formal Parameters:
//      UInt_t nParameter:
//        The parameter being checked.

  for(UInt_t i = 0; i < m_vParameters.size(); i++) {
    if( m_vParameters[i] == nId)
      return kfTRUE;
  }
  return kfFALSE;

}

//////////////////////////////////////////////////////////////////////
//
// Function:
//   ULong_t operator[](const UInt_t* pIndices) const
// Operation:
//   Selector.
//
ULong_t
CSummarySpectrumB::operator[](const UInt_t* pIndices) const
{
  // Provides the value of an element of the spectrum.
  // note:  This is not a 'normal' indexing operation in that
  // references are not returned.
  //
  Int_t   nx = pIndices[0];
  Int_t   ny = pIndices[1];
  if(nx + 2 >= Dimension(0)) {
    throw CRangeError(0, Dimension(0)-1, nx,
		      std::string("Indexing SummaryW spectrum x axis"));
  }
  if(ny +2 >= Dimension(1)) {
    throw CRangeError(0, Dimension(1)-1, ny,
		      std::string("Indexing SummaryW spectrum y axis"));
  }
  const TH1* pRootSpectrum = getRootSpectrum();
  return static_cast<ULong_t>(pRootSpectrum->GetBinContent(
    pRootSpectrum->GetBin(nx+1, ny+1)
  ));
		      
}
///////////////////////////////////////////////////////////////////////////
//
// Function:
//    virtual void CSummarySpectrumB::set(const UInt_t* pIndices, ULong_t nValue)
//
// Operation Type:
//    Mutator.
//
void
CSummarySpectrumB::set(const UInt_t* pIndices, ULong_t nValue)
{
  // Provides write access to a channel of the spectrum.
  //
  
  Int_t   nx = pIndices[0];
  Int_t   ny = pIndices[1];
  if(nx +2 >= Dimension(0)) {
    throw CRangeError(0, Dimension(0)-1, nx,
		      std::string("Indexing 2DW spectrum x axis"));
  }
  if(ny + 2 >= Dimension(1)) {
    throw CRangeError(0, Dimension(1)-1, ny,
		      std::string("Indexing 2DW spectrum y axis"));
  }
  TH1* pRootSpectrum = getRootSpectrum();
  pRootSpectrum->SetBinContent(
    pRootSpectrum->GetBin(nx+1, ny+1), static_cast<Double_t>(nValue)
  );
}

///////////////////////////////////////////////////////////////////////////
//
// Function
//   void GetParameterIds(vector<UInt_t>& rvIds)
// Operation type:
//   Selector.
void 
CSummarySpectrumB::GetParameterIds(vector<UInt_t>& rvIds)
{
  // Gets the set of parameter ids which make up this 
  // spectrum.  In this case this is just the x and y
  // parameters.
  // 
  // Formal Parameters:
  //    vector<UInt_t>& rvIds:
  //       vector which will contain the list of parameter ids which go into
  //       this spectrum.
  //
  rvIds = m_vParameters;

}
////////////////////////////////////////////////////////////////////////
//
// Function:
//    void GetResolutions(vector<UInt_t>&  rvResolutions)
// Operation type:
//    Selector
void
CSummarySpectrumB::GetResolutions(vector<UInt_t>&  rvResolutions)
{
  // Returns a vector containing the resolutions of the spectrum.
  // In this case just m_nYScale.
  //
  // Formal Parameters:
  //    vector<UInt_t>&  rvResolutions:
  //          Vector which will contain the resolutions.

  rvResolutions.push_back(m_nYScale);
}

/*!
   Fill the parameter array from a set of parameter definitions.
   \param rrParameters (vector<CParam> [in]):  The set of parameters
      whose ids we need.
*/
void
CSummarySpectrumB::FillParameterArray(vector<CParameter> rrParameters)
{
  // Fill in the parameter and scale difference vector members:

  for(UInt_t i = 0; i < rrParameters.size(); i++) {
    CParameter& rParam(rrParameters[i]);
    m_vParameters.push_back(rParam.getNumber());
  }
}
/*!
  Allocate the storage required by the spectrum and 
  make it self owned.
*/
void
CSummarySpectrumB::CreateStorage()
{

  setStorageType(keByte);

  UInt_t        nBytes   = StorageNeeded();
  UChar_t*      pStorage = new UChar_t[nBytes/sizeof(UChar_t)];

  ReplaceStorage(pStorage);	// Storage now owned by parent.
  Clear();
  
  createStatArrays(1);        // Can only over/under flow on y.
}
/*!
   Creates a spectrum defintion that is suffient to allow the
   spectrum to be re-created.  THe base class default implementation
   is almost correct, however we'll need to remove the x axis 
   definition since that's implicitly defined from the number of
   parameters.
   \retval CSpectrum::SpectrumDefinition
     A struct that defines the spectrum.
*/

CSpectrum::SpectrumDefinition&
CSummarySpectrumB::GetDefinition()
{
  // Get the base class's idea of our definition.

  static CSpectrum::SpectrumDefinition Def;
  Def = CSpectrum::GetDefinition();

  // Remove the X axis information from nChannels, fLows, fHighs:

  Def.nChannels.erase(Def.nChannels.begin()); // X is first axis.
  Def.fLows.erase(Def.fLows.begin());
  Def.fHighs.erase(Def.fHighs.begin());

  return Def;  
}
/*!
   Creates the axis mapping array for the constructor.
   \param <TT> Parameters (vector<CParameter> Params [in]) </TT>
        The set of parameters on the spectrum.
   \param <TT> nChannels (UInt_t [in]) </TT>
       The nubmer of channels on the y axis.
   \param <TT> fyLow (Float_t [in]) </TT>
       The parameter value that represents channel 0 on the y axis.
   \param <TT> fyHigh (Float_t [in]) </TT>
       The parameter value that represents the last channel of the
       y axis.
   \retval CSpectrum::Axes
      An array of axis scaling objects.
*/
CSpectrum::Axes
CSummarySpectrumB::CreateAxes(vector<CParameter> Parameters,
			      UInt_t             nChannels,
			      Float_t fyLow, Float_t fyHigh)
{
  CSpectrum::Axes maps;
  for(int i =0; i < Parameters.size(); i++) {
    maps.push_back(CAxis(fyLow, fyHigh, nChannels, 
			 CParameterMapping(Parameters[i])));
  }
  return maps;
}
/*!
   Summary spectra don't actually need their parameters:

*/
Bool_t
CSummarySpectrumB::needParameter() const
{
  return kfFALSE;
}

/**
 *  Set the spectrum to live somewhere else.
 *
 * @param pStorage - pointer to new spectrum channel storage.
 */
void
CSummarySpectrumB::setStorage(Address_t pStorage)
{
  TH2C* pRootSpectrum = reinterpret_cast<TH2C*>(getRootSpectrum());
  pRootSpectrum->fArray = reinterpret_cast<Char_t*>(pStorage);
  pRootSpectrum->fN     = m_nXChannels * m_nYScale ;
}
/**
 * StorageNeeded
 *
 * @return Size_t number of bytes of storage required.
 */
Size_t
CSummarySpectrumB::StorageNeeded() const
{
  return m_nXChannels * m_nYScale * sizeof(Char_t);
}
/**
 * Dimension
 *    Return the # bins on one axis:
 *
 *  @param axis - 0 or 1
 */
Size_t
CSummarySpectrumB::Dimension(UInt_t axis) const
{
  if (axis == 0) {
    return m_nXChannels;
  } else if (axis == 1) {
    return m_nYScale;
  } else {
    throw CRangeError(0, 1, axis,
		      std::string("Selecting axis number."));
  }
}