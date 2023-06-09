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


//
// Header Files:
//

#include <config.h>
#include "Spectrum2DL.h"                               
#include "Parameter.h"
#include "RangeError.h"
#include "Event.h"
#include "CAxis.h"
#include "CParameterMapping.h"
#include <TH2I.h>
#include <TDirectory.h>

#ifdef HAVE_STD_NAMESPACE
using namespace std;
#endif
// Functions for class CSpectrum2DL

//////////////////////////////////////////////////////////////////////////
//
//  Function:
//   CSpectrum2DL(const std::string& rname, UInt_t nId,
//               const CParameter& rXParameter, const CParameter rYParmeter,
//               UInt_t            nxScale, UInt_t nYScale)
// Operation Type:
//   Constructor
//

/*!
   Construct a spectrum.  This constructor creates a spectrum
   with Axis mapping:
   - Xaxis [0, nXScale),
   - Yaxis [0, nYScale).
   
   \param rName (const string& [in]) Name of the spectrum being
      created.
   \param nId   (UInt_t [in]) Id of the spectrum being created.
   \param rxParameter (const CParameter& [in]): Describes the
        parameter on the Xaxis of the histogram.
   \param ryParameter (const CParameter& [in]): Describes the
        parameter no the Y axis of the histogram.
   \param nXScale  (UInt_t [in]) Number of channels on the X axis.
   \param nYScale  (UInt_t [in]) Number of channels on the Y axis.

*/
CSpectrum2DL::CSpectrum2DL(const std::string& rName, UInt_t nId,
			   const CParameter& rXParameter, 
			   const CParameter& rYParameter,
			   UInt_t nXScale, UInt_t nYScale) :
  CSpectrum(rName, nId,
	    CreateAxisVector(rXParameter, 
			     nXScale, 0.0, (Float_t)(nXScale-1),
			     rYParameter, 
			     nYScale, 0.0, (Float_t)(nYScale-1))),
  m_nXScale(nXScale + 2),                 // Account for Root needing 2
  m_nYScale(nYScale + 2),                 // extra chans on each axis.
  m_nXParameter(rXParameter.getNumber()),
  m_nYParameter(rYParameter.getNumber())

{
  AddAxis(nXScale, 0.0, (Float_t)(nXScale), 
	  rXParameter.getUnits());
  AddAxis(nYScale, 0.0, (Float_t)(nYScale), 
	  rYParameter.getUnits());
  
  // Create the root spectrum and set its storage to null so that it gets
  // deleted.  CreateStorate indirectly calls setStorage which will swap in
  // SpecTcl managed storage for the root TArrayI normally used:
  
  std::string olddir = gDirectory->GetPath();
  gDirectory->Cd("/");
  TH2I* pRootSpectrum = new TH2I(
    rName.c_str(), rName.c_str(),
    nXScale, 0.0, static_cast<Double_t>(nXScale),
    nYScale, 0.0, static_cast<Double_t>(nYScale)
  );
  pRootSpectrum->Adopt(0, nullptr);          // Free the root storage.
  setRootSpectrum(pRootSpectrum);
  gDirectory->Cd(olddir.c_str());
  
  CreateStorage();
}

/*!
  Create a 2d spectrum with an arbitrary axis mapping.
  - Xaxis contains nXChannels that represent the interval
     [fxLow, fxHigh]
  - Yaxis contain nYChannels that represent the interval
     [fyLOw, fyHigh].

   \param rName (const string& [in]) Name of the spectrum being
      created.
   \param nId   (UInt_t [in]) Id of the spectrum being created.
   \param rxParameter (const CParameter& [in]): Describes the
        parameter on the Xaxis of the histogram.
   \param ryParameter (const CParameter& [in]): Describes the
        parameter no the Y axis of the histogram.
   \param nXChannels  (UInt_t [in]) Number of channels on the X axis.
   \param fxLow (Float_t [in]) Low limit of the x axis.
   \param fxHigh (Float_t [in]) High limit of the x axis.
   \param nYChannels  (UInt_t [in]) Number of channels on the Y axis.
   \param fyLow (Float_t [in]) Low limit of the y axis.
   \param fyHigh (Float_t [in]) High limit of the y axis.


*/
CSpectrum2DL:: CSpectrum2DL(const std::string& rName, UInt_t nId,
			    const CParameter& rXParameter,
			    const CParameter& rYParameter,
			    UInt_t nXChannels, 
			    Float_t fxLow, Float_t fxHigh,
			    UInt_t nYChannels, 
			    Float_t fyLow, Float_t fyHigh) :
  CSpectrum(rName, nId,
	    CreateAxisVector(rXParameter, nXChannels,
			     fxLow, fxHigh,
			     rYParameter, nYChannels,
			     fyLow, fyHigh)),
  m_nXScale(nXChannels + 2),
  m_nYScale(nYChannels + 2),
  m_nXParameter(rXParameter.getNumber()),
  m_nYParameter(rYParameter.getNumber())
  
{
  AddAxis(nXChannels, fxLow, fxHigh, rXParameter.getUnits());
  AddAxis(nYChannels, fyLow, fyHigh, rYParameter.getUnits());
  
  // See comments in previousl constructor:
  
  std::string olddir = gDirectory->GetPath();
  gDirectory->Cd("/");
  TH2I* pRootSpectrum = new TH2I(
    rName.c_str(),  rName.c_str(),
    nXChannels,  static_cast<Double_t>(fxLow), static_cast<Double_t>(fxHigh),
    nYChannels,  static_cast<Double_t>(fyLow), static_cast<Double_t>(fyHigh)
  );
  pRootSpectrum->Adopt(0, nullptr);
  setRootSpectrum(pRootSpectrum);
  gDirectory->Cd(olddir.c_str());
  
  CreateStorage();
}
  
CSpectrum2DL::~CSpectrum2DL()
{
  TH2I* pRootSpectrum = reinterpret_cast<TH2I*>(getRootSpectrum());
  pRootSpectrum->fArray = nullptr;         // Prevents multiple deletes of
  
}

//////////////////////////////////////////////////////////////////////////
//
//  Function:   
//    void Increment( const CEvent& rEvent )
//  Operation Type:
//     mutator
//
void 
CSpectrum2DL::Increment(const CEvent& rE) 
{
// Increments channel number rEvent[m_nParameter] >> m_nScaleDifference
// Formal Parameters:
//
//          const CEvent& rEvent:
//               Event which drives the histogramming
//
  CEvent& rEvent((CEvent&)rE);
  CParameterValue& xParam(rEvent[m_nXParameter]);
  CParameterValue& yParam(rEvent[m_nYParameter]);


  if(xParam.isValid()  && // Require the parameters be in event
     yParam.isValid()) {
    getRootSpectrum()->Fill(xParam, yParam);
  }
}
//////////////////////////////////////////////////////////////////////////
//
//  Function:   
//    Boolt_t UsesParameter (UInt_t nId) const
//  Operation Type:
//     Selector
     
//
Bool_t 
CSpectrum2DL::UsesParameter(UInt_t nId) const
{
// Returns nParameter == m_nParameter
//
// Formal Parameters:
//      UInt_t nParameter:
//        The parameter being checked.

  return (m_nXParameter == nId || 
	  m_nYParameter == nId);

}

//////////////////////////////////////////////////////////////////////
//
// Function:
//   ULong_t operator[](const UInt_t* pIndices) const
// Operation:
//   Selector.
//
ULong_t
CSpectrum2DL::operator[](const UInt_t* pIndices) const
{
  // Provides the value of an element of the spectrum.
  // note:  This is not a 'normal' indexing operation in that
  // references are not returned.
  //
  
  Int_t   nx = pIndices[0];
  Int_t   ny = pIndices[1];
  
  const TH1* pRootSpectrum = getRootSpectrum();
  Int_t bin = pRootSpectrum->GetBin(nx + 1, ny + 1);
  return (ULong_t)(pRootSpectrum->GetBinContent(bin));
		      
}
///////////////////////////////////////////////////////////////////////////
//
// Function:
//    virtual void CSpectrum2DL::set(const UInt_t* pIndices, ULong_t nValue)
//
// Operation Type:
//    Mutator.
//
void
CSpectrum2DL::set(const UInt_t* pIndices, ULong_t nValue)
{
  // Provides write access to a channel of the spectrum.
  //
  
  UInt_t   nx = pIndices[0];
  UInt_t   ny = pIndices[1];

  TH1* pRootSpectrum = getRootSpectrum();
  Int_t bin = pRootSpectrum->GetBin(nx + 1, ny + 1);
  pRootSpectrum->SetBinContent(bin, static_cast<Double_t>(nValue));

  
}

///////////////////////////////////////////////////////////////////////////
//
// Function
//   void GetParameterIds(vector<UInt_t>& rvIds)
// Operation type:
//   Selector.
void 
CSpectrum2DL::GetParameterIds(vector<UInt_t>& rvIds)
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
  rvIds.erase(rvIds.begin(), rvIds.end());// Erase the vector.
  rvIds.push_back(m_nXParameter);
  rvIds.push_back(m_nYParameter);
}
////////////////////////////////////////////////////////////////////////
//
// Function:
//    void GetResolutions(vector<UInt_t>&  rvResolutions)
// Operation type:
//    Selector
void
CSpectrum2DL::GetResolutions(vector<UInt_t>&  rvResolutions)
{
  // Returns a vector containing the resolutions of the spectrum.
  // In this case just m_nXScale, m_nYScale
  //
  // Formal Parameters:
  //    vector<UInt_t>&  rvResolutions:
  //          Vector which will contain the resolutions.

  rvResolutions.erase(rvResolutions.begin(), rvResolutions.end());
  rvResolutions.push_back(m_nXScale);
  rvResolutions.push_back(m_nYScale);
}


/*!
   Creates spectrum storage.  This is a common utility function used
   by all constructors.
*/
void
CSpectrum2DL::CreateStorage()
{
  // Just need to allocate storage and pass it to our base class for
  // management:

  setStorageType(keLong);

  Size_t nBytes = StorageNeeded();
  UInt_t*      pStorage = new UInt_t[static_cast<unsigned>(nBytes/sizeof(UInt_t))];

  ReplaceStorage(pStorage);	// Storage now owned by parent.
  Clear();
  createStatArrays(2);
}
/*!
   Create an axis vector for the spectrum constructor.  
   A 2 element axis vector is constructed and returned.
   The X axis description is first, and the Y axis second.
   \param xParam (const CParameter& [in]) Refers to the X axis
     parameter
     description (used to construct the ParameterMapping).
   \param nxChannels (UInt_t [in]) Number of channels on the
     X Axis.
   \param fxLow (Float_t [in]) coordinate in mapped parameter space
       represented by channel 0 on the X axis.
   \param fxHigh (Float_t [in]) coordinate in mapped parameter space
       represented by channel nxChannels-1 on the X axis.
   \param yParam (const CParameter& [in]) Refers to the Y axis
       parameter description.  Used to construct a Parameter Mapping
   \param nyChannels (UInt_t [in]) Number of channels on the Y
       axis.
   \param fyLow (Float_t [in]) coordinate in mapped parameter space
       represented by  channel 0 on the X axis.
   \param fyHigh (Float_t [in]) coordinat in mapped parameter space
       represented by channel nyChannels - 1 on the Y axis.

   \return A CAxes array that has the X axis mappgin as element 0
   and the Y axis mapping as element 1.
*/
CSpectrum::Axes
CSpectrum2DL::CreateAxisVector(const CParameter& xParam,
		       UInt_t      nxChannels,
		       Float_t     fxLow, Float_t fxHigh,
		       const CParameter& yParam,
		       UInt_t      nyChannels,
		       Float_t     fyLow, Float_t fyHigh)
{
  CSpectrum::Axes aMappings;
  CAxis xMap(fxLow, fxHigh, nxChannels, 
	     CParameterMapping(xParam));
  CAxis yMap(fyLow, fyHigh, nyChannels,
	     CParameterMapping(yParam));

  aMappings.push_back(xMap);
  aMappings.push_back(yMap);

  return aMappings;
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
CSpectrum2DL::Dimension(UInt_t n) const
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
/**
 * setStorage
 *   Swap out the current storage for the  spectrum with new storage.
 *   In SpecTcl, SpecTcl manages storage used by histograms.
 *
 *   @param pStorage   - pointer to new storage (m_nXSale * m_nYScale) cells.
 */
void
CSpectrum2DL::setStorage(Address_t pStorage)
{
  TH2I* pRootSpectrum = reinterpret_cast<TH2I*>(getRootSpectrum());
  pRootSpectrum->fN = m_nXScale * m_nYScale;
  pRootSpectrum->fArray = reinterpret_cast<Int_t*>(pStorage);
}
/**
 * StorageNeeded
 *    Compute the storage requirements in bytes of the spectrum.
 *
 *  @return Size_t
 */
Size_t
CSpectrum2DL::StorageNeeded() const
{
  return m_nXScale * m_nYScale * sizeof(Int_t);
}