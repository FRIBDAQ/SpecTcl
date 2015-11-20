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


/* Change log:
   $Log$
   Revision 5.5  2007/02/23 20:38:18  ron-fox
   BZ291 enhancement... add gamma deluxe spectrum type (independent x/y
   parameter lists).

   Revision 5.4  2006/09/26 11:06:56  ron-fox
   Added 2d multiply incremented spectra to the spectrum factory

   Revision 5.3  2006/09/20 10:57:59  ron-fox
   Modify more licenses to short form

   Revision 5.2  2005/06/03 15:19:24  ron-fox
   Part of breaking off /merging branch to start 3.1 development

   Revision 5.1.2.2  2005/05/11 16:54:54  thoagland
   Add Support for Stripchart Spectra

   Revision 5.1.2.1  2004/12/21 17:51:25  ron-fox
   Port to gcc 3.x compilers.

   Revision 5.1  2004/11/29 16:56:09  ron-fox
   Begin port to 3.x compilers calling this 3.0

   Revision 4.2  2003/04/01 19:53:46  ron-fox
   Support for Real valued parameters and spectra with arbitrary binnings.

*/

#ifndef __SPECTRUMFACTORY_H  //Required for current class
#define __SPECTRUMFACTORY_H
          
#ifndef __HISTOTYPES_H
#include <histotypes.h>
#endif                     

#ifndef __PARAMETER_H
#include "Parameter.h"
#endif

#ifndef __STL_VECTOR
#include <vector>   //Required for include files, eg <CList.h>
#ifndef __STL_VECTOR
#define __STL_VECTOR
#endif
#endif

#ifndef __STL_STRING
#include <string>   
#ifndef __STL_STRING
#define __STL_STRING
#endif
#endif


//Required for include files, eg <CList.h>                               

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
  CSpectrum* CreateSpectrum (const STD(string)& rName,
			     SpectrumType_t eSpecType, 
			     DataType_t eDataType, 
			     STD(vector)<STD(string)>& rParameters, 
			     STD(vector)<UInt_t>&  rChannels,
			     STD(vector)<Float_t>* pLows  = (STD(vector)<Float_t>*)kpNULL,
			     STD(vector)<Float_t>* pHighs = (STD(vector)<Float_t>*)kpNULL);

  CSpectrum* CreateSpectrum (const char* pName,
			     SpectrumType_t eSpecType, 
			     STD(vector)<STD(string)>& rParameters,
			     DataType_t eDataType, 
			     STD(vector)<UInt_t>&  rChannels,
			     STD(vector)<Float_t>* pLows = (STD(vector)<Float_t>*)kpNULL,
			     STD(vector)<Float_t>* pHighs= (STD(vector)<Float_t>*)kpNULL)
    {
      return CreateSpectrum(STD(string)(pName),
			    eSpecType, eDataType, rParameters, rChannels,
			    pLows, pHighs);
    }

  // Create spectra where the axis specs must be segregated (e.g.
  // gamma 2d deluxe (the only current type):
  //

  CSpectrum* CreateSpectrum(const STD(string)& rName,
			    SpectrumType_t eSpecType,
			    DataType_t     eDataType,
			    STD(vector)<STD(string)>   xParameters,
			    STD(vector)<STD(string)>   yParameters,
			    UInt_t                     xChannels,
			    UInt_t                     yChannels,
			    STD(vector)<Float_t>*      pLows = (STD(vector)<Float_t>*)kpNULL,
			    STD(vector)<Float_t>*      pHighs= (STD(vector)<Float_t>*)kpNULL);



  // Create spectra that have arrays of parameter arrays (e.g.
  // gamma summary spectra.

  CSpectrum* CreateSpectrum(const std::string name,
			    SpectrumType_t    specType,
			    DataType_t        dataType,
			    std::vector<std::vector<std::string> > parameterNames,
			    std::vector<UInt_t> channels,
			    std::vector<Float_t>* pLows  = (std::vector<Float_t>*)kpNULL,
			    std::vector<Float_t>* pHighs = (std::vector<Float_t>*)kpNULL);

  // Create a gamma 2d deluxe spectrum:

  CSpectrum* CreateG2dDeluxe(STD(string) name,
			     DataType_t        eType,
			     STD(vector)<CParameter>& rxParameters,
			     STD(vector)<CParameter>& ryParameters,
			     UInt_t  xChannels, 
			     Float_t xLow, Float_t xHigh,
			     UInt_t  yChannels,
			     Float_t yLow, Float_t yHigh);
			     


  // Create 1d spectrum:

  CSpectrum* Create1D (const STD(string)& rName, DataType_t eType, 
		       CParameter Param, UInt_t  nChannels)  ;
  CSpectrum* Create1D (const STD(string)& rName, DataType_t eType,
		       CParameter Param, UInt_t  nChannels,
		       Float_t fxLow, Float_t fxHigh);
  
  // Create StripChart spectrum:

  CSpectrum* CreateStrip (const STD(string)& rName, DataType_t eType, 
		       CParameter Param, CParameter Time, UInt_t  nChannels)  ;
  CSpectrum* CreateStrip (const STD(string)& rName, DataType_t eType,
		       CParameter Param,CParameter Time, UInt_t  nChannels,
		       Float_t fxLow, Float_t fxHigh);
  

  // Create 2d Spectra:

  CSpectrum* Create2D (const STD(string)& rName, DataType_t eType,
		       CParameter xParam, CParameter yParam,
		       UInt_t nXChannels, UInt_t nYChannels);
  CSpectrum* Create2D (const STD(string)& rName, DataType_t eType, 
		       CParameter xParam, CParameter yParam, 
		       UInt_t nXChannels, 
		       Float_t fXLow, Float_t fXHigh,
		       UInt_t nYChanels,
		       Float_t fYLow, Float_t fYHigh)  ;


  CSpectrum* CreateG1D (const STD(string)& rName, DataType_t eType,
			STD(vector)<CParameter>& rvParameters, UInt_t nResolution);
  CSpectrum* CreateG1D(const STD(string)& rName, DataType_t eType,
		       STD(vector)<CParameter>& rvParameters, UInt_t nChannels,
		       Float_t fxLow, Float_t fxHigh);

  CSpectrum* CreateG2D (const STD(string)& rName, DataType_t eType,
			STD(vector)<CParameter>& rvParameters, UInt_t nXRes, 
			UInt_t nYRes);
  CSpectrum* CreateG2D(const STD(string)& rName, DataType_t eType,
		       STD(vector)<CParameter>& rvParameters, 
		       UInt_t nXChannels, 
		       Float_t fxLow, Float_t fxHigh,
		       UInt_t nYChannels,
		       Float_t fyLow, Float_t fyHigh);

  CSpectrum* CreateBit (const STD(string)& rName, DataType_t eType, 
			CParameter Param, UInt_t nResolution)  ;
  CSpectrum* CreateBit(const STD(string)& rName, DataType_t eType, 
		       CParameter Param, UInt_t nLow, UInt_t nHigh);

  CSpectrum* CreateSummary (const STD(string)& rName, DataType_t eType, 
			    STD(vector)<CParameter>& rParameters, UInt_t nYRes)  ;
  CSpectrum* CreateSummary(const STD(string)& rName, DataType_t eType, 
			   STD(vector)<CParameter>& rParameters, UInt_t nyChannels,
			   Float_t fyLow, Float_t fyHigh);
  CSpectrum* Create2DMultiple(STD(string) name, DataType_t eType,
			      STD(vector)<CParameter>& parameters, 
			      UInt_t  xChans,
			      Float_t xLow, Float_t xHigh,
			      UInt_t  yChans,
			      Float_t yLow, Float_t yHigh);

  CSpectrum* CreateGammaSummary(std::string name, DataType_t eType,
				std::vector<std::vector<CParameter> > parameters,
				UInt_t      yChannels,
				Float_t     yLow,
				Float_t     yHigh);


  UInt_t NextId ()  ;
  Bool_t ExceptionMode() const { return m_fExceptions; }
  Bool_t ExceptionMode(Bool_t fNewMode) {
    Bool_t fOldMode = m_fExceptions;
    m_fExceptions   = fNewMode;
    return fOldMode;
  }

protected:
  STD(vector)<CParameter> ParameterArray(STD(vector)<STD(string)>& rParameters) ;
  static void Require(DataType_t          dType,
		      SpectrumType_t      sType,
		      const STD(string)&  rName,
		      STD(vector)<CParameter>& rparams, // Needs all this extra
		      STD(vector)<UInt_t>&     rResolutions,	// junk to throw.
		      UInt_t              nParams, 
		      UInt_t              nResolutions);
  static void MappedRequire(DataType_t         dType,
			    SpectrumType_t     sType,
			    const STD(string)& rName,
			    STD(vector)<Float_t>&   rTransform,
			    STD(vector)<UInt_t>&    rChannels,
			    STD(vector)<CParameter>& ParameterList,
			    UInt_t             nCoords,
			    UInt_t             nChans);
  static Float_t  DefaultAxisLength(UInt_t nChannels, 
				    CParameter& rParam);
};

#endif
