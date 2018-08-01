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

//  CGamma1DL.h:
//
//    This file defines the CGamma1DL class.
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

#ifndef GAMMA1DL_H  // Required for current class
#define GAMMA1DL_H

#include "CGammaSpectrum.h"
#include "Spectrum.h"
#include <string>
#include <vector>
#include <histotypes.h>

class TH1I;             // Root 1d 32 bit spectrum.

//  Foward Class definition:

class CParameter;

class CGamma1DL : public CGammaSpectrum
{
  UInt_t         m_nScale;	// Spectrum channel count.
  TH1I*          m_pRootSpectrum;
 public:

  //Constructors

  CGamma1DL(const std::string& rName, UInt_t nId,
	    std::vector<CParameter>& rrParameters,
	    UInt_t nScale);	//!< Axis from [0,nScale)

  CGamma1DL(const std::string& rName, UInt_t nId,
	    std::vector<CParameter>& rrParameters,
	    UInt_t nChannels,
	    Float_t fLow, Float_t fHigh); //!< axis is [fLow,fHigh]


  // Constructor for use by derived classes
  // CGamma1DL(const std::string& rName, UInt_t nId,
  //	    std::vector<CParameter> rrParameters);

  virtual ~CGamma1DL( );

 private:

  //Copy constructor [illegal]
  CGamma1DL(const CGamma1DL& aCGamma1DL);

  //Operator= Assignment operator [illegal]
  CGamma1DL operator= (const CGamma1DL& aCGamma1DL);
  int operator==(const CGamma1DL& aCGamma1D) const;
  int operator!=(const CGamma1DL& aCGamma1d) const;


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
  static Axes MakeAxesVector(std::vector<CParameter> Params,
			      UInt_t             nChannels,
			      Float_t fLow, Float_t fHigh);
  void   CreateStorage();
};

#endif
