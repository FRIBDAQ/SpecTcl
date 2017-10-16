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

//  CSpectrum1DW.cpp
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

  Revision 5.1  2004/11/29 16:56:08  ron-fox
  Begin port to 3.x compilers calling this 3.0

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
#include "Spectrum1DW.h"                               
#include "Parameter.h"
#include "RangeError.h"
#include "Event.h"
#include "CAxis.h"
#include <assert.h>
#include <TH1S.h>

#ifdef HAVE_STD_NAMESPACE
using namespace std;
#endif

static const char* Copyright = 
"CSpectrum1DW.cpp: Copyright 1999 NSCL, All rights reserved\n";

// Functions for class CSpectrum1DW

//////////////////////////////////////////////////////////////////////////
//
//  Function:
//   CSpectrum1DW(const std::string& rname, UInt_t nId,
//               const CParameter& rParameter,
//               UInt_t            nScale)
// Operation Type:
//   Constructor
//
/*!
  Construct a 1d Spectrum.  In this constructor, the
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
CSpectrum1DW::CSpectrum1DW(const std::string& rName, 
			  UInt_t            nId,
			  const CParameter& rParameter,
			  UInt_t            nChannels) :
  CSpectrum(rName, nId,
	    Axes(1,
		  CAxis(0.0, (Float_t)(nChannels-1),
		  nChannels,
		  CParameterMapping(rParameter)))),
  m_nChannels(nChannels+2),            // Root adds two channels.
  m_nParameter(rParameter.getNumber()),
  m_pRootSpectrum(0)
{
  AddAxis(nChannels, 0.0, (Float_t)(nChannels), rParameter.getUnits());
  
  // Create the root spectrum.  Then CreateChannels will indirectly
  // set the spectrum's  storage to the storage SpecTcl manages:
  
  m_pRootSpectrum = new TH1S(
    rName.c_str(), rName.c_str(), nChannels, 0.0,
    static_cast<Double_t>(nChannels)
  );
  m_pRootSpectrum->Adopt(0, nullptr);      // Get th1 to delete its storage.
  
  CreateChannels();
}
/*!
    Construct a 1d spectrum.   In this constructor,
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
CSpectrum1DW::CSpectrum1DW(const std::string&  rName,
			   UInt_t              nId,
			   const   CParameter& rParameter,
			   UInt_t              nChannels,
			   Float_t             fLow, 
			   Float_t             fHigh) :
  CSpectrum(rName, nId,
	    Axes(1, CAxis(fLow, fHigh, nChannels,
			   CParameterMapping(rParameter)))),
  m_nChannels(nChannels+2),                  // Root adds two channels.
  m_nParameter(rParameter.getNumber()),
  m_pRootSpectrum(0)
{
  AddAxis(nChannels, fLow, fHigh, rParameter.getUnits());
  
  // See comments in prior constructor about the order of this.
  
  m_pRootSpectrum = new TH1S(
    rName.c_str(), rName.c_str(), nChannels,
    static_cast<Double_t>(fLow), static_cast<Double_t>(fHigh)
  );
  m_pRootSpectrum->Adopt(0, nullptr);
  
  CreateChannels();
}
/**
 * destructor
 */
CSpectrum1DW::~CSpectrum1DW()
{
  m_pRootSpectrum->fArray = nullptr;          // Prevents attempted delete.
  delete m_pRootSpectrum;
}

//////////////////////////////////////////////////////////////////////////
//
//  Function:   
//    void Increment( const CEvent& rEvent )
//  Operation Type:
//     mutator
//
/*!
   Increments the spectrum.  The parameter is mapped
   to axis coordinates and, if the resulting channel
   is in range, it is incremented.

   \param rE  (CEvent& [in]): Event to analyze.

   The parameter id (m_nParameter) determines which
   of the parameters in the rE to use.
*/
void 
CSpectrum1DW::Increment(const CEvent& rE) 
{


  CEvent& rEvent((CEvent&)rE);	// Ok since non const  operator[] on rhs only.

  if(m_nParameter < rEvent.size()) {
    if(rEvent[m_nParameter].isValid()) {  // Only increment if param present.
        m_pRootSpectrum->Fill(rEvent[m_nParameter]);
    }
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
CSpectrum1DW::UsesParameter(UInt_t nId) const
{
// Returns nParameter == m_nParameter
//
// Formal Parameters:
//      UInt_t nParameter:
//        The parameter being checked.

  return (m_nParameter == nId);

}

//////////////////////////////////////////////////////////////////////
//
// Function:
//   ULong_t operator[](const UInt_t* pIndices) const
// Operation:
//   Selector.
//
ULong_t
CSpectrum1DW::operator[](const UInt_t* pIndices) const
{
  
  return static_cast<ULong_t>(m_pRootSpectrum->GetBinContent(pIndices[0]));
		      
}
///////////////////////////////////////////////////////////////////////////
//
// Function:
//    virtual void CSpectrum1DW::set(const UInt_t* pIndices, ULong_t nValue)
//
// Operation Type:
//    Mutator.
//
void
CSpectrum1DW::set(const UInt_t* pIndices, ULong_t nValue)
{
  m_pRootSpectrum->SetBinContent(pIndices[0], static_cast<Double_t>(nValue));
}
/////////////////////////////////////////////////////////////////////
//
//  Function:
//    void GetParameterIds(vector<UInt_t>& rvIds)
//  Operation Type:
//    Selector.
//
void
CSpectrum1DW::GetParameterIds(vector<UInt_t>& rvIds)
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
CSpectrum1DW::GetResolutions(vector<UInt_t>&  rvResolutions)
{
  // Returns a vector containing the set of spectrum resolutions.
  // In this case it's just the single resolution.
  //
  rvResolutions.erase(rvResolutions.begin(), rvResolutions.end());
  rvResolutions.push_back(m_nChannels);
}

/*!
   Create storage for the spectrum.  
*/
void
CSpectrum1DW::CreateChannels()
{
  // Just need to allocate storage and pass it to our base class for
  // management:

  setStorageType(keWord);
  UShort_t* pStorage = new UShort_t[m_nChannels];
  ReplaceStorage(pStorage);	// Storage now owned by parent.
  Clear();
  createStatArrays(1);
}
/**
 * setStorage
 *    Need to set the root storage:
 *
 * @param pStorage - newly assigned spectrum storage.
 */
void
CSpectrum1DW::setStorage(Address_t pStorage)
{
  m_pRootSpectrum->fArray = reinterpret_cast<Short_t*>(pStorage);
  m_pRootSpectrum->fN = m_nChannels;            // Number of cells.
}
/**
 * StorageNeeded
 *    @return number of bytes needed by this spectrum.  Note the constructor
 *            has diddled m_nChannels to account for the extra 2 chans.
 */
Size_t
CSpectrum1DW::StorageNeeded() const
{
  return m_nChannels * sizeof(UShort_t);
}
/**
 * Dimension
 *    Number of channels in a specific dimension:
 *
 *  @param nDim - Which dimension (0 will give m_nChannels but others give 1).
 *  @return Size_t
 */
Size_t
CSpectrum1DW::Dimension(UInt_t nDim) const
{
  if (nDim == 0) return m_nChannels;
  return 1;
}
