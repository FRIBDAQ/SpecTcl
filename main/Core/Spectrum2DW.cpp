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
//  CSpectrum2DW.cpp
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
  Change log:
  $Log$
  Revision 5.2  2005/06/03 15:19:24  ron-fox
  Part of breaking off /merging branch to start 3.1 development

  Revision 5.1.2.1  2004/12/21 17:51:25  ron-fox
  Port to gcc 3.x compilers.

  Revision 5.1  2004/11/29 16:56:09  ron-fox
  Begin port to 3.x compilers calling this 3.0

  Revision 4.6.2.1  2004/10/27 15:38:31  ron-fox
  Optimize 2d spectrum increments (well the word increments).

  Revision 4.6  2004/02/03 21:32:58  ron-fox
  Make definitions of spectra from resolutions consistent with those that have ranges.

  Revision 4.5.2.1  2004/02/02 21:47:08  ron-fox
  *** empty log message ***

  Revision 4.5  2003/10/24 14:43:28  ron-fox
  Bounds check parameter ids against the size of
  of the event.

  Revision 4.4  2003/08/25 16:25:32  ron-fox
  Initial starting point for merge with filtering -- this probably does not
  generate a goo spectcl build.

  Revision 4.3  2003/04/01 19:53:46  ron-fox
  Support for Real valued parameters and spectra with arbitrary binnings.

*/

//
// Header Files:
//

#include <config.h>
#include "Spectrum2DW.h"                               
#include "Parameter.h"
#include "RangeError.h"
#include "Event.h"
#include "CAxis.h"
#include "CParameterMapping.h"
#include <TH2S.h>
#include <TDirectory.h>

#ifdef HAVE_STD_NAMESPACE
using namespace std;
#endif
// Functions for class CSpectrum2DW

//////////////////////////////////////////////////////////////////////////
//
//  Function:
//   CSpectrum2DW(const std::string& rname, UInt_t nId,
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
CSpectrum2DW::CSpectrum2DW(const std::string& rName, UInt_t nId,
			   const CParameter& rXParameter, 
			   const CParameter& rYParameter,
			   UInt_t nXScale, UInt_t nYScale) :
  CSpectrum(rName, nId,
	    CreateAxisVector(rXParameter, 
			     nXScale, 0.0, (Float_t)(nXScale-1),
			     rYParameter, 
			     nYScale, 0.0, (Float_t)(nYScale-1))),
  m_nXScale(nXScale + 2),           // For underflow and overflow
  m_nYScale(nYScale + 2),           // root channels we add 2.
  m_nXParameter(rXParameter.getNumber()),
  m_nYParameter(rYParameter.getNumber())

{
  AddAxis(nXScale, 0.0, (Float_t)(nXScale), 
	  rXParameter.getUnits());
  AddAxis(nYScale, 0.0, (Float_t)(nYScale), 
	  rYParameter.getUnits());
  
  // Create the root spectrum, note that CreateStorage will set the histogram's
  // storage to be managed by SpecTcl not root:
  
  std::string olddir = gDirectory->GetPath();
  gDirectory->Cd("/");
  TH2S* pRootSpectrum = new TH2S(
    rName.c_str(), rName.c_str(),
    nXScale, static_cast<Double_t>(0.0), static_cast<Double_t>(nXScale),
    nYScale, static_cast<Double_t>(0.0), static_cast<Double_t>(nYScale)
  );
  pRootSpectrum->Adopt(0, nullptr);       // Free root's storage (see above).
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
CSpectrum2DW:: CSpectrum2DW(const std::string& rName, UInt_t nId,
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
  
  // Create the root spectrum and let its storage be managed by us.
  // The call to CreateStorage establishes our storage:
  
  std::string olddir = gDirectory->GetPath();
  gDirectory->Cd("/");
  TH2S* pRootSpectrum = new TH2S(
    rName.c_str(), rName.c_str(),
    nXChannels, static_cast<Double_t>(fxLow), static_cast<Double_t>(fxHigh),
    nYChannels, static_cast<Double_t>(fyLow), static_cast<Double_t>(fyHigh)
  );
  pRootSpectrum->Adopt(0, nullptr);     // Free root storage for spectrum.
  setRootSpectrum(pRootSpectrum);
  gDirectory->Cd(olddir.c_str());
  
  CreateStorage();
}
/**
 * destructor
 *    We need to replace the storage with a null pointer as deletion
 *    of the actual storage will have been done by SpecTcl
 */
CSpectrum2DW::~CSpectrum2DW()
{
  TH2S* pRootSpectrum = reinterpret_cast<TH2S*>(getRootSpectrum());
  pRootSpectrum->fArray = nullptr;     // Prevents root from deleting actual storage.
}



//////////////////////////////////////////////////////////////////////////
//
//  Function:   
//    void Increment( const CEvent& rEvent )
//  Operation Type:
//     mutator
//
void 
CSpectrum2DW::Increment(const CEvent& rE) 
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
CSpectrum2DW::UsesParameter(UInt_t nId) const
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
CSpectrum2DW::operator[](const UInt_t* pIndices) const
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
//    virtual void CSpectrum2DW::set(const UInt_t* pIndices, ULong_t nValue)
//
// Operation Type:
//    Mutator.
//
void
CSpectrum2DW::set(const UInt_t* pIndices, ULong_t nValue)
{
  // Provides write access to a channel of the spectrum.
  //
  
  Int_t   nx = pIndices[0];
  Int_t   ny = pIndices[1];

  TH1* pRootSpectrum = getRootSpectrum();
  Int_t bin = pRootSpectrum->GetBin(nx + 1 , ny + 1);
  pRootSpectrum->SetBinContent(bin, static_cast<Double_t>(nValue));
  

  
}

///////////////////////////////////////////////////////////////////////////
//
// Function
//   void GetParameterIds(vector<UInt_t>& rvIds)
// Operation type:
//   Selector.
void 
CSpectrum2DW::GetParameterIds(vector<UInt_t>& rvIds)
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
CSpectrum2DW::GetResolutions(vector<UInt_t>&  rvResolutions)
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
CSpectrum2DW::CreateStorage()
{
  // Just need to allocate storage and pass it to our base class for
  // management:

  setStorageType(keWord);

  Size_t nBytes = StorageNeeded();
  UShort_t*      pStorage = new UShort_t[static_cast<unsigned>(nBytes/sizeof(UShort_t))];

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
CSpectrum2DW::CreateAxisVector(const CParameter& xParam,
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
CSpectrum2DW::Dimension(UInt_t n) const
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
 *    Replaces the root spectrum storage with new storage.  This is done when
 *    spectra are sbind'ed and unbind-ed.
 * @param pStorage - New spectrum storage.  The caller is responsible for
 *        copying the spectrum into the new storage at some point
 */
void
CSpectrum2DW::setStorage(Address_t pStorage)
{
  TH2S* pRootSpectrum = reinterpret_cast<TH2S*>(getRootSpectrum());
  pRootSpectrum->fN = m_nXScale * m_nYScale;    // # cells of storage.
  pRootSpectrum->fArray = reinterpret_cast<Short_t*>(pStorage);
}
/**
 * StorageNeeded
 *    Compute the storage requirements in bytes of the spectrum.
 *
 *  @return Size_t
 */
Size_t
CSpectrum2DW::StorageNeeded() const
{
  return m_nXScale * m_nYScale * sizeof(UShort_t);
}
