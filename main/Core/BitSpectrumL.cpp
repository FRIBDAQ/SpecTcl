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
static const char* Copyright = "(C) Copyright Michigan State University 2008, All rights reserved";
//  BitSpectrumL.cpp:
//     Implements the non trivial functions of the CBitSpectrumL class.
//     This class maintains bit mask spectra.  For each bit set in the
//     parameter represented by this spectrum in each event, the corresponding
//     channel  is incremented.  The scale (spectrum size), simply determined
//     how many bits are represented by the spectrum.
// Author:
//   Ron Fox
//   NSCL
//   Michigan State Univesrity
//   East Lansing, MI 48824-1321
//
/*
   Change Log
   $Log$
   Revision 5.3  2006/06/22 17:28:37  ron-fox
   Defect 209 : more 64bit unclean issues.

   Revision 5.2.2.1  2006/06/22 17:11:23  ron-fox
   Defect 209: Some other 64 bit uncleanliness that needed mopping up.

   Revision 5.2  2005/06/03 15:19:22  ron-fox
   Part of breaking off /merging branch to start 3.1 development

  Revision 5.1.2.1  2004/12/21 17:51:23  ron-fox
   Port to gcc 3.x compilers.

   Revision 5.1  2004/11/29 16:56:06  ron-fox
   Begin port to 3.x compilers calling this 3.0

   Revision 4.5.2.1  2004/10/27 12:38:40  ron-fox
   optimize performance of Spectrum1DL histogram increments.  Total
   performance gain was a factor of 2.8.  The 'unusual' modifications
   are documented via comments that indicate they were suggested by profile
   data.

   Revision 4.5  2004/02/03 21:32:57  ron-fox
   Make definitions of spectra from resolutions consistent with those that have ranges.

   Revision 4.4.2.1  2004/02/02 21:47:07  ron-fox
   *** empty log message ***

   Revision 4.4  2003/10/24 14:43:29  ron-fox
   Bounds check parameter ids against the size of
   of the event.

   Revision 4.3  2003/04/03 02:20:15  ron-fox
   Fix mis description of bitmap spectrum channel counts.

   Revision 4.2  2003/04/01 19:52:40  ron-fox
   Support for Real valued parameters and spectra with arbitrary binnings.

*/

#include <config.h>
#include "BitSpectrumL.h"
#include "Parameter.h"
#include "Event.h"
#include "RangeError.h"
#include <histotypes.h>
#include <string>
#include <assert.h>
#include <math.h>
#include "CAxis.h"
#include <TH1I.h>
#include <TDirectory.h>

#ifdef HAVE_STD_NAMESPACE
using namespace std;
#endif

/////////////////////////////////////////////////////////////////////////////

// Constructors:



/*!
  Construct a bit spectrum with longword channels.
  This constructor builds a spectrum that has the 
  specified number of channels.  The axis runs from 
  0-(nChannels-1)
  \param rName (string& [in]) Name of the spectrum.
  \param nId   (UInt_t  [in]) Identifier associated with
                              the spectrum.
  \param rParameter (const CParameter& [in]):
                Description of the parameter to 
		histogram.
  \param nChannels (UInt_t [in]): Number of channels
                in the spectrum.

*/

CBitSpectrumL::CBitSpectrumL(const std::string& rName, 
			     UInt_t nId,
			     const CParameter& rParameter,
			     UInt_t nChannels) :
  CSpectrum(rName, nId, 
	    Axes(1, CAxis(0.0, Float_t(nChannels-1), 
			  nChannels ,
			  CParameterMapping(rParameter)))),
  m_nChannels(nChannels + 2),
  m_nParameter(rParameter.getNumber()),
  m_PDescription(rParameter)
  
{
  AddAxis(nChannels, 0.0, (Float_t)(nChannels)); // bits are unitless.
  
  // Create the corresponding Root spectrum - don't assume anything about root's cd.
  
  std::string olddir = gDirectory->GetPath();
  gDirectory->Cd("/");
  TH1I* pRootSpectrum = new TH1I(
    rName.c_str(), rName.c_str(),
    nChannels, static_cast<Double_t>(0.0), static_cast<Double_t>(nChannels)
  );
  pRootSpectrum->Adopt(0, nullptr);      // delete the root spectrum storage
  setRootSpectrum(pRootSpectrum);        // for the base class
  CreateStorage();                       // This replaces spectrum storage too.
  
  gDirectory->Cd(olddir.c_str());
}
/*! 
  Constructs a bit spectrum that has a cut in axis and
  histograms with long word bins.  The histogram histograms
  the bits set in the scaled parameter in the interval
  from nLow - nHigh.

  \param rName (const string& [in]) Name of the spectrum 
                                    to create.
  \param nId (UInt_t [in]): Id number of the spectrum.
            This is a unique number.
  \param rParameter (CParameter& [in]): Description of
            the parameter to histogram.
  \param nLow (UInt_t [in]): Lowest bit to histogram
            (inclusive).
  \param nHigh (UINt_t [in]): Highst bit to historam
            (inclusive).

   Note that if asked to histogram a bit that is out of range,
   no channel will be incremented for that bit.

*/
CBitSpectrumL::CBitSpectrumL(const std::string& rName, UInt_t nId,
			     const CParameter& rParameter,
			     UInt_t nLow,
			     UInt_t nHigh) :
  CSpectrum(rName, nId,
	    Axes(1, 
		 CAxis((Float_t) nLow,
		       (Float_t) nHigh,
		       (nHigh - nLow) ,
		       CParameterMapping(rParameter)))),
  m_nChannels(nHigh - nLow + 2),
  m_nParameter(rParameter.getNumber()),
  m_PDescription(rParameter)
{
  AddAxis((nHigh - nLow), 
	  (Float_t)nLow, (Float_t)nHigh); // bits are unitless.
  
  // Don't assume the user hasn't changed the directory.  For example
  // TBrowser will Cd when you click on a directory.
  
  std::string olddir = gDirectory->GetPath();
  gDirectory->Cd("/");
  TH1I* pRootSpectrum = new TH1I(
    rName.c_str(), rName.c_str(),
    m_nChannels - 2, static_cast<Double_t>(nLow), static_cast<Double_t>(nHigh)
  );
  pRootSpectrum->Adopt(0, nullptr);
  setRootSpectrum(pRootSpectrum);      // Set base class root spectrump ptr.
  CreateStorage();
  gDirectory->Cd(olddir.c_str());
}

/**
 * destructor
 *   We need to delete the root spectrum but first - need to null out its
 *   storage attribute so that its destruction does not try to kill SpecTcl's
 *   storage.
 */
CBitSpectrumL::~CBitSpectrumL()
{
  TH1I* pRootSpectrum = reinterpret_cast<TH1I*>(getRootSpectrum());
  pRootSpectrum->fArray = nullptr;
  
}

// Functions for class CBitSpectrumL:

///////////////////////////////////////////////////////////////////////
// Function:
//    void Increment(const CEvent& rEvent)
// Operation Type:
//   Functionalizer.
//
void 
CBitSpectrumL::Increment(const CEvent& rE)
{
  // Increment the histogram as dictated by the event.
  // 
  // Formal Paramters:
  //    const CEvent& rEvent:
  //      Refers to the event being histogrammed at this instant.
  //
  CEvent& rEvent((CEvent&)rE);
  if(m_nParameter < rEvent.size()) {
    if(rEvent[m_nParameter].isValid()) {
      UInt_t nParam = rEvent[m_nParameter];
      Double_t bitnum = 0;
      UInt_t bit = 1;
      while(nParam) {
        if (bit & nParam) {
            
            nParam &= ~bit;
            getRootSpectrum()->Fill(bitnum);
        }
        bit = bit << 1;
        bitnum += 1.0;
      }
    }
  }
  
}
///////////////////////////////////////////////////////////////////////////
//
// Function:
//    Bool_t  UsesParameter (UInt_t nId  ) const
//  Operation Type:
//     Selector
//
Bool_t
CBitSpectrumL::UsesParameter (UInt_t nId  ) const
{
  return (m_nParameter == nId);
}
/////////////////////////////////////////////////////////////////////////
//
// Function:
//     ULong_t operator[] (const UInt_t* pIndices) const
// Operation Type:
//     Selector.
//
ULong_t
CBitSpectrumL::operator[](const UInt_t* pIndices) const
{
  // Provides the value of an element of hte spectrum.
  // Note: this is not a 'normal' indexing operator in that it only
  // produces rvalues.  This, however allows a uniform call/return 
  // for all types of spectra.
  //
  
  const TH1*     pRootSpectrum = getRootSpectrum();
  Int_t    n = pIndices[0];
  Int_t   bin = pRootSpectrum->GetBin(n + 1);
  return static_cast<ULong_t>(pRootSpectrum->GetBinContent(bin));

}
///////////////////////////////////////////////////////////////////////
//
// Function:
//     void set(const UInt_t* pIndices, ULong_t nValue)
// Operation Type:
//     Mutator.
//
void
CBitSpectrumL::set(const UInt_t* pIndices, ULong_t nValue)
{
  // Provides write access to a spectrum channel.
  // 
  // Formal Parameters:
  //    const UInt_t* pIndices:
  //        Set of array indices for the spectrum (only [0] is used).
  //    ULong_t nValue:
  //        Value to set.
  //
 
  // Provides write access to a channel of the spectrum.
  //
  
  TH1*    pRootSpectrum = getRootSpectrum();
  Int_t   n = pIndices[0];
  Int_t  bin = pRootSpectrum->GetBin(n + 1);
  pRootSpectrum->SetBinContent(bin, static_cast<Double_t>(nValue));
}
/////////////////////////////////////////////////////////////////////
//
//  Function:
//    void GetParameterIds(vector<UInt_t>& rvIds)
//  Operation Type:
//    Selector.
//
void
CBitSpectrumL::GetParameterIds(vector<UInt_t>& rvIds)
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
CBitSpectrumL::GetResolutions(vector<UInt_t>&  rvResolutions)
{
  // Returns a vector containing the set of spectrum resolutions.
  // In this case it's just the single resolution.
  //
  rvResolutions.erase(rvResolutions.begin(), rvResolutions.end());

  rvResolutions.push_back(m_nChannels);
}

/*!
    Creates the initial spectrum storage.
    this is a protected utility function that is used
    to factor out common code required of all constructors.
*/
void
CBitSpectrumL::CreateStorage()
{  
  setStorageType(keLong);

  Size_t        nBytes   = StorageNeeded();
  UInt_t*      pStorage = new UInt_t[static_cast<unsigned>(nBytes/sizeof(UInt_t))];

  ReplaceStorage(pStorage);	// Storage now owned by parent.
  Clear();

  createStatArrays(1);

}
/**
 * setStorage
 *    Replace the root storage with SpecTcl managed storage.
 * @param pStorage - pointer to SpecTcl managed storage for the spectrum.
 */
void
CBitSpectrumL::setStorage(Address_t pStorage)
{
  CSpectrum::setStorage(pStorage);
  
  TH1I* pRootSpectrum = reinterpret_cast<TH1I*>(getRootSpectrum());
  
  pRootSpectrum->fArray = reinterpret_cast<Int_t*>(pStorage);
  pRootSpectrum->fN     = m_nChannels;
}
/**
 * StorageNeeded
 *    Return the number of bytres of storage  needed by this spectrum.
 *
 *  @return Size_t
 */
Size_t
CBitSpectrumL::StorageNeeded() const
{
  return m_nChannels * sizeof(UInt_t);
}
