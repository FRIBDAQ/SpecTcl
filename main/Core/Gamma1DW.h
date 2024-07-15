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


//  CGamma1DW.h:
//
//    This file defines the CGamma1DW class.
//
// Author:
//    Jason Venema
//    NSCL
//    Michigan State University
//    East Lansing, MI 48824-1321
//    mailto:venemaja@msu.edu
//
//  Copyright 2001 NSCL, All Rights Reserved.
//
/////////////////////////////////////////////////////////////
/*
  Change log:
  $Log$
  Revision 5.3  2007/02/23 20:38:18  ron-fox
  BZ291 enhancement... add gamma deluxe spectrum type (independent x/y
  parameter lists).

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

  Revision 5.1  2004/11/29 16:56:06  ron-fox
  Begin port to 3.x compilers calling this 3.0

  Revision 4.2  2003/04/01 19:53:12  ron-fox
  Support for Real valued parameters and spectra with arbitrary binnings.

*/

#ifndef GAMMA1DW_H  // Required for current class
#define GAMMA1DW_H

#include "Gamma1D.h"

#ifdef UNDEFINED

#include "CGammaSpectrum.h"
#include "Spectrum.h"
#include <string>
#include <vector>
#include <histotypes.h>

//  Foward Class definition:

class CParameter;
class TH1S;             /// Root histogram class we'll use.

class CGamma1DW : public CGammaSpectrum
{
  UInt_t         m_nScale;	//!< Spectrum channel count.
  TH1S*          m_pRootSpectrum;

 public:

  //Constructors

  CGamma1DW(const std::string& rName, UInt_t nId,
	    std::vector<CParameter>& rrParameters,
	    UInt_t nScale);	//!< Axis from [0,nScale)

  CGamma1DW(const std::string& rName, UInt_t nId,
	    std::vector<CParameter>& rrParameters,
	    UInt_t nChannels,
	    Float_t fLow, Float_t fHigh); //!< axis is [fLow,fHigh]


  // Constructor for use by derived classes
  // CGamma1DW(const std::string& rName, UInt_t nId,
  //	    std::vector<CParameter> rrParameters);

  virtual ~CGamma1DW( ) ;

 private:

  //Copy constructor [illegal]
  CGamma1DW(const CGamma1DW& aCGamma1DW);

  //Operator= Assignment operator [illegal]
  CGamma1DW operator= (const CGamma1DW& aCGamma1DW);

  int operator==(const CGamma1DW& cGamma1);
  int operator!=(const CGamma1DW& cGamma1);


  // Selectors

 public:
  virtual SpectrumType_t getSpectrumType()
    {
      return keG1D;
    }

  // Mutators (available to derived classes):

 protected:

  void setScale (UInt_t am_nScale)
    {
      m_nScale = am_nScale;
    }

  // Operations:

 public:

  virtual ULong_t operator[] (const UInt_t* pIndices) const;
  virtual void set (const UInt_t* pIndices, ULong_t nValue);

  virtual void Increment(std::vector<std::pair<UInt_t, Float_t> >& rParameters);
  virtual void Increment(std::vector<std::pair<UInt_t, Float_t> >& xParameters,
			 std::vector<std::pair<UInt_t, Float_t> >& yParameters);


  virtual void GetResolutions(std::vector<UInt_t>& rvResolutions);
  virtual void setStorage(Address_t pStorage);
  virtual Size_t StorageNeeded() const;
  
  
  // Utility functions:

protected:
  void   FillParameterArray(std::vector<CParameter> Params);
  static CSpectrum::Axes MakeAxesVector(std::vector<CParameter> Params,
					UInt_t             nChannels,
					Float_t fLow, Float_t fHigh);
  void   CreateStorage();
};

#endif
#endif