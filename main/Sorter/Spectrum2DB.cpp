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
//  CSpectrum2DB.cpp
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

  Revision 4.5  2004/02/03 21:32:58  ron-fox
  Make definitions of spectra from resolutions consistent with those that have ranges.

  Revision 4.4.2.1  2004/02/02 21:47:08  ron-fox
  *** empty log message ***

  Revision 4.4  2003/10/24 14:43:28  ron-fox
  Bounds check parameter ids against the size of
  of the event.

  Revision 4.3  2003/04/01 19:53:46  ron-fox
  Support for Real valued parameters and spectra with arbitrary binnings.

*/

//
// Header Files:
//

#include <config.h>
#include "Spectrum2DB.h"                               
#include "Parameter.h"
#include "RangeError.h"
#include "Event.h"
#include "CAxis.h"
#include "CParameterMapping.h"


#ifdef HAVE_STD_NAMESPACE
using namespace std;
#endif
// Functions for class CSpectrum2DB

//////////////////////////////////////////////////////////////////////////
//
//  Function:
//   CSpectrum2DB(const std::string& rname, UInt_t nId,
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
CSpectrum2DB::CSpectrum2DB(const std::string& rName, UInt_t nId,
			   const CParameter& rXParameter, 
			   const CParameter& rYParameter,
			    UInt_t nXScale, UInt_t nYScale) :
  CSpectrum(rName, nId,
	    CreateAxisVector(rXParameter, 
			     nXScale, 0.0, (Float_t)(nXScale-1),
			     rYParameter, 
			     nYScale, 0.0, (Float_t)(nYScale-1))),
  m_nXScale(nXScale),
  m_nYScale(nYScale),
  m_nXParameter(rXParameter.getNumber()),
  m_nYParameter(rYParameter.getNumber())

{
  AddAxis(nXScale, 0.0, (Float_t)(nXScale -1), rXParameter.getUnits());
  AddAxis(nYScale, 0.0, (Float_t)(nYScale -1), rYParameter.getUnits());
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
CSpectrum2DB:: CSpectrum2DB(const std::string& rName, UInt_t nId,
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
  m_nXScale(nXChannels),
  m_nYScale(nYChannels),
  m_nXParameter(rXParameter.getNumber()),
  m_nYParameter(rYParameter.getNumber())
  
{
  AddAxis(nXChannels, fxLow, fxHigh, rXParameter.getUnits());
  AddAxis(nYChannels, fyLow, fyHigh, rYParameter.getUnits());
  CreateStorage();
}
  

//////////////////////////////////////////////////////////////////////////
//
//  Function:
//   CSpectrum2DB(const std::string& rname, UInt_t nId,
//               const CParameter& rXParameter, const CParameter rYParmeter)
// Operation Type:
//   Constructor
// Comments:
//   This constructor exists for use by derived classes that want
//   to allocate their own storage.
//
//CSpectrum2DB::CSpectrum2DB(const std::string& rName, UInt_t nId,
//		   const CParameter& rXParameter, 
//		   const CParameter& rYParameter) :
//  CSpectrum(rName, nId),
//  m_nXScale(0),
//m_nYScale(0),
//m_nXParameter(rXParameter.getNumber()),
//m_nYParameter(rYParameter.getNumber()),
//m_nXScaleDifference(0),
//m_nYScaleDifference(0)
//{
//  setStorageType(keByte);
//}

//////////////////////////////////////////////////////////////////////////
//
//  Function:   
//    void Increment( const CEvent& rEvent )
//  Operation Type:
//     mutator
//
void 
CSpectrum2DB::Increment(const CEvent& rE) 
{
// Increments channel number rEvent[m_nParameter] >> m_nScaleDifference
// Formal Parameters:
//
//          const CEvent& rEvent:
//               Event which drives the histogramming
//
  CEvent& rEvent((CEvent&)rE);
  int nParams = rEvent.size();
  if((m_nXParameter < nParams) && (m_nYParameter < nParams)) {
    if(rEvent[m_nXParameter].isValid()  && // Require the parameters be in event
       rEvent[m_nYParameter].isValid()) {
      Int_t nx = Randomize(ParameterToAxis(0, rEvent[m_nXParameter]));
      Int_t ny = Randomize(ParameterToAxis(1, rEvent[m_nYParameter]));
      if( (nx >= 0)   && (nx < m_nXScale)     &&
	  (ny >= 0)   && (ny < m_nYScale)) {
	
	UChar_t* pSpec = (UChar_t*)getStorage();
	pSpec[nx + (ny * m_nXScale)]++;
      }
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
CSpectrum2DB::UsesParameter(UInt_t nId) const
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
CSpectrum2DB::operator[](const UInt_t* pIndices) const
{
  // Provides the value of an element of the spectrum.
  // note:  This is not a 'normal' indexing operation in that
  // references are not returned.
  //
  UChar_t* p = (UChar_t*)getStorage();
  UInt_t   nx = pIndices[0];
  UInt_t   ny = pIndices[1];
  if(nx >= Dimension(0)) {
    throw CRangeError(0, Dimension(0)-1, nx,
		      std::string("Indexing 2DW spectrum x axis"));
  }
  if(ny >= Dimension(1)) {
    throw CRangeError(0, Dimension(1)-1, ny,
		      std::string("Indexing 2DW spectrum y axis"));
  }
  return (ULong_t)p[nx + (ny * m_nXScale)];
		      
}
///////////////////////////////////////////////////////////////////////////
//
// Function:
//    virtual void CSpectrum2DB::set(const UInt_t* pIndices, ULong_t nValue)
//
// Operation Type:
//    Mutator.
//
void
CSpectrum2DB::set(const UInt_t* pIndices, ULong_t nValue)
{
  // Provides write access to a channel of the spectrum.
  //
  UChar_t* p = (UChar_t*)getStorage();
  UInt_t   nx = pIndices[0];
  UInt_t   ny = pIndices[1];
  if(nx >= Dimension(0)) {
    throw CRangeError(0, Dimension(0)-1, nx,
		      std::string("Indexing 2DB spectrum x axis"));
  }
  if(ny >= Dimension(1)) {
    throw CRangeError(0, Dimension(1)-1, ny,
		      std::string("Indexing 2DB spectrum y axis"));
  }
  p[nx + (ny * m_nXScale)] = (UInt_t)nValue;

  
}

///////////////////////////////////////////////////////////////////////////
//
// Function
//   void GetParameterIds(vector<UInt_t>& rvIds)
// Operation type:
//   Selector.
void 
CSpectrum2DB::GetParameterIds(vector<UInt_t>& rvIds)
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
CSpectrum2DB::GetResolutions(vector<UInt_t>&  rvResolutions)
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
CSpectrum2DB::CreateStorage()
{
  // Just need to allocate storage and pass it to our base class for
  // management:

  setStorageType(keByte);

  Size_t nBytes = StorageNeeded();
  UChar_t*      pStorage = new UChar_t[nBytes/sizeof(UChar_t)];

  ReplaceStorage(pStorage);	// Storage now owned by parent.
  Clear();
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
CSpectrum2DB::CreateAxisVector(const CParameter& xParam,
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
UInt_t 
CSpectrum2DB::Dimension(UInt_t n) const
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
