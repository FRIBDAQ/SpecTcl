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

//
// Header Files:
//


#include "SpectrumFactory.h"
#include "SpectrumFactoryException.h"
#include "DictionaryException.h"
#include "Spectrum.h"
#include "Spectrum1DL.h"
#include "Spectrum1DW.h"
#include "Spectrum2DW.h"
#include "Spectrum2DB.h"
#include "BitSpectrumL.h"
#include "BitSpectrumW.h"
#include "SummarySpectrumW.h"
#include "SummarySpectrumB.h"
#include "Gamma1DW.h"
#include "Gamma1DL.h"
#include "Gamma2DB.h"
#include "Gamma2DW.h"
#include "Histogrammer.h"



                               

static const char* Copyright = 
"CSpectrumFactory.cpp: Copyright 1999 NSCL, All rights reserved\n";


// Assumptions:
//  There are the following global variables which get me to the stuff I need:
//

extern CEventSink* gpEventSink;	// Current event sink which must really be a
				// histogrammer.

static CParameter NullParameter;

// Static class attributes:

UInt_t CSpectrumFactory::m_nNextId = 0;

// Functions for class CSpectrumFactory

//////////////////////////////////////////////////////////////////////////
//
//  Function:   
//    CSpectrum* CreateSpectrum ( const std::string& rName, 
//                                SpectrumType_t     eSpecType, 
//                                DataType_t         eDataType, 
//                                vector<std::string>& rParameters, 
//                                vector<UInt_t>& rResolutions )
//  Operation Type:
//     Creational
//
CSpectrum* 
CSpectrumFactory::CreateSpectrum(const std::string& rName, 
				 SpectrumType_t     eSpecType, 
				 DataType_t         eDataType, 
				 vector<std::string>& rParameters, 
				 vector<UInt_t>&      rResolutions) 
{
// Creates a spectrum and returns a pointer to it.
//
// Formal Parameters:
//
//   const std::string&  rName
//         Name of the spectrum to create.
//
//   SpectrumType_t    eType:      
//          Spectrum type which is one of:
//          { kOneD, kTwoD, kBit, kSummary, kGOneD, kGTwoD}
//     
//   DataType_t          eDataType:
//         Type of data spectrum holds.
//  
//   vector<string>&    rParameters
//         Set of parameters being histogrammed.
//
//   vector<UInt_t>&     rResolutions:
//          Set of resolutions for spectrum axes for which this is
//          a free parameter.
// Returns:
//     A pointer to the spectrum which was created.
// Throws:
//     CSpectrumFactoryException on errors.
//

  vector<CParameter> ParameterList;
  ParameterList = ParameterArray(rParameters);

  switch(eSpecType) {		// Actions break out depending on type:
  case ke1D:
    Require(eDataType, eSpecType, rName,
	    ParameterList, rResolutions, 1,1);
    return Create1D(rName, eDataType, ParameterList[0], rResolutions[0]);
  case ke2D:
    Require(eDataType, eSpecType, rName,
	     ParameterList, rResolutions, 2,2);
    return Create2D(rName, eDataType, 
		    ParameterList[0], ParameterList[1],
		    rResolutions[0],  rResolutions[1]);
  case keBitmask:
    Require(eDataType, eSpecType, rName,
	     ParameterList, rResolutions, 1,1);
    return CreateBit(rName, eDataType,
		     ParameterList[0], rResolutions[0]);
  case keSummary:
    // Summary spectra are weird since they require 1 resolution (Y axis) and
    // at *least* one parameter (althought such a small # of params doesn't
    // make much sense.  In any event, the Require member can't be used
    // to check for this, sinc it looks for exact matches in the counts.

    if(rResolutions.size() != 1) { // Incorrect # of resolutions
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
    return CreateSummary(rName, eDataType,
			 ParameterList, rResolutions[0]);
  case keG1D:
    Require(eDataType, eSpecType, rName,
	    ParameterList, rResolutions, ParameterList.size(), 1);
    return CreateG1D(rName, eDataType, ParameterList, 
		     rResolutions[0]);
  case keG2D:
    Require(eDataType, eSpecType, rName,
	    ParameterList, rResolutions, ParameterList.size(), 2);
    return CreateG2D(rName, eDataType, ParameterList,
		     rResolutions[0], rResolutions[1]);
  default:
    throw CSpectrumFactoryException(eDataType, eSpecType,
				    rName,
				 CSpectrumFactoryException::keBadSpectrumType,
	    "CSpectrumFactory::CreateSpectrum - Deciding spectrum Type"); 
  }

}
//////////////////////////////////////////////////////////////////////////
//
//  Function:   
//    CSpectrum* Create1D ( const std::string& rName, DataType_t eType, 
//                          CParameter rParameter, UInt_t  nRes )
//  Operation Type:
//     Creational.
//
CSpectrum* 
CSpectrumFactory::Create1D(const std::string& rName, DataType_t eType, 
			   CParameter rParameter, UInt_t  nRes) 
{
// Called to create 1-d spectra.
//
// Formal Parameters:
//     const std::string& rName:
//          Name of the spectrum.
//     DataType_t eType:
//          Type of data contained by spectrum.
//      CParameter rParameter:
//           Parameter on which the spectrum is to be set.
//      UInt_t nRes:
//           Resolution of the spectrum.
// Returns:
//    A pointer to the created spectrum.
// Throws:
//    On error throws a CSpectrumFactoryExceptoin.
//

  switch(eType) {
  case keWord:
    return new CSpectrum1DW(rName, NextId(), rParameter, nRes);
  case keLong:
    return new CSpectrum1DL(rName, NextId(), rParameter, nRes);
  default:			// Only word and long spectra are supported.
    throw CSpectrumFactoryException(eType, ke1D, rName,
				    CSpectrumFactoryException::keBadDataType,
				    "Creating 1d spectrum");
				    

  }
}
//////////////////////////////////////////////////////////////////////////
//
//  Function:   
//    CSpectrum* Create2D ( const std::string& rName, DataType_t eType, 
//                          CParameter xParam, CParameter yParam, 
//                          UInt_t nXRes, UInt_t nYRes )
//  Operation Type:
//     Creational
//
CSpectrum* 
CSpectrumFactory::Create2D(const std::string& rName, DataType_t eType, 
			   CParameter xParam, CParameter yParam, 
			   UInt_t nXRes, UInt_t nYRes) 
{
// Creates a 2-d spectrum.
//
//  Formal Parameters:
//        const std::string& rName:
//             Name of the spectrum.
//         DataType_t eType:
//              Type of the data held by the spectrum.
//         CParameter  xPar:
//               X parameter.
//         CParameter yPar:
//               Y Channel parameter.
//         UInt_t nXRes:
//                X spectrum resolution.
//         UInt_t nYRes:
//               Y Spectrum resolution.
// Returns:
//    A pointer to the new spectrum.
// Throws:
//     CSpectrumFactoryException on errors in creation.

  switch(eType) {
  case keWord:
    return new CSpectrum2DW(rName, NextId(),
			    xParam, yParam, nXRes, nYRes);
  case keByte:
    return new CSpectrum2DB(rName, NextId(),
			    xParam, yParam, nXRes, nYRes);

  default:			// Unsupported data type.
    throw CSpectrumFactoryException(eType, ke2D, rName,
				    CSpectrumFactoryException::keBadDataType,
				    "Creating 2d spectrum");

  }

}

CSpectrum*
CSpectrumFactory::CreateG1D(const std::string& rName, DataType_t eType,
			 vector<CParameter>& rvParameters, UInt_t nResolution)
{
  switch(eType) {
  case keWord:
    return new CGamma1DW(rName, NextId(), rvParameters, nResolution);

  case keLong:
    return new CGamma1DL(rName, NextId(), rvParameters, nResolution);

  default:
    throw CSpectrumFactoryException(eType, keG1D, rName,
				    CSpectrumFactoryException::keBadDataType,
				    "Creating 1d Gamma spectrum");
			 
  }
}

CSpectrum*
CSpectrumFactory::CreateG2D(const std::string& rName, DataType_t eType,
			    vector<CParameter>& rvParameters, UInt_t nXRes,
			    UInt_t nYRes)
{
  switch(eType) {
  case keByte:
    return new CGamma2DB(rName, NextId(), rvParameters, nXRes, nYRes);

  case keWord:
    return new CGamma2DW(rName, NextId(), rvParameters, nXRes, nYRes);

  default:
    throw CSpectrumFactoryException(eType, keG2D, rName,
				    CSpectrumFactoryException::keBadDataType,
				    "Creating 2d Gamma spectrum");
  }
}

//////////////////////////////////////////////////////////////////////////
//
//  Function:   
//    CSpectrum* CreateBit ( const std::string& rName, DataType_t eType, CParameter Param, UInt_t nResolution )
//  Operation Type:
//     creational
//
CSpectrum* 
CSpectrumFactory::CreateBit(const std::string& rName, DataType_t eType, 
			    CParameter Param, UInt_t nResolution) 
{
// Creates a bitmask spectrum.
//  Bitmask spectra are incremented once for each
//  bit set in their parameter.
//
// Formal Parameters:
//    const std::string& rName:
//       Name of the spectrum.
//    DataType_t eType:
//       Channel type.
//    CParameter Param:
//       Parameter to be  histogrammed in this way.
//   UInt_t nRes:
//       Resolution of spectrum (determines which set of bits to histogram).
// Returns:
//    Pointer to the created spectrum.
//
// Throws:
//      CSpectrumFactoryException on error.
//

  switch(eType) {
  case keWord:
    return new CBitSpectrumW(rName, NextId(),
			     Param, nResolution);
  case keLong:
    return new CBitSpectrumL(rName, NextId(),
			     Param, nResolution);
  default:
    throw CSpectrumFactoryException(eType, keBitmask, rName,
				    CSpectrumFactoryException::keBadDataType,
				    "Creating bit mask  spectrum");

  }

}
//////////////////////////////////////////////////////////////////////////
//
//  Function:   
//    CSpectrum* CreateSummary ( const std::string& rName, DataType_t eType, 
//                               vector<CParameter>& rParameters, 
//                               UInt_t nYRes )
//  Operation Type:
//     Creational
//
CSpectrum* 
CSpectrumFactory::CreateSummary(const std::string& rName, DataType_t eType, 
				vector<CParameter>& rParameters, UInt_t nYRes) 
{
// Creates a summary spectrum.  Summary spectra
// are 2-d spectra, where the X channel is a parameter index
// and the y channels histogram parameter values for each parameter.
// The intent is to be able to see at a  glance at a set of similar channels
//  to be able to see which ones have problems.
//
// Formal Parameters:
//      const std::string& rName:
//          Name  of the spectrum.
//      DataType_t eType:
//          Type of datat histogrammed.
//      vector<CParameter>& rParams:
//          The set of parameters  to histogram.
//       UInt_t nYResolution:
//           Resolution on the Y axis.
//
// Returns a pointer to the spectrum created.
//  
// Throws:
//      A CSpectrumFactoryException if there are problems.
//

  switch(eType) {
  case keWord:
    return new CSummarySpectrumW(rName, NextId(),
				 rParameters, nYRes);
    
  case keByte:
    return new CSummarySpectrumB(rName, NextId(),
				 rParameters, nYRes);
    
  default:
    throw CSpectrumFactoryException(eType, keSummary, rName,
				    CSpectrumFactoryException::keBadDataType,
				    "Creating summary  spectrum");

  }

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
	    "Checking parameter counts of keG2D in CSpectrumFactory::Require");
  }

  if(rResolutions.size() != nResolutions) { // Incorrect # of resolutions.
    throw CSpectrumFactoryException(dType, sType, rName,
			      CSpectrumFactoryException::keBadResolutionCount,
		    "Checking resolution counts in CSpectrumFactory::Require");
  }
}
