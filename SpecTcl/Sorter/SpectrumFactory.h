//  CSpectrumFactory.h:
//
//    This file defines the CSpectrumFactory class.
//
// Author:
//    Ron Fox
//    NSCL
//    Michigan State University
//    East Lansing, MI 48824-1321
//    mailto:fox@nscl.msu.edu
//
//  Copyright 1999 NSCL, All Rights Reserved.
//
/////////////////////////////////////////////////////////////

#ifndef __SPECTRUMFACTORY_H  //Required for current class
#define __SPECTRUMFACTORY_H
          
#ifndef __HISTOTYPES_H
#include <histotypes.h>
#endif                     

#ifndef __PARAMETER_H
#include "Parameter.h"
#endif

#include <vector>   //Required for include files, eg <CList.h>
#include <string>   //Required for include files, eg <CList.h>                               

// Forward class definitions:

class CSpectrum;
                                                               
class CSpectrumFactory      
{
  static UInt_t m_nNextId;  // Next Spectrum ID to assign.
  Bool_t        m_fExceptions;	// If true, exceptions are thrown
public:
			//Default constructor
			//Update to access base class attributes 
			//Update to access 1:1 part class attributes 
			//Update to access 1:M part class attributes
			//Update to access 1:1 associated class attributes
			//Update to access 1:M associated class attributes
			//Ensure initial values entered
  CSpectrumFactory (): m_fExceptions(kfTRUE)    { } 
  ~ CSpectrumFactory ( ) { }       //Destructor
	
			//Copy constructor
			//Update to access 1:M part class attributes
			//Update to access 1:1 associated class attributes
			//Update to access 1:M associated class attributes      
  CSpectrumFactory (const CSpectrumFactory& aCSpectrumFactory ) 
  {             
  }                                     

			//Operator= Assignment Operator

  CSpectrumFactory& operator= (const CSpectrumFactory& aCSpectrumFactory)
  { 
    m_fExceptions = aCSpectrumFactory.m_fExceptions;
    return *this;                                                           
  }                                     

			//Operator== Equality Operator
			//Update to access 1:M part class attributes
			//Update to access 1:1 associated class attributes
			//Update to access 1:M associated class attributes      
  int operator== (const CSpectrumFactory& aCSpectrumFactory)
  { 
    return (1);

  }  
  // Selectors:
public:
                       //Get accessor function for attribute
  UInt_t getnNextId() const
  {
    return m_nNextId;
  }
          
  // Mutators:
protected:
                       //Set accessor function for attribute
  void setNextId (UInt_t am_nNextId)
  { 
    m_nNextId = am_nNextId;
  }
  // Actions provided by the class:

public:                   
  CSpectrum* CreateSpectrum (const std::string& rName,
			     SpectrumType_t eSpecType, 
			     DataType_t eDataType, 
			     vector<std::string>& rParameters, 
			     vector<UInt_t>& rResolutions)  ;  
  CSpectrum* CreateSpectrum (const char* pName,
			     SpectrumType_t eSpecType, 
			     DataType_t eDataType, 
			     vector<std::string>& rParameters, 
			     vector<UInt_t>& rResolutions) 
  {
    return CreateSpectrum(std::string(pName),
			  eSpecType, eDataType, rParameters, rResolutions);
  }

  CSpectrum* Create1D (const std::string& rName, DataType_t eType, 
		       CParameter Param, UInt_t  nResolution)  ;
  CSpectrum* Create2D (const std::string& rName, DataType_t eType, 
		       CParameter xParam, CParameter yParam, UInt_t 
		       nXRes, UInt_t nYRes)  ;
  CSpectrum* CreateG1D (const std::string& rName, DataType_t eType,
			vector<CParameter>& rvParameters, UInt_t nResolution);
  CSpectrum* CreateG2D (const std::string& rName, DataType_t eType,
			vector<CParameter>& rvParameters, UInt_t nXRes, 
			UInt_t nYRes);
  CSpectrum* CreateBit (const std::string& rName, DataType_t eType, 
			CParameter Param, UInt_t nResolution)  ;
  CSpectrum* CreateSummary (const std::string& rName, DataType_t eType, 
			    vector<CParameter>& rParameters, UInt_t nYRes)  ;
  UInt_t NextId ()  ;
  Bool_t ExceptionMode() const { return m_fExceptions; }
  Bool_t ExceptionMode(Bool_t fNewMode) {
    Bool_t fOldMode = m_fExceptions;
    m_fExceptions   = fNewMode;
    return fOldMode;
  }

protected:
  vector<CParameter> ParameterArray(vector<std::string>& rParameters) ;
  static void Require(DataType_t          dType,
		      SpectrumType_t      sType,
		      const std::string&  rName,
		      vector<CParameter>& rparams, // Needs all this extra
		      vector<UInt_t>&     rResolutions,	// junk to throw.
		      UInt_t              nParams, 
		      UInt_t              nResolutions);
};

#endif
