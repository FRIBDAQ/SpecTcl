/*
    This software is Copyright by the Board of Trustees of Michigan
                         State University (c) Copyright 2014.
                     
                         You may use this software under the terms of the GNU public license
                         (GPL).  The terms of this license are described at:
                     
                          http://www.gnu.org/licenses/gpl.txt
                     
                          Authors:
                                  Ron Fox
                                  Jeromy Tompkins
                                  NSCL
                                  Michigan State University
                                  East Lansing, MI 48824-1321
*/
//  CSpectrumS.cpp
//    Base Class for a StripChart histogram.  
//
//
//   Author:
//      Tim Hoagland
//      NSCL / WIENER
//      s04.thoagland@wittenberg.edu
//
//   CSpectrumS is based on CSpectrum1DL which
//     was written by:
//        Ron Fox
//        NSCL
//        Michigan State University
//        East Lansing, MI 48824-1321
//        mailto:fox@nscl.msu.edu
//
//////////////////////////.cpp file/////////////////////////////////////////////////////

//
// Header Files:
//
/*
  Change Log:
  
  May  5 2005 - StripChart spectra has been verified to work.  Thus making
                this the first working version of this file. -Tim Hoagland
  May 13 2005 - Fixed an error in GetParameterId in which the function
                returned the parameters in the wrong order -Tim Hoagland


*/
#include <config.h>
#include "SpectrumS.h"                               
#include "Parameter.h"
#include "RangeError.h"
#include "Event.h"
#include "CAxis.h"
#include <assert.h>
#include <math.h>
#include <TH1I.h>
#include <TAxis.h>
#include <TDirectory.h>

#ifdef HAVE_STD_NAMESPACE
using namespace std;
#endif

static const char* Copyright = 
"CSpectrumS.cpp: Copyright 1999 NSCL, All rights reserved\n";

// Note by definition, strip chart spectra never have overflows as they just
// shift channels around as needed to accomodate the new increment.

// Functions for class CSpectrumS

//////////////////////////////////////////////////////////////////////////
//
//  Function:
//   CSpectrumS(const std::string& rname, UInt_t nId,
//               const CParameter& rParameter,
//               UInt_t            nScale)
// Operation Type:
//   Constructor
//
/*!
  Construct a StripChart.  In this constructor, the
  axis is assumed to go from [0,nChannels).  The axis
  channels are the axis coordinates as well (unit 
  mapping).  The parameter may, however be mapped or
  unmapped.
  \param rName      (const string& [in])
       The name of the spectrum.
  \param nId        (UInt_t [in])
       The spectrum id.  need not be unique.
  \param rParameter (const CParameter& [in])
       The discription of the parameter to histogram.
  \param nChannels   (UInt_t [in])
       The number of channels to allocate to the
       spectrum.  In this constructor, the axis 
       coordinates are [0,nChannels).

 */
CSpectrumS::CSpectrumS(const std::string&     rName, 
			  UInt_t              nId,
			  const CParameter&   rParameter,
		    const CParameter&   nChannel,
			  UInt_t              nChannels):
  CSpectrum(rName, nId,
	    Axes(1,
		  CAxis((Float_t)0.0, (Float_t)(nChannels),
			nChannels,
			CParameterMapping(rParameter)))),
  m_nChannels(nChannels + 2),
  m_nChannel(nChannel.getNumber()),
  m_nParameter(rParameter.getNumber()),
  m_nOffset(0)
 
{
  AddAxis(nChannels, 0.0, (Float_t)(nChannels), rParameter.getUnits());
std::string olddir = gDirectory->GetPath();
  gDirectory->Cd("/");
  TH1I* pRootSpectrum = new TH1I(
    rName.c_str(), rName.c_str(),
    nChannels, static_cast<Double_t>(0.0), static_cast<Double_t>(nChannels)
  );
  pRootSpectrum->Adopt(0, nullptr);
  setRootSpectrum(pRootSpectrum);
  gDirectory->Cd(olddir.c_str());
  
  CreateChannels();
}
/*!
    Construct a StripChart.   In this constructor,
    the axis is assumed to go in the range [fLow, fHigh]
    and have nChannels bins.  The parameter may be mapped
    or unmapped.
  \param rName      (const string& [in])
       The name of the spectrum.
  \param nId        (UInt_t [in])
       The spectrum id.  need not be unique.
  \param rParameter (const CParameter& [in])
       The discription of the parameter to histogram.
  \param nChannels   (UInt_t [in])
       The number of channels to allocate to the
       spectrum.  In this constructor, the axis 
       coordinates are [0,nChannels).
  \param fLow (Float_t [in]):
       The low limit of the axis.
  \param fHigh  (Float_t [in]):
        The high limit of the axis.
*/
CSpectrumS::CSpectrumS(const std::string&        rName,
			   UInt_t                nId,
			   const CParameter&     rParameter,
		           const CParameter&     nchannel,
			   UInt_t                nChannels,
			   Float_t               fLow, 
			   Float_t               fHigh) :
    CSpectrum(rName, nId,
	    Axes(1, CAxis(fLow, fHigh, nChannels,
			  CParameterMapping(rParameter)))),
  m_nChannels(nChannels + 2),
  m_nChannel(nchannel.getNumber()),
  m_nParameter(rParameter.getNumber()),
  m_nOffset(0)
{
  AddAxis(nChannels, fLow, fHigh, rParameter.getUnits());
  std::string olddir = gDirectory->GetPath();
  gDirectory->Cd("/");
  TH1I* pRootSpectrum = new TH1I(
    rName.c_str(), rName.c_str(),
    nChannels, static_cast<Double_t>(fLow), static_cast<Double_t>(fHigh)
  );
  pRootSpectrum->Adopt(0, nullptr);
  setRootSpectrum(pRootSpectrum);
  gDirectory->Cd(olddir.c_str());
  
  CreateChannels();
  
}
/**
 * destructor
 */
CSpectrumS::~CSpectrumS()
{
  TH1I* pRootSpectrum = reinterpret_cast<TH1I*>(getRootSpectrum());
  pRootSpectrum->fArray = nullptr;

}


/**
 *   Increment
 * @param rE - the event to use to increment the spectrum.
 */
void 
CSpectrumS::Increment(const CEvent& rE) 
{


  CEvent& rEvent((CEvent&)rE);	// Ok since non const  operator[] on rhs only.
  CParameterValue& rTime(rEvent[m_nChannel]);
  CParameterValue& rParam(rEvent[m_nParameter]);
  TH1* pRootSpectrum  = getRootSpectrum();

  if(rTime.isValid() && rParam.isValid()) {  // Only increment if params present.
    TAxis* axis       = pRootSpectrum->GetXaxis();
    
    Double_t  xmin  = axis->GetXmin();
    Double_t  xmax  = axis->GetXmax();
    Double_t  range = (xmax - xmin);

    Double_t time   = rTime;
    Double_t counts = rParam;
    Int_t bin       = pRootSpectrum->FindBin(rTime);
    Int_t effectiveBin = (time - xmin)*(m_nChannels-1)/(range) + 2;  // Bin if infinite spectrum.
    Int_t overflowBin = m_nChannels - 1;
    
    // See if we need to shift the spectrum and reset the axis limits.

    if (bin == overflowBin ) {
      int shift = static_cast<int>(effectiveBin + (.25 * m_nChannels) - m_nChannels);
      ShiftDataDown(shift);                // Moves the data down
      
      // Compute and set the new values for the Axis limits/
      
      xmin = xmin + 0.25*range;
      xmax = xmax + 0.25*range;
      axis->Set(m_nChannels - 2, xmin, xmax);
      
      
    }else if (bin == 0) {            ///  Underflow bin.
      
      ShiftDataUp(effectiveBin);
      
      xmin = time;
      xmax = xmin + range;
      axis->Set(m_nChannels - 2, xmin, xmax);
    }
    pRootSpectrum->Fill(time, counts);
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
CSpectrumS::UsesParameter(UInt_t nId) const
{
// Returns nParameter == m_nParameter
//
// Formal Parameters:
//      UInt_t nParameter:
//        The parameter being checked.

  return (m_nParameter == nId||
	  m_nChannel == nId);

}

//////////////////////////////////////////////////////////////////////
//
// Function:
//   ULong_t operator[](const UInt_t* pIndices) const
// Operation:
//   Selector.
//
ULong_t
CSpectrumS::operator[](const UInt_t* pIndices) const
{
  // Provides the value of an element of the spectrum.
  // note:  This is not a 'normal' indexing operation in that
  // references are not returned.
  //
  const TH1* pRootSpectrum = getRootSpectrum();
  Int_t   n = pIndices[0];
  if(n + 2 >= Dimension(0)) {
    throw CRangeError(0, Dimension(0)-1, n,
		      std::string("Indexing StripChart"));
  }
  
  return (ULong_t)pRootSpectrum->GetBinContent(
    pRootSpectrum->GetBin(n + 1)
  );
		      
}
///////////////////////////////////////////////////////////////////////////
//
// Function:
//    virtual void CSpectrumS::set(const UInt_t* pIndices, ULong_t nValue)
//
// Operation Type:
//    Mutator.
//
void
CSpectrumS::set(const UInt_t* pIndices, ULong_t nValue)
{
  // Provides write access to a channel of the spectrum.
  //
  UInt_t* p = (UInt_t*)getStorage();
  UInt_t   n = pIndices[0];
  if(n >= Dimension(0)) {
    throw CRangeError(0, Dimension(0)-1, n,
		      std::string("Indexing StripChart"));
  }
  TH1* pRootSpectrum = getRootSpectrum();
  pRootSpectrum->SetBinContent(
    pRootSpectrum->GetBin(n+1), static_cast<Double_t>(nValue)
  );

  
}
/////////////////////////////////////////////////////////////////////
//
//  Function:
//    void GetParameterIds(vector<UInt_t>& rvIds)
//  Operation Type:
//    Selector.
//
void
CSpectrumS::GetParameterIds(vector<UInt_t>& rvIds)
{
  // Returns a vector containing the set of parameter Ids which are
  // used to increment this spectrum.  In our case, there's just one
  // id, the single parameter histogrammed.
  // 
  // Formal Parameters:
  //    vector<UInt_t>& rvIds:
  //       Refers to the vector to be returned.
  //
  rvIds.erase(rvIds.begin(), rvIds.end());// Clear the vector.
  rvIds.push_back(m_nChannel);
  rvIds.push_back(m_nParameter);
}
///////////////////////////////////////////////////////////////////////
//
//
// Function:
//    void GetResolutions(vector<UInt_t>&  rvResolutions)

// Operation type:
//    Selector
void
CSpectrumS::GetResolutions(vector<UInt_t>&  rvResolutions)
{
  // Returns a vector containing the set of spectrum resolutions.
  // In this case it's just the single resolution.
  //
  rvResolutions.clear();
  rvResolutions.push_back(m_nChannels);
  
}

/*!
   Create storage for the spectrum.  
*/
void
CSpectrumS::CreateChannels()
{
  // Just need to allocate storage and pass it to our base class for
  // management:

  setStorageType(keLong);
  UInt_t* pStorage = new UInt_t[m_nChannels];
  ReplaceStorage(pStorage);	// Storage now owned by parent.
  Clear();
}


/*!
  Is called when the Strip chart data goes off the rightt end of 
  the time axis.  All data is shifted to the left by nChannel's
  If the shift is greater than the spectrum size the spectrum is just cleared.

*/

void
CSpectrumS::ShiftDataDown(int64_t nShift) 
{
  if (nShift >= m_nChannels) {
    Clear();
    return;
  }
  TH1* pRootSpectrum = getRootSpectrum();
  for (int i = 1; i <= m_nChannels-nShift; i++) {
    pRootSpectrum->SetBinContent(i, pRootSpectrum->GetBinContent(i+nShift));
  }
  for (int i =  m_nChannels-nShift+1; i <= m_nChannels; i++) {
    pRootSpectrum->SetBinContent(i, 0.0);
  }
}


void 
CSpectrumS::ShiftDataUp(int64_t nShift)
{
    
    if (m_nChannels <=  (-nShift)) {
      Clear();
      return;

    }
    TH1* pRootSpectrum =  getRootSpectrum();
    for (int i =  m_nChannels ; i > (-nShift); i--) {
      pRootSpectrum->SetBinContent(i, pRootSpectrum->GetBinContent(i+nShift));
    }
    for (int i = (-nShift ) -1 ; i > 0 ; i--) {
      pRootSpectrum->SetBinContent(i, 0.0);
    }
}
/**
 * setStorage
 *    Set storage for the spectrum.
 *
 *  @param pStorage - pointer to new spectrum storage.
 */
void
CSpectrumS::setStorage(Address_t pStorage)
{
  TH1I* pRootSpectrum = reinterpret_cast<TH1I*>(getRootSpectrum());
  pRootSpectrum->fArray = reinterpret_cast<Int_t*>(pStorage);
  pRootSpectrum->fN    = m_nChannels;
}
/*
 *  StorageNeeded
 *
 *  @return Size_t - number of bytes of storage required by this spectrum.
 */
Size_t
CSpectrumS::StorageNeeded() const
{
  return m_nChannels * sizeof(Int_t);
}
