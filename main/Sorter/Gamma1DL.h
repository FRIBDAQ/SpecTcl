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

#ifndef __GAMMA1DL_H  // Required for current class
#define __GAMMA1DL_H

#ifndef __CGAMMASPECTRUM_H
#include "CGammaSpectrum.h"
#endif

#ifndef __SPECTRUM_H  // Required for base class
#include "Spectrum.h"
#endif

#ifndef __STL_STRING
#include <string>
#ifndef __STL_STRING
#define __STL_STRING
#endif
#endif

#ifndef __STL_VECTOR
#include <vector>
#ifndef __STL_VECTOR
#define __STL_VECTOR
#endif
#endif

#ifndef __HISTOTYPES_H
#include <histotypes.h>
#endif

//  Foward Class definition:

class CParameter;

class CGamma1DL : public CGammaSpectrum
{
  UInt_t         m_nScale;	// Spectrum channel count.

 public:

  //Constructors

  CGamma1DL(const STD(string)& rName, UInt_t nId,
	    STD(vector)<CParameter>& rrParameters,
	    UInt_t nScale);	//!< Axis from [0,nScale)

  CGamma1DL(const STD(string)& rName, UInt_t nId,
	    STD(vector)<CParameter>& rrParameters,
	    UInt_t nChannels,
	    Float_t fLow, Float_t fHigh); //!< axis is [fLow,fHigh]


  // Constructor for use by derived classes
  // CGamma1DL(const STD(string)& rName, UInt_t nId,
  //	    STD(vector)<CParameter> rrParameters);

  virtual ~CGamma1DL( ) { }      //Destructor

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
  
  virtual void Increment(STD(vector)<STD(pair)<UInt_t, Float_t> >& rParameters);
  virtual void Increment(STD(vector)<STD(pair)<UInt_t, Float_t> >& xParameters,
			 STD(vector)<STD(pair)<UInt_t, Float_t> >& yParameters);


  virtual void GetResolutions(STD(vector)<UInt_t>& rvResolutions);

  // Utility functions:

protected:
  static Axes MakeAxesVector(STD(vector)<CParameter> Params,
			      UInt_t             nChannels,
			      Float_t fLow, Float_t fHigh);
  void   CreateStorage();
};

#endif
