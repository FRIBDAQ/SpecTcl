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
// CGamma2DL.cpp
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
    Revision 1.5  2007/03/02 00:38:09  ron-fox
    fix type Int-t -> Uint_t

    Revision 1.4  2007/03/02 00:26:04  ron-fox
    Get allthe ULong_t's right

    Revision 1.3  2007/03/02 00:12:22  ron-fox
    Get the size 64bit clean in Gamma2DD and Gamma2Dl, for some reason
    Gamma2DL did not track when this was fixed for 3.1

    Revision 1.2  2007/02/23 20:38:18  ron-fox
    BZ291 enhancement... add gamma deluxe spectrum type (independent x/y
    parameter lists).

    Revision 1.1  2006/04/17 12:46:26  ron-fox
    Add files missing found by doing a build from checkout rather than
    from tarball.

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
#include "Gamma2DL.h"
#include "Parameter.h"
#include <Exception.h>
#include <RangeError.h>
#include "Event.h"
#include "GateContainer.h"
#include "Gate.h"
#include "CGammaCut.h"
#include "CGammaBand.h"
#include "CGammaContour.h"
#include <assert.h>


#ifdef HAVE_STD_NAMESPACE
using namespace std;
#endif

// Functions for class CGamma2DL

//////////////////////////////////////////////////////////////////////////
//
//  Function:
//   CGammaDW(const std::string& rname, UInt_t nId,
//            vector<CParameter>& rParameters,
//            UInt_t nxScale, UInt_t nYScale)
// Operation Type:
//   Constructor
//

CGamma2DL::CGamma2DL(const std::string& rName, UInt_t nId,
		     vector<CParameter>& rParameters,
		     UInt_t nXScale, UInt_t nYScale) :
  CGammaSpectrum(rName, nId,
	    CreateAxisVector(rParameters,
			     nXScale, nYScale,
			     0.0,    (Float_t)(nXScale ),
			     0.0,    (Float_t)(nYScale)),
		 rParameters),
  m_nXScale(nXScale),
  m_nYScale(nYScale)
{

  // The assumption is all parameters have the same units.

  AddAxis(nXScale, 0.0, (Float_t)(nXScale - 1), rParameters[0].getUnits());
  AddAxis(nYScale, 0.0, (Float_t)(nYScale - 1), rParameters[0].getUnits());

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
CGamma2DL::CGamma2DL(const std::string& rName, UInt_t nId,
		     vector<CParameter>& rParameters,
		     UInt_t nXScale, UInt_t nYScale,
		     Float_t xLow, Float_t xHigh,
		     Float_t yLow, Float_t yHigh) :
  CGammaSpectrum(rName, nId,
	    CreateAxisVector(rParameters, nXScale, nYScale,
			     xLow, xHigh, yLow, yHigh), rParameters),
  m_nXScale(nXScale),
  m_nYScale(nYScale)
{
  // The assumption is all paramters have the same units.
  AddAxis(nXScale, xLow, xHigh, rParameters[0].getUnits());
  AddAxis(nYScale, yLow, yHigh, rParameters[0].getUnits());

  CreateStorage();
}


//////////////////////////////////////////////////////////////////////
//
// Function:
//   ULong_t operator[](const UInt_t* pIndices) const
// Operation:
//   Selector.
//

ULong_t
CGamma2DL::operator[] (const UInt_t* pIndices) const
{
  UInt_t* pStorage = (UInt_t*)getStorage();
  UInt_t nx = pIndices[0];
  UInt_t ny = pIndices[1];
  if (nx >= Dimension(0)) {
    throw CRangeError(0, Dimension(0)-1, nx,
		      std::string("Indexing 2DW gamma spectrum x axis"));
  }
  if (ny >= Dimension(1)) {
    throw CRangeError(0, Dimension(1)-1, ny,
		      std::string("Indexing 2DW gamma spectrum y axis"));
  }
  return (ULong_t)pStorage[nx + (ny * m_nXScale)];
}

///////////////////////////////////////////////////////////////////////////
//
// Function:
//    virtual void CGamma2DL::set(const UInt_t* pIndices, ULong_t nValue)
//
// Operation Type:
//    Mutator.
//

void
CGamma2DL::set (const UInt_t* pIndices, ULong_t nValue)
{
  UInt_t* pStorage = (UInt_t*)getStorage();
  UInt_t nx = pIndices[0];
  UInt_t ny = pIndices[1];
  if (nx >= Dimension(0)) {
    throw CRangeError(0, Dimension(0)-1, nx,
		      std::string("Indexing 2DW gamma spectrum x axis"));
  }
  if (ny >= Dimension(1)) {
    throw CRangeError(0, Dimension(1)-1, ny,
		      std::string("Indexing 2DW gamma spectrum y axis"));
  }
  pStorage[nx + (ny * m_nXScale)] = (UInt_t)nValue;
}




////////////////////////////////////////////////////////////////////////
//
// Function:
//    void GetResolutions(vector<UInt_t>&  rvResolutions)
// Operation type:
//    Selector
//

void
CGamma2DL::GetResolutions (vector<UInt_t>& rvResolutions)
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
CGamma2DL::CreateStorage()
{
  setStorageType(keLong);
  Size_t nBytes = StorageNeeded();
  UInt_t* pStorage = new UInt_t[static_cast<unsigned>(nBytes/sizeof(UInt_t))];

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
CGamma2DL::CreateAxisVector(vector<CParameter>& rParams,
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
CGamma2DL::Dimension(UInt_t n) const
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
CGamma2DL::Increment(vector<pair<UInt_t, Float_t> >& rParameters)
{
  UInt_t* pStorage = (UInt_t*)getStorage();

  if (rParameters.size() > 0) {
    for(int i = 0; i < rParameters.size() - 1; i++) {
      for(int j = i+1; j < rParameters.size(); j++ ) {
	UInt_t  parx = rParameters[i].first;
	Float_t xval = rParameters[i].second;
	
	UInt_t  pary = rParameters[j].first;
	Float_t yval = rParameters[j].second;
	
	// transform -> Spectrum coordinates and increment.
	
  Int_t x = (Int_t)ParameterToAxis(0, xval);
	Int_t y = (Int_t)ParameterToAxis(1, yval);
	bool xok = checkRange(x, m_nXScale, 0);
  bool yok = checkRange(y, m_nYScale, 1);
        
	if (xok && yok) {
	  pStorage[x + y*m_nXScale]++;
	}     }
    }
  }
}

//! Incrementing with 2d deluxe increment is ann error:

void
CGamma2DL::Increment(std::vector<std::pair<UInt_t, Float_t> >& xParameters,
		     std::vector<std::pair<UInt_t, Float_t> >& yParameters)
{
  throw CException("Gamma 2d Deluxe increment called on CGamma2DL");
}
