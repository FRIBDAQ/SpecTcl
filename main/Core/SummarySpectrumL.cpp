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
//  CSummarySpectrumW.cpp
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
   Revision 1.3  2007/05/11 20:51:57  ron-fox
   Make NSCLAsciiSpectrumFormatter correctly deal with rev 2, 3 and
   'malforme3d 2' produced by version 3.2-pre2 for summary spectra.

   Revision 1.2  2006/06/22 17:28:37  ron-fox
   Defect 209 : more 64bit unclean issues.

   Revision 1.1.2.1  2006/06/22 17:11:23  ron-fox
   Defect 209: Some other 64 bit uncleanliness that needed mopping up.

   Revision 1.1  2006/04/17 12:46:26  ron-fox
   Add files missing found by doing a build from checkout rather than
   from tarball.

   Revision 5.2  2005/06/03 15:19:24  ron-fox
   Part of breaking off /merging branch to start 3.1 development

   Revision 5.1.2.1  2004/12/21 17:51:26  ron-fox
   Port to gcc 3.x compilers.

   Revision 5.1  2004/11/29 16:56:09  ron-fox
   Begin port to 3.x compilers calling this 3.0

   Revision 4.7  2004/02/03 21:32:58  ron-fox
   Make definitions of spectra from resolutions consistent with those that have ranges.

   Revision 4.6.2.1  2004/02/02 21:47:08  ron-fox
   *** empty log message ***

   Revision 4.6  2003/11/07 21:48:30  ron-fox
   Fix error in CSummarySpectrmB.cpp' s
   increment.  It would sometimes not increment
   when it should

   Revision 4.5  2003/10/24 14:43:29  ron-fox
   Bounds check parameter ids against the size of
   of the event.

   Revision 4.4  2003/04/19 00:11:13  ron-fox
   Fix a nasty issue with GetDefinition() that was causing death due to a number of problems with the static output struct.  For later: change the struct to a class so that it can be returned by value rather than by reference.. then it wouldn't have to be static.

   Revision 4.3  2003/04/03 02:21:12  ron-fox
   *** empty log message ***

   Revision 4.2  2003/04/01 19:53:46  ron-fox
   Support for Real valued parameters and spectra with arbitrary binnings.

*/

//
// Header Files:
//

#include <config.h>
#include "SummarySpectrumL.h"                               
#include "Parameter.h"
#include "RangeError.h"
#include "Event.h"
#include <algorithm>
#include <assert.h>
#include <TH2I.h>

#ifdef HAVE_STD_NAMESPACE
using namespace std;
#endif

// Functions for class CSummarySpectrumL

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
CSummarySpectrumL::CSummarySpectrumL(const std::string& rName, 
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
	  (Float_t)(rrParameters.size())); // Unit-less.
  AddAxis(nYScale, 0.0, (Float_t)(nYScale), 
	  rrParameters[0].getUnits());
  FillParameterArray(rrParameters);
  
  m_pRootSpectrum = new TH2I(
    rName.c_str(), rName.c_str(),
    rrParameters.size(), 0.0, static_cast<Double_t>(rrParameters.size()),
    nYScale, 0.0, static_cast<Double_t>(nYScale)
  );
  m_pRootSpectrum->Adopt(0, nullptr);       // SpecTcl will manage storage.
  CreateStorage();

}
/*!
   Creat a summary spectrum with a y axis that represents an 
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
CSummarySpectrumL::CSummarySpectrumL(const std::string& rName, 
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
	  (Float_t)(rrParameters.size() - 1)); //  Unit-less
  AddAxis(nYScale, fYLow, fYHigh, rrParameters[0].getUnits());
  FillParameterArray(rrParameters);
  
  m_pRootSpectrum  = new TH2I(
    rName.c_str(), rName.c_str(),
    rrParameters.size(), 0.0, static_cast<Double_t>(rrParameters.size()),
    nYScale, static_cast<Double_t>(fYLow), static_cast<Double_t>(fYHigh)
  );
  m_pRootSpectrum->Adopt(0, nullptr);              // SpecTcl manages storage.
  CreateStorage();

}
/**
 * Destructor:
 */
CSummarySpectrumL::~CSummarySpectrumL()
{
  m_pRootSpectrum->fArray = nullptr;
  delete m_pRootSpectrum;
}
//////////////////////////////////////////////////////////////////////////
//
//  Function:   
//    void Increment ( const CEvent& rE )
//  Operation Type:
//     mutator
//
void 
CSummarySpectrumL::Increment(const CEvent& rE) 
{
// Increments channel number rEvent[m_nParameter] >> m_nScaleDifference
// Formal Parameters:
//
//          const CEvent& rEvent:
//               Event which drives the histogramming
//
  CEvent&   rEvent((CEvent&)rE);
  UInt_t    nYChans  = m_nYScale;
  UInt_t* pStorage = (UInt_t*)getStorage();
  int       nParams  = rEvent.size();
  for(UInt_t xChan = 0; xChan < m_vParameters.size(); xChan++) {
    if(m_vParameters[xChan] < nParams) {
      if(rEvent[m_vParameters[xChan]].isValid()) {
        Double_t rawParam = rEvent[m_vParameters[xChan]];
        m_pRootSpectrum->Fill(static_cast<Double_t>(xChan), rawParam);
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
CSummarySpectrumL::UsesParameter(UInt_t nId) const
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
CSummarySpectrumL::operator[](const UInt_t* pIndices) const
{
  // Provides the value of an element of the spectrum.
  // note:  This is not a 'normal' indexing operation in that
  // references are not returned.
  //
  UInt_t* p = (UInt_t*)getStorage();
  Int_t   nx = pIndices[0] + 1;
  Int_t   ny = pIndices[1] + 1;
  if(nx + 1 >= Dimension(0)) {
    throw CRangeError(0, Dimension(0)-1, nx,
		      std::string("Indexing SummaryW spectrum x axis"));
  }
  if(ny + 1 >= Dimension(1)) {
    throw CRangeError(0, Dimension(1)-1, ny,
		      std::string("Indexing SummaryW spectrum y axis"));
  }
  
  return static_cast<ULong_t>(
    m_pRootSpectrum->GetBinContent(m_pRootSpectrum->GetBin(nx, ny))
  );
		      
}
///////////////////////////////////////////////////////////////////////////
//
// Function:
//    virtual void CSummarySpectrumL::set(const UInt_t* pIndices, ULong_t nValue)
//
// Operation Type:
//    Mutator.
//
void
CSummarySpectrumL::set(const UInt_t* pIndices, ULong_t nValue)
{
  // Provides write access to a channel of the spectrum.
  //
  UInt_t* p = (UInt_t*)getStorage();
  UInt_t   nx = pIndices[0];
  UInt_t   ny = pIndices[1];
  if(nx +2 >= Dimension(0)) {
    throw CRangeError(0, Dimension(0)-1, nx,
		      std::string("Indexing 2DW spectrum x axis"));
  }
  if(ny +2 >= Dimension(1)) {
    throw CRangeError(0, Dimension(1)-1, ny,
		      std::string("Indexing 2DW spectrum y axis"));
  }
  m_pRootSpectrum->SetBinContent(
    m_pRootSpectrum->GetBin(nx+1, ny+1), static_cast<Double_t>(nValue)
  );
}

///////////////////////////////////////////////////////////////////////////
//
// Function
//   void GetParameterIds(vector<UInt_t>& rvIds)
// Operation type:
//   Selector.
void 
CSummarySpectrumL::GetParameterIds(vector<UInt_t>& rvIds)
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
CSummarySpectrumL::GetResolutions(vector<UInt_t>&  rvResolutions)
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
   \param Parameters (vector<CParam> [in]):  The set of parameters
      whose ids we need.
*/
void
CSummarySpectrumL::FillParameterArray(vector<CParameter> Params)
{
  // Fill in the parameter and scale difference vector members:

  for(UInt_t i = 0; i < Params.size(); i++) {
    CParameter& rParam(Params[i]);
    m_vParameters.push_back(rParam.getNumber());
  }
}
/*!
  Allocate the storage required by the spectrum and 
  make it self owned.
*/
void
CSummarySpectrumL::CreateStorage()
{

  setStorageType(keLong);

  UInt_t        nBytes   = StorageNeeded();
  UInt_t*      pStorage = new UInt_t[nBytes/sizeof(UInt_t)];

  ReplaceStorage(pStorage);	// Storage now owned by parent.
  Clear();
  createStatArrays(1);          // only y axis overflows.
}
/*!
   Overrides the base class spectrum definition fetching 
   function.  The base class function is almost correct, however
   our x axis is implicitly defined.  Therefore, we use the base
   class and then remove the x axis definitions.. leaving only the
   y axis (which is explicitly defined).

   \retval CSpectrum::SpectrumDefinition
      A structure that describes the spectrum sufficiently to 
      remanufature it if needed.
*/
CSpectrum::SpectrumDefinition&
CSummarySpectrumL::GetDefinition()
{
  // Get the base class's idea of our definition.

  static CSpectrum::SpectrumDefinition Def;
  Def = (CSpectrum::GetDefinition());

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
CSummarySpectrumL::CreateAxes(vector<CParameter> Parameters,
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
  Don't actually need a parameter.
*/
Bool_t
CSummarySpectrumL::needParameter() const
{
  return kfFALSE;
}
/**
 * setStorage
 *    Replace root storage with our storage.  Since we're managing
 *    storage we don't let Root delete the current spectrum's storage.
 *    We did that once at construction time.
 *  @param pStorage - pointer to the new spectrum storage.
 */
void
CSummarySpectrumL::setStorage(Address_t pStorage)
{
  m_pRootSpectrum->fArray = reinterpret_cast<Int_t*>(pStorage);
  m_pRootSpectrum->fN     = Dimension(0) * Dimension(1);
}
/**
 * StorageNeeded
 *    @return size_t - bytes of spectrum storage required.
 */
Size_t
CSummarySpectrumL::StorageNeeded() const
{
  
  return static_cast<Size_t>(Dimension(0) * Dimension(1) * sizeof(Int_t));
}
/**
 *  Dimension
 *     Returns the number of cells on an axis of the spectrum.
 *  @param axis - 0 for x axis (number of params + 2) 1 for y axis.
 */
Size_t
CSummarySpectrumL::Dimension(UInt_t axis) const
{
  if (axis == 0) {
    return m_nXChannels;
  } else if (axis == 1) {
    return m_nYScale;
  } else {
    throw CRangeError(
      0, 1, axis, std::string("requesting dimension of an axis")
    );
  }
}