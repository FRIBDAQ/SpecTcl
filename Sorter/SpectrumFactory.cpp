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
//  CSpectrumFactory.cpp
// CSpectrumFactory is used by the CSpectrumPackage
// create spectra given descriptions.
// Most callers will invoke the CreateSpectrum
// member to create a spectrum.  The other spectrum
// creationals are called by that function, however they are
// available if you know what you need.
//
//  Major changes:
//     - Support creation of spectra with invalid parameters if exceptions
//       are turned off.  This is to allow a spectrum to be built for a
//       file read-in which may not match the set of parameters defined
//       at the time the data is read in.
//
//   Author:
//      Ron Fox
//      NSCL
//      Michigan State University
//      East Lansing, MI 48824-1321
//      mailto:fox@nscl.msu.edu
//
//////////////////////////.cpp file/////////////////////////////////////////////////////

/*!
  Change log:
    $Log$
    Revision 5.4  2006/06/28 20:00:31  ron-fox
    Fix g1 with empty parameter list crash (defect 211) and issues with gui
    when user makes both a and a.b

    Revision 5.3.2.1  2006/06/28 19:32:08  ron-fox
    Defect 211 - g1 spectrum creation with empty parameter list segfaults.
    Should return error .. insufficient parameters.

    Revision 5.3  2005/09/22 12:40:00  ron-fox
    Fix defects in gamma 2d spectrum increment.  When there are no valid
    parameters the outer loop limits are bad and eventually lead to segflt

    Revision 5.2  2005/06/03 15:19:24  ron-fox
    Part of breaking off /merging branch to start 3.1 development

    Revision 5.1.2.2  2005/05/11 16:54:54  thoagland
    Add Support for Stripchart Spectra


    2005/05/05 Tim Hoagland
    Added Support for StripChart Spectra

    Revision 5.1.2.1  2004/12/21 17:51:25  ron-fox
    Port to gcc 3.x compilers.

    Revision 5.1  2004/11/29 16:56:09  ron-fox
    Begin port to 3.x compilers calling this 3.0

    Revision 4.3  2004/02/03 21:32:58  ron-fox
    Make definitions of spectra from resolutions consistent with those that have ranges.

    Revision 4.2.2.1  2004/02/02 21:47:08  ron-fox
    *** empty log message ***

    Revision 4.2  2003/04/01 19:53:46  ron-fox
    Support for Real valued parameters and spectra with arbitrary binnings.

*/

//
// Header Files:
//

#include <config.h>
#include "SpectrumFactory.h"
#include "SpectrumFactoryException.h"
#include "DictionaryException.h"
#include "Spectrum.h"
#include "Spectrum1DL.h"
#include "Spectrum1DW.h"
#include "Spectrum2DW.h"
#include "Spectrum2DB.h"
#include "Spectrum2DL.h"
#include "BitSpectrumL.h"
#include "BitSpectrumW.h"
#include "SummarySpectrumL.h"
#include "SummarySpectrumW.h"
#include "SummarySpectrumB.h"
#include "Gamma1DW.h"
#include "Gamma1DL.h"
#include "Gamma2DB.h"
#include "Gamma2DW.h"
#include "Gamma2DL.h"
#include "SpectrumS.h"

#include "Histogrammer.h"

#ifdef HAVE_STD_NAMESPACE
using namespace std;
#endif

// Assumptions:
//  There are the following global variables which get me to the 
//  stuff I need:
//

extern CEventSink* gpEventSink;	// Current event sink which must really be a
				// histogrammer.

static CParameter NullParameter;

// Static class attributes:

UInt_t CSpectrumFactory::m_nNextId = 0;

// Functions for class CSpectrumFactory


/*!
 Creates a spectrum and returns a pointer to it.


   \param   <tt> rName (const std::string& [in]): </tt>
         Name of the spectrum to create.
   \param   <tt>eType (SpectrumType_t [in]):    </tt>
          Spectrum type which is one of:
          - kOneD  One dimensional spectrum 
	  - kTwoD  Two dimensional spectrum.
	  - kBit   Bit mask spectrum
	  - kSummary Summary sepctrum.
	  - kGOneD   One-d Gamma spectrum.
	  - kGTwoD   Two-d Gamma spectrum.
   \param <TT> rParameters (vector<string>& [in]): </TT>
        Names of the parameters that are involved with the
	spectrum.
   \param <tt>eDataType (DataType_t [in]):</tt>
         Type of data spectrum holds.
   \param  <tt>rParameters (vector<string>& [in]):</tt>
         Set of parameters being histogrammed.
   \param <tt>rChannels (vector<UInt_t>& [in]) </tt>
          Number of channels on each axis of the spectrum.
   \param <tt> pLows (vector<Float_t>* [in] = kpNULL) </tt>
         Vector of axis low limit values.  These values represent
	 the (mapped) value of the parameter displayed at the
	 origin of an axis.  If the pointer is null, the assumed
	 value is 0.0.
   \param <tt> pHighs (vector<Float_t>* [in] = kpNULL) </tt>
         Vector of axis high limit values.  These values 
	 represent the (mapped) value of the parameter that will
	 increment the last channel of the corresponding axis
	 of the histogram.
       
   \retval CSpectrum* 
           A pointer to the spectrum which was created.

   \throw   CSpectrumFactoryException on errors.
*/
CSpectrum* 
CSpectrumFactory::CreateSpectrum(const std::string&   rName, 
				 SpectrumType_t       eSpecType,
				 DataType_t           eDataType, 
				 vector<std::string>& rParameters, 
				 vector<UInt_t>&      rChannels,
				 vector<Float_t>*     pLows,
				 vector<Float_t>*     pHighs)
{


  vector<CParameter> ParameterList;
  ParameterList = ParameterArray(rParameters);

  switch(eSpecType) {		// Actions break out depending on type:
  case ke1D: 
    {
    // Need 1 parameter and 1 axis size.
      Require(eDataType, eSpecType, rName,
	      ParameterList, rChannels, 1,1);

      CParameter p = ParameterList[0];
      Float_t fLow, fHigh;

      if(pLows == (vector<Float_t>*) kpNULL) { 
	fLow = 0.0;
	fHigh= DefaultAxisLength(rChannels[0], p);
      } else {
	fLow = (*pLows)[0];
	fHigh= (*pHighs)[0];
	if(fLow == fHigh) {
	  fLow = 0.0;		// No length means default scaling.
	  fHigh= DefaultAxisLength(rChannels[0], p);
	}
      }
      return Create1D(rName, eDataType, p,
		      rChannels[0], fLow, fHigh);
    }
  case keStrip: 
    {
    // Need 2 parameter and 1 axis size.
      Require(eDataType, eSpecType, rName,
    	      ParameterList, rChannels, 2,1);

      CParameter p = ParameterList[1];
      CParameter c = ParameterList[0];
      Float_t fLow, fHigh;

      if(pLows == (vector<Float_t>*) kpNULL) { 
	fLow = 0.0;
	fHigh= DefaultAxisLength(rChannels[0], p);
      } else {
	fLow = (*pLows)[0];
	fHigh= (*pHighs)[0];
	if(fLow == fHigh) {
	  fLow = 0.0;		// No length means default scaling.
	  fHigh= DefaultAxisLength(rChannels[0], p);
	}
      }
      return CreateStrip(rName, eDataType, p, c,
		      rChannels[0], fLow, fHigh);
    }
  case ke2D:
    {
      
      Require(eDataType, eSpecType, rName,
	      ParameterList, rChannels, 2,2);
      
      CParameter p1 = ParameterList[0];
      CParameter p2 = ParameterList[1];
      Float_t fxLow, fxHigh;
      Float_t fyLow, fyHigh;
      
      
      if(pLows == (vector<Float_t>*) kpNULL) {
	// Default the axis ranges.
	fxLow = fyLow = 0.0;
	fxHigh= DefaultAxisLength(rChannels[0], p1);
	fyHigh= DefaultAxisLength(rChannels[1], p2);
      }
      else {
	fxLow = (*pLows)[0];
	fyLow = (*pLows)[1];
	fxHigh= (*pHighs)[0];
	fyHigh= (*pHighs)[1];
	if(fxLow == fxHigh) {	// Default x axis scale.
	  fxLow = 0.0;
	  fxHigh= DefaultAxisLength(rChannels[0], p1);
	}
	if(fyLow == fyHigh) {	// Default y axis scale.
	  fyLow = 0.0;
	  fyHigh= DefaultAxisLength(rChannels[1], p2);
	}

      }
      return Create2D(rName, eDataType,
		      p1, p2,
		      rChannels[0],
		      fxLow, fxHigh,
		      rChannels[1],
		      fyLow, fyHigh);
    }

  case keBitmask:
    Require(eDataType, eSpecType, rName,
	     ParameterList, rChannels, 1,1);
    if(pLows == (vector<Float_t>*)kpNULL) {
      return CreateBit(rName, eDataType,
		       ParameterList[0], rChannels[0]);
    }
    else {
      UInt_t nLow  = (UInt_t)((*pLows)[0]);
      UInt_t nHigh = (UInt_t)((*pHighs)[0]);
      if(nLow == nHigh) {
	nLow = 0;
	nHigh= rChannels[0];
      }
      return CreateBit(rName, eDataType,
		       ParameterList[0], nLow, nHigh);
    }

  case keSummary:
    {
      // Summary spectra are weird since they require 1 resolution (Y axis) and
      // at *least* one parameter (althought such a small # of params doesn't
      // make much sense.  In any event, the Require member can't be used
      // to check for this, sinc it looks for exact matches in the counts.
      
      if(rChannels.size() != 1) { // Incorrect # of resolutions
	throw CSpectrumFactoryException(eDataType, eSpecType,
					rName, 
			      CSpectrumFactoryException::keBadResolutionCount,
					"Creating summary spectrum");
	
      }
      if(ParameterList.size() < 1) { // Incorect # of parameters.
	throw CSpectrumFactoryException(eDataType, eSpecType,
					rName, 
			     	CSpectrumFactoryException::keBadParameterCount,
					"Creating summary spectrum");
      }

      // Default axis sizing can only be done under the assumption of
      // uniform parameter definitions (the usual case for a summary
      // spectrum.  We'll use ParameterList[0] to figure out what should be
      // done.  We can always change this later if necessary.
      //

      Float_t fLow;
      Float_t fHigh;
      if(pLows == (vector<Float_t>*)kpNULL) {
	
	fLow = 0.0;
	fHigh= DefaultAxisLength(rChannels[0], ParameterList[0]);

      }
      else {
	fLow = (*pLows)[0];
	fHigh= (*pHighs)[0];
	if(fLow == fHigh) {
	  fLow = 0.0;
	  fHigh= DefaultAxisLength(rChannels[0], ParameterList[0]);
	}
      }
      return CreateSummary(rName, eDataType,
			   ParameterList, rChannels[0],
			   fLow, fHigh);
    }
  case keG1D:
    {
      Require(eDataType, eSpecType, rName,
	      ParameterList, rChannels, ParameterList.size(), 1);
      Float_t fLow;
      Float_t fHigh;

      // Default axis mappings can only be derived on the assumption that
      // all of the parameters are uniform in their characteritics.  This
      // is usually the case.

      if(pLows == (vector<Float_t>*)kpNULL) {
	fLow = 0.0;
	fHigh= DefaultAxisLength(rChannels[0], ParameterList[0]);
      }
      else {
	fLow = (*pLows)[0];
	fHigh= (*pHighs)[0];
	if(fLow == fHigh) {
	  fLow = 0.0;
	  fHigh= DefaultAxisLength(rChannels[0], ParameterList[0]);
	}
	
      }
      return CreateG1D(rName, eDataType, ParameterList,
		       rChannels[0], fLow, fHigh);
    }
  case keG2D:
    {
      Require(eDataType, eSpecType, rName,
	      ParameterList, rChannels, ParameterList.size(), 2);
      Float_t fxLow, fyLow;
      Float_t fxHigh, fyHigh;
      
      if(pLows == (vector<Float_t>*) kpNULL) {
	fxLow = 0.0;
	fyLow = 0.0;
	fxHigh= DefaultAxisLength(rChannels[0], ParameterList[0]);
	fyHigh= DefaultAxisLength(rChannels[1], ParameterList[1]);

      }
      else {
	fxLow = (*pLows)[0];
	fxHigh= (*pHighs)[0];
	fyLow = (*pLows)[1];
	fyHigh= (*pHighs)[1];
	if(fxLow == fxHigh) {	// Default x axis:
	  fxLow = 0.0;
	  fxHigh= DefaultAxisLength(rChannels[0], ParameterList[0]);
	}
	if(fyLow == fyHigh) {
	  fyLow = 0.0;
	  fyHigh= DefaultAxisLength(rChannels[1], ParameterList[1]);
	}

      }
      return CreateG2D(rName, eDataType, ParameterList,
		       rChannels[0], fxLow, fxHigh,
		       rChannels[1], fyLow, fyHigh);
      
    }

  default:
    throw CSpectrumFactoryException(eDataType, eSpecType,
				    rName,
				 CSpectrumFactoryException::keBadSpectrumType,
	    "CSpectrumFactory::CreateSpectrum - Deciding spectrum Type"); 
  }
  return (CSpectrum*)NULL;
}
/*!
   Called to create a 1-d spectrum with default axis mapping (that
   is identity transformation between parameter and axis coordinates.

   \param <tt> rName (const string& [in]) </tt>
      The name of the spectrum.
   \param <tt> eType (DataType_t [in]) </tt>
      The data type for the channels in the spectrum.
   \param <tt> Param (CParameter [in]) </tt>
      The parameter that increments the spectrum.
   \param <tt> nChannels (UInt_t [in]) </tt>
      The number of channels on the X axis.

\retval CSpectrum* 
     A pointer to the newly created spectrum. 

\throw CSpectrumFactorException on error.
*/
CSpectrum*
CSpectrumFactory::Create1D(const std::string& rName, 
			   DataType_t Type,
			   CParameter Param,
			   UInt_t     nChannels)
{
  // This function will convert to a call to the fully specified
  // Create1D:

  return Create1D(rName, Type, Param, nChannels,
		  0.0, (Float_t)(nChannels - 1));
}

/*!
    Called to create a 1-d spectrum with arbitrary mapping from
    parameter space to axis coordinates.  The mapping is specified
    by a linear map that places fxLow on channel 0 and fxHigh
    on channel nChannels-1

   \param <tt> rName (const string& [in]) </tt>
      The name of the spectrum.
   \param <tt> eType (DataType_t [in]) </tt>
      The data type for the channels in the spectrum.
   \param <tt> Param (CParameter [in]) </tt>
      The parameter that increments the spectrum.
   \param <tt> nChannels (UInt_t [in]) </tt>
      The number of channels on the X axis.
   \param <tt> fxLow (Float_t [in]): </tt>
      The parameter value that maps to axis channel 0.
   \param <tt> fxHigh (Float_t [in]): </tt>
      The parameter value tha maps to axis channel (nChannels -1).

\retval CSpectrum* 
     A pointer to the newly created spectrum. 

\throw CSpectrumFactorException on error.
*/
CSpectrum* 
CSpectrumFactory::Create1D(const std::string& rName, DataType_t eType, 
			   CParameter rParameter, UInt_t  nChannels,
			   Float_t fxLow, Float_t fxHigh) 
{


  switch(eType) {
  case keWord:
    return new CSpectrum1DW(rName, NextId(), rParameter, nChannels,
			    fxLow, fxHigh);
  case keLong:
    return new CSpectrum1DL(rName, NextId(), rParameter, nChannels,
			    fxLow, fxHigh);
  default:			// Only word and long spectra are supported.
    throw CSpectrumFactoryException(eType, ke1D, rName,
				    CSpectrumFactoryException::keBadDataType,
				    "Creating 1d spectrum");
				    

  }
}

/*!
   Called to create a StripChart spectrum with default axis mapping (that
   is identity transformation between parameter and axis coordinates.

   \param <tt> rName (const string& [in]) </tt>
      The name of the spectrum.
   \param <tt> eType (DataType_t [in]) </tt>
      The data type for the channels in the spectrum.
   \param <tt> Param (CParameter [in]) </tt>
      The parameter that is the value to add to channel
   \param <tt> Param (CParameter [in]) </tt>
      The parameter that carries the time channel
   \param <tt> nChannels (UInt_t [in]) </tt>
      The number of channels on the X axis.

\retval CSpectrum* 
     A pointer to the newly created spectrum. 

\throw CSpectrumFactorException on error.
*/

CSpectrum*
CSpectrumFactory::CreateStrip(const std::string& rName, 
			   DataType_t Type,
			   CParameter Param,
			   CParameter Time,
			   UInt_t     nChannels)
{
  // This function will convert to a call to the fully specified
  // Create1D:

  return CreateStrip(rName, Type, Param, Time, nChannels,
		  0.0, (Float_t) (nChannels - 1));
}


/*!
    Called to create a 1-d spectrum with arbitrary mapping from
    parameter space to axis coordinates.  The mapping is specified
    by a linear map that places fxLow on channel 0 and fxHigh
    on channel nChannels-1

   \param <tt> rName (const string& [in]) </tt>
      The name of the spectrum.
   \param <tt> eType (DataType_t [in]) </tt>
      The data type for the channels in the spectrum.
   \param <tt> Param (CParameter [in]) </tt>
      The parameter that is the value to add to channel
   \param <tt> Param (CParameter [in]) </tt>
      The parameter that carries the time channel
   \param <tt> nChannels (UInt_t [in]) </tt>
      The number of channels on the X axis.
   \param <tt> fxLow (Float_t [in]): </tt>
      The parameter value that maps to axis channel 0.
   \param <tt> fxHigh (Float_t [in]): </tt>
      The parameter value tha maps to axis channel (nChannels -1).

\retval CSpectrum* 
     A pointer to the newly created spectrum. 

\throw CSpectrumFactorException on error.
*/
CSpectrum* 
CSpectrumFactory::CreateStrip(const std::string& rName, DataType_t eType, 
			   CParameter rParameter, CParameter time,
			   UInt_t  nChannels, Float_t fxLow, Float_t fxHigh) 
{


  switch(eType) {
  case keLong:
    return new CSpectrumS(rName, NextId(), rParameter, time,  nChannels,
			    fxLow, fxHigh);
  default:			// Only long spectra are supported.
    throw CSpectrumFactoryException(eType, ke1D, rName,
				    CSpectrumFactoryException::keBadDataType,
				    "Creating 1d spectrum");
				    

  }
}


/*!
  Creates a 2d spectrum with identity mapping between the parameters
  on the axes and the axis coordinates themselves. For arbitrary 
  mappings, see the next function.  This function in fact delgates.

  \param <tt> rName (const string& [in])</tt>
     Name of the new spectrum.
  \param <tt> eType (DataType_t [in])</tt>
     Data type for the channels of the spectrum.
  \param <tt> xParam (CParameter [in]) </tt>
     Parameter derfinition of the parameter on the x axis.
  \param <tt> yParam (CParameter [in]) </tt>
     Parameter definintion of the parameter on the y axis.
  \param <tt> nXChannels (UInt_t [in]) </tt>
     Number of channels on the x axis.
  \param <tt> nYChannels (UInt_t [in]) </tt>
     Number of channels on the y axis.

  \retval <tt> CSpectrum* </tt>
     Pointer to the newly created spectrum.

  \throw CSpectrumFactoryException
*/
CSpectrum*
CSpectrumFactory::Create2D(const std::string& rName, DataType_t eType, 
			   CParameter xParam, CParameter yParam, 
			   UInt_t nXChannels, UInt_t nYChannels)
{
  return Create2D(rName, eType, xParam, yParam,
		  nXChannels, 0.0, (Float_t)(nXChannels - 1),
		  nYChannels, 0.0, (Float_t)(nYChannels - 1));

}
/*!
   Creates a 2d spectrum with arbitrary mapping between the 
   parameters on the axes and the axis coordinates.  The mapping
   is defined by the (fxLow, fxHigh), and (fyLow, fyHigh) pairs.
   Eac pair defines the parameter values for the 0'th and last
   channel of the corresponding axis.


  \param <tt> rName (const string& [in])</tt>
     Name of the new spectrum.
  \param <tt> eType (DataType_t [in])</tt>
     Data type for the channels of the spectrum.
  \param <tt> xParam (CParameter [in]) </tt>
     Parameter derfinition of the parameter on the x axis.
  \param <tt> yParam (CParameter [in]) </tt>
     Parameter definintion of the parameter on the y axis.
  \param <tt> nXChannels (UInt_t [in]) </tt>
     Number of channels on the x axis.
  \param <tt> fxLow (Float_t [in]) </tt>
      The parameter value that corresponds to the origin of the
      x axis.
  \param <tt> fxHigh (Float_t [in]) </tt>
      The parameter value that corresponds to the last channel
      (nXChannels -1) of the X axis.
  \param <tt> nYChannels (UInt_t [in]) </tt>
     Number of channels on the y axis.
  \param <tt> fyLow (Float_t [in]) </tt>
      The parameter value that corresponds to the origin of the
      y axis.
  \param <tt> fyHigh (Float_t [in]) </tt>
      The parameter value that corresponds to the last channel
      (nYChannels -1) of the Y axis.
 
  \retval <tt> CSpectrum* </tt>
     Pointer to the newly created spectrum.

  \throw CSpectrumFactoryException
*/
CSpectrum* 
CSpectrumFactory::Create2D(const std::string& rName, 
			   DataType_t eType, 
			   CParameter xParam, CParameter yParam, 
			   UInt_t nXChannels, 
			   Float_t fxLow, Float_t fxHigh,
			   UInt_t nYChannels, 
			   Float_t fyLow, Float_t fyHigh)
{


  switch(eType) {
  case keWord:
    return new CSpectrum2DW(rName, NextId(),
			    xParam, yParam,
			    nXChannels, fxLow, fxHigh,
			    nYChannels, fyLow, fyHigh);
    break;
  case keByte:

    return new CSpectrum2DB(rName, NextId(),
			    xParam, yParam,
			    nXChannels, fxLow, fxHigh,
			    nYChannels, fyLow, fyHigh);
    break;
  case keLong:
    return new CSpectrum2DL(rName, NextId(),
			    xParam, yParam,
			    nXChannels, fxLow, fxHigh,
			    nYChannels, fyLow, fyHigh);
  default:			// Unsupported data type.
    throw CSpectrumFactoryException(eType, ke2D, rName,
				    CSpectrumFactoryException::keBadDataType,
				    "Creating 2d spectrum");

  }
  return (CSpectrum*)NULL;

}

/*!
  Creates a 1d gamma spectrum with identity transforms between
  the parameters and the axis coordinates. 

  \param <tt> rName (const string& [in])</tt>
      Name to be given to the spectrum
  \param <tt> eType (DataType_t [in]) </tt>
      Data type for each channel of the spectrum.
  \param <tt> rvParameters (vector<CParameter>& [in]) </tt>
      Set of parameters that will be histogrammed.  To first order,
      each valid parameter will increment its corresponding channel.
  \param <tt> nChannels (UInt_t [in]) </tt>
      Number of channels on the axis.  The axis is assumed to cover
      the intervale [0.0, nChannels).
  \retval CSpectrum*
    Pointer to a newly created spectrum.

    \throw CSpectrumFactoryException on errors in creation.
*/
CSpectrum*
CSpectrumFactory::CreateG1D(const std::string& rName, DataType_t eType,
			    vector<CParameter>& rvParameters, 
			    UInt_t nChannels)
{
  if (rvParameters.empty()) {
    throw CSpectrumFactoryException(eType, keG1D, rName,
				    CSpectrumFactoryException::keBadParameterCount,
				    "Creating 1d gamma - need at least one parameter");
				    
  }
  return CreateG1D(rName, eType, rvParameters,
		   nChannels, 0.0, (Float_t)(nChannels - 1));
}

/*!
  Creates a 1d gamma spectrum with arbitrary transforms between
  the parameters and the axis coordinates.  The axis covers the
  same range for all parameters, however in the presence of mapped
  parameters, each parameter will in general have a different 
  transformation function.


  \param <tt> rName (const string& [in])</tt>
      Name to be given to the spectrum
  \param <tt> eType (DataType_t [in]) </tt>
      Data type for each channel of the spectrum.
  \param <tt> rvParameters (vector<CParameter>& [in]) </tt>
      Set of parameters that will be histogrammed.  To first order,
      each valid parameter will increment its corresponding channel.
  \param <tt> nChannels (UInt_t [in]) </tt>
      Number of channels on the axis.  The axis is assumed to cover
      the intervale [0.0, nChannels).
  \param <tt> fxLow (Float_t [in]) </tt>
      Mapped parameter value that increments channel 0 of the axis.
  \param <tt> fxHigh (Float_t [in]) </tt>   
      Mapped parameter value that increments the last channel
      (nChannels - 1) of the spectrum.
  \retval CSpectrum*
    Pointer to a newly created spectrum.

    \throw CSpectrumFactoryException on errors in creation.
*/
CSpectrum*
CSpectrumFactory::CreateG1D(const std::string& rName, DataType_t eType,
			    vector<CParameter>& rvParameters, 
			    UInt_t nChannels, 
			    Float_t fxLow, Float_t fxHigh)
{

  switch(eType) {
  case keWord:
    return new CGamma1DW(rName, NextId(), rvParameters, 
			 nChannels, fxLow, fxHigh);

  case keLong:
    return new CGamma1DL(rName, NextId(), rvParameters, 
			 nChannels, fxLow, fxHigh);

  default:
    throw CSpectrumFactoryException(eType, keG1D, rName,
				    CSpectrumFactoryException::keBadDataType,
				    "Creating 1d Gamma spectrum");
			 
  }
}
/*!
   Creates a 2-d gamma spectrum with an identity mapping between
parameter space and axis space.  That is, each axis will cover 
the interval [0, nChannels) where nChannels is the number of 
channels on \em that axis.

   \param <tt>rName (const string& [in]) </tt>
      The name of the spectrum.
   \param <tt>eType (DataType_t [in]) </tt>
       The data type for each channel of the spectrum.
   \param <tt>rvParameters (vector<CParameter>& [in]) </tt>
       Vector containing the parameter definitions that describe
       how to increment the spectrum.  To first order, the
       spectrum is incremented for each ordered pair.
   \param <tt>nxChannels (UInt_t [in]) </tt>
      Number of channels on the x axis.
   \param <tt>nyChannels (UInt_t [in]) </tt>
      Number of channels on the y axis.

   \retval CSpectrum*
       The spectrum created by the function.

   \throw CSpectrumFactoryException on errors in creation.

*/
CSpectrum*
CSpectrumFactory::CreateG2D(const std::string& rName, 
			    DataType_t eType,
			    vector<CParameter>& rvParameters,
			    UInt_t nxChannels, UInt_t nyChannels)
{
  
  return CreateG2D(rName, eType, rvParameters,
		   nxChannels, 0.0, (Float_t)(nxChannels - 1),
		   nyChannels, 0.0, (Float_t)(nyChannels - 1));
}
/*!
   Creates a 2d gamma spectrum with arbitrary mapping between
   the parameters and axis space.

   \param <tt>rName (const string& [in]) </tt>
      The name of the spectrum.
   \param <tt>eType (DataType_t [in]) </tt>
       The data type for each channel of the spectrum.
   \param <tt>rvParameters (vector<CParameter>& [in]) </tt>
       Vector containing the parameter definitions that describe
       how to increment the spectrum.  To first order, the
       spectrum is incremented for each ordered pair.
   \param <tt>nxChannels (UInt_t [in]) </tt>
      Number of channels on the x axis.
   \param <tt> fxLow (Float_t [in]) </tt>
      Parameter space value that is mapped to channel 0 of the
      x axis.
   \param <tt> fxHigh (Float_t [in]) </tt>
      Parameter space value that is mapped to channel (nxChannels-1)
      of the x axis.
   \param <tt>nyChannels (UInt_t [in]) </tt>
      Number of channels on the y axis.
   \param <tt> fyLow (Float_t [in]) </tt>
      Parameter space value that is mapped to channel 0 of the
      y axis.
   \param <tt> fyHigh (Float_t [in]) </tt>
      Parameter space value that is mapped to channel (nyChannels-1)
      of the y axis.
   \retval CSpectrum*
       The spectrum created by the function.

   \throw CSpectrumFactoryException on errors in creation.
*/
CSpectrum*
CSpectrumFactory::CreateG2D(const std::string& rName, DataType_t eType,
			    vector<CParameter>& rvParameters, 
			    UInt_t nxChannels,
			    Float_t fxLow, Float_t fxHigh,
			    UInt_t nyChannels,
			    Float_t fyLow, Float_t fyHigh)
{



  switch(eType) {
  case keByte:
    return new CGamma2DB(rName, NextId(), rvParameters, 
			 nxChannels,  nyChannels,
			 fxLow, fxHigh,
			 fyLow, fyHigh);

  case keWord:
    return new CGamma2DW(rName, NextId(), rvParameters, 
			 nxChannels, nyChannels,
			 fxLow, fxHigh,
			 fyLow, fyHigh);
  case keLong:
    return new CGamma2DL(rName, NextId(), rvParameters,
			 nxChannels, nyChannels, 
			 fxLow, fxHigh,
			 fyLow, fyHigh);
  default:
    throw CSpectrumFactoryException(eType, keG2D, rName,
				    CSpectrumFactoryException::keBadDataType,
				    "Creating 2d Gamma spectrum");
  }
  return (CSpectrum*)NULL;
}

/*!
   Creates a bitmap spectrum wih an identity mapping that maps bit 0
to channel 0 and bit rChannels-1 to the last channel of the spectrum.

  \param <tt> rName (const string& [in]):</tt>
       The name to be given to the spectrum.
  \param <tt> eType (DataType_t [in]): </tt>
       The type of data stored in each channel of the spectrum.
  \param <tt> Param  (CParameter [in])  </tt>
       The definition of the parameter to histogram.
  \param <tt> nChannels (UInt_t [in])</tt>
       The number of channels in the histogram.  There should be a
       channel for each bit that is desired.

  \retval CSpectrum*
      The spectrum created by the function.
  
  \throw CSpectrumFactoryException on errors in creation. 
*/
CSpectrum*
CSpectrumFactory::CreateBit(const std::string& rName,
			    DataType_t         eType,
			    CParameter         Param,
			    UInt_t             nChannels)
{
  // We delegate to the general function:

  return CreateBit(rName, eType, Param,
		   0, nChannels-1);
}
/*!
   Creates a bitmap spectrum wih an arbitrary mapping that
   maps bit number nLow to the first channel (channel 0) of the
   spectrum and bit number nHigh to the last channel (nHigh-nLow).
   Note therefore that the interval is inclusive of the endpoinst,
   that is all bits in the range [nLow, nHigh] have channels.
   If the parameter is a mapped parameter, the mapped parameter is
   treated as a bitmask.

  \param <tt> rName (const string& [in]):</tt>
       The name to be given to the spectrum.
  \param <tt> eType (DataType_t [in]): </tt>
       The type of data stored in each channel of the spectrum.
  \param <tt> Param  (CParameter [in])  </tt>
       The definition of the parameter to histogram.
  \param <tt> nLow (UInt_t [in])</tt>
       The number of the first bit to display on the spectrum.
  \param <tt> nHigh (Uint_t [in])</tt>
       The number of the last bit to display on the spectrum.

  \retval CSpectrum*
      The spectrum created by the function.
  
  \throw CSpectrumFactoryException on errors in creation. 
*/
CSpectrum* 
CSpectrumFactory::CreateBit(const std::string& rName, 
			    DataType_t eType, 
			    CParameter Param, 
			    UInt_t nLow,
			    UInt_t nHigh)
{


  switch(eType) {
  case keWord:
    return new CBitSpectrumW(rName, NextId(),
			     Param, nLow, nHigh);
  case keLong:
    return new CBitSpectrumL(rName, NextId(),
			     Param, nLow, nHigh);
  default:
    throw CSpectrumFactoryException(eType, keBitmask, rName,
				    CSpectrumFactoryException::keBadDataType,
				    "Creating bit mask  spectrum");

  }

}
/*!
   Creates a summary spectrum.  Summary spectra
   are 2-d spectra, where the X channel is a parameter index
   and the y channels histogram parameter values for each parameter.
   The intent is to be able to see at a  glance at a set of similar channels
   to be able to see which ones have problems.  The parameters are
   mapped to the Y axis via an identity transform.

   \param <tt>  rName (const string& [in])</tt>
       The name of the spectrum.
   \param <tt>  eType (DataType_t [in]) </tt>
       The data type for each channel of the spectrum.
   \param <tt>  rParameters (vector<CParameter>& [in])</tt>
       A reference to a vector of parameters that define the
       x axis and how the spectrum is incremented.
   \param <tt>  nChannels (UInt_t [in]</tt>
      The number of channels on the Y axis of the spectrum.

   \retval CSpectrum*
      A pointer to the spectrum created.
    
    \throw CSpectrumFactoryException 
       If there are problems.

*/
CSpectrum*
CSpectrumFactory::CreateSummary(const std::string& rName, 
			       DataType_t eType, 
			       vector<CParameter>& rParameters, 
			       UInt_t nChannels)
{
  // This function delegates to the more general version of the
  // creational:

  return CreateSummary(rName, eType, rParameters,
		       nChannels, 0.0, (Float_t)(nChannels - 1));
}

/*!
   Creates a summary spectrum.  Summary spectra
   are 2-d spectra, where the X channel is a parameter index
   and the y channels histogram parameter values for each parameter.
   The intent is to be able to see at a  glance at a set of similar channels
   to be able to see which ones have problems.  The parameters are
   mapped to the y axis in accordance with the values fyLow, fyHigh.
   Channel 0 of the y axis will correspond to the mapped paramter 
   value fyLow, and channel nChannels will correspond to fyHigh.

   \param <tt>  rName (const string& [in])</tt>
       The name of the spectrum.
   \param <tt>  eType (DataType_t [in]) </tt>
       The data type for each channel of the spectrum.
   \param <tt>  rParameters (vector<CParameter>& [in])</tt>
       A reference to a vector of parameters that define the
       x axis and how the spectrum is incremented.
   \param <tt>  nChannels (UInt_t [in]</tt>
      The number of channels on the Y axis of the spectrum.
   \param <tt> fyLow (Float_t [in]) </tt>
      Mapped parameter value of channel 0 on the y axis.
   \param <tt> fyHigh (Float_t [in]) </tt>
      Mapped parameter value of the last channel (nChannels-1) on
      the y axis.

   \retval CSpectrum*
      A pointer to the spectrum created.
    
    \throw CSpectrumFactoryException 
       If there are problems.

*/
CSpectrum* 
CSpectrumFactory::CreateSummary(const std::string& rName, 
				DataType_t eType, 
				vector<CParameter>& rParameters, 
				UInt_t nChannels,
				Float_t fxLow, Float_t fxHigh) 
{


  switch(eType) {
  case keWord:
    return new CSummarySpectrumW(rName, NextId(),
				 rParameters, 
				 nChannels, fxLow, fxHigh);
    
  case keByte:
    return new CSummarySpectrumB(rName, NextId(),
				 rParameters, 
				 nChannels, fxLow, fxHigh);
  case keLong:
    return new CSummarySpectrumL(rName, NextId(),
				 rParameters, 
				 nChannels, fxLow, fxHigh);
  default:
    throw CSpectrumFactoryException(eType, keSummary, rName,
				    CSpectrumFactoryException::keBadDataType,
				    "Creating summary  spectrum");

  }
  return (CSpectrum*)NULL;

}
//////////////////////////////////////////////////////////////////////////
//
//  Function:   
//    UInt_t& NextId (  )
//  Operation Type:
//     selector.
//
UInt_t 
CSpectrumFactory::NextId() 
{
// Returns the next spectrum id available 
// Exceptions:  

  UInt_t nId = m_nNextId;
  
  m_nNextId++;
  
  return nId;
}
//////////////////////////////////////////////////////////////////////////
//
//  Function:   
//    vector<CParameter> ParameterArray ( vector<std::string>& rParameters )
//  Operation Type:
//     Utility
//
vector<CParameter> 
CSpectrumFactory::ParameterArray(vector<std::string>& rParameters) 
{
// Transforms an array of parameter names into 
// an array of corresponding CParameter s.
//  Throws a CDictionaryException if one of the
//   names is bad.
//
// Formal Parameters:
//    vector<std::string>& rNames;
//       Names to transform.
// Returns:
//   vector<CParameters>
//         vector of parameter objects.
// Throws:
//     CDictionaryException if any names are bad.
//
  CHistogrammer*     pSorter = (CHistogrammer*)gpEventSink;
  vector<CParameter> vParams;

  // For each element in the rParmeter vector, look up and construct the
  // corresponding CParameter and stick it into the vector.
  //

  for(int i = 0; i < rParameters.size(); i++) {
    CParameter* pParam = pSorter->FindParameter(rParameters[i]);
    if(pParam) {
      vParams.push_back(CParameter(*pParam));
    }
    else {
      if(m_fExceptions) {
	throw CDictionaryException(CDictionaryException::knNoSuchKey,
				   "Parameter Lookup",
				   rParameters[i]);
      }
      else {
	vParams.push_back(CParameter(NullParameter));
      }
				 
    }
  }
  return vParams;

}
///////////////////////////////////////////////////////////////////////
// 
// Function:
//  static void Require(Datatype_t          dType,
//		        SpectrumType_t      sType,
//		        const std::string&  rName
//		        vector<CParameter>& rparams,
//		        vector<UInt_t>&     rResolutions,
//		        UInt_t              nParams, 
//		        UInt_t              nResolutions)
// Operation type:
//    Protected utility.
//  
void
CSpectrumFactory::Require(DataType_t          dType,
			  SpectrumType_t      sType,
			  const std::string&  rName,
			  vector<CParameter>& rparams,
			  vector<UInt_t>&     rResolutions,
			  UInt_t              nParams, 
			  UInt_t              nResolutions)
{
  // Determines if sufficient parameters and resolutions have been supplied.
  // if not, then throws the appropriate exception.
  //

  if(rparams.size() != nParams) {             // Incorrect # of params. 
    throw CSpectrumFactoryException(dType, sType, rName,
		     CSpectrumFactoryException::keBadParameterCount,
		     "Checking parameter counts in CSpectrumFactory::Require");
  }

  if(sType == keG2D && nParams < 2) {
    throw CSpectrumFactoryException(dType, sType, rName,
	    CSpectrumFactoryException::keBadParameterCount,
	    "Checking parameter counts of keG2D in CSpectrumFactory::Require need at least 2 parameters");
  }
  if (sType == keG1D && nParams < 1) {
    throw CSpectrumFactoryException(dType, sType, rName,
				    CSpectrumFactoryException::keBadParameterCount,
				    "Checking keG1D parameter count in CSpectrumFactory::Require - need at least one parameters");
  }
  if(rResolutions.size() != nResolutions) { // Incorrect # of resolutions.
    throw CSpectrumFactoryException(dType, sType, rName,
			      CSpectrumFactoryException::keBadResolutionCount,
		    "Checking resolution counts in CSpectrumFactory::Require");
  }
}

///////////////////////////////////////////////////////////////////////
// 
// Function:
//  static void MappedRequire(Datatype_t          dType,
//		              SpectrumType_t      sType,
//		              const std::string&  rName
//		              vector<Float_t>&    rTransform,
//		              vector<UInt_t>&     rChannels,
//		              UInt_t              nCoords, 
//		              UInt_t              nChans)
// Operation type:
//    Protected utility.
//  
void
CSpectrumFactory::MappedRequire(DataType_t          dType,
				SpectrumType_t      sType,
				const std::string&  rName,
				vector<Float_t>&    rTransform,
				vector<UInt_t>&     rChannels,
				vector<CParameter>& ParameterList,
				UInt_t              nCoords, 
				UInt_t              nChans)
{
  // Determines if sufficient transformation coords, and channel counts have
  // been supplied. If not, then throws the appropriate exception.
  //

  if(rTransform.size() != nCoords) {  // Incorrect # of mapped coords
    throw CSpectrumFactoryException(dType, sType, rName,
		CSpectrumFactoryException::keBadTransformCount,
		"Checking transformation in CSpectrumFactory::MappedRequire");
  }

  if(rChannels.size() != nChans) { // Incorrect # of channels
    throw CSpectrumFactoryException(dType, sType, rName,
	  CSpectrumFactoryException::keBadChannelCount,
	  "Checking mapped channel counts in CSpectrumFactory::MappedRequire");
  }

  // Number of channels less than 0
  for(Int_t i = 0; i < rChannels.size(); i++) {
    if(rChannels[i] < 1)
      throw CSpectrumFactoryException(dType, sType, rName,
	 CSpectrumFactoryException::keBadChannelCount,
	 "Checking mapped channel counts in CSpectrumFactory::MappedRequire");
  }

  // Make sure all of the parameters actually contain valid mapping info
  for(Int_t i = 0; i < ParameterList.size(); i++) {
    if((ParameterList[i].getLow() == 0) &&
       (ParameterList[i].getHigh() == 0))
      throw CSpectrumFactoryException(dType, sType, rName,
	       CSpectrumFactoryException::keNoMappingInformation,
	       "No mapping information in parameter to mapped spectrum in CSpectrumFactory::MappedRequire");
  }
}
/*!
   Determines the default maximum parameter value represented by an axis
   in the case an axis doesn't have its range specified. The value returned
   depends on whether or not the associated parameter has a scale.  If it
   does not, the axis is a 1:1 correspondence to channels.  If it does,
   the axis defaults to cover the entire parameter range:
   \param <TT> nChannels (UInt_t [in]): </TT>
      Number of channels in the axis.
   \param <TT> rParam (CParameter& [in]): </TT>
      Reference to the description of the parameter that's on this axis.

   \retval Float_t  the value of the parameter that corresponds to nChannels-1
      (the 0.0 -> 0 regardless).
*/
Float_t
CSpectrumFactory::DefaultAxisLength(UInt_t nChannels, CParameter& rParam)
{
  if(rParam.hasScale()) {
    return (Float_t)((1 << rParam.getScale())); // param governs mapping.
  }
  else {
    return (Float_t)(nChannels - 1); // The axis governs the mapping.
  }
}


