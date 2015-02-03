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

#ifndef __GAMMA1DW_H  // Required for current class
#define __GAMMA1DW_H

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

class CGamma1DW : public CGammaSpectrum
{
  UInt_t         m_nScale;	//!< Spectrum channel count.

 public:

  //Constructors

  CGamma1DW(const STD(string)& rName, UInt_t nId,
	    STD(vector)<CParameter>& rrParameters,
	    UInt_t nScale);	//!< Axis from [0,nScale)

  CGamma1DW(const STD(string)& rName, UInt_t nId,
	    STD(vector)<CParameter>& rrParameters,
	    UInt_t nChannels,
	    Float_t fLow, Float_t fHigh); //!< axis is [fLow,fHigh]


  // Constructor for use by derived classes
  // CGamma1DW(const STD(string)& rName, UInt_t nId,
  //	    STD(vector)<CParameter> rrParameters);

  virtual ~CGamma1DW( ) { }      //Destructor

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

  virtual void Increment(STD(vector)<STD(pair)<UInt_t, Float_t> >& rParameters);
  virtual void Increment(STD(vector)<STD(pair)<UInt_t, Float_t> >& xParameters,
			 STD(vector)<STD(pair)<UInt_t, Float_t> >& yParameters);


  virtual void GetResolutions(STD(vector)<UInt_t>& rvResolutions);

  // Utility functions:

protected:
  void   FillParameterArray(STD(vector)<CParameter> Params);
  static CSpectrum::Axes MakeAxesVector(STD(vector)<CParameter> Params,
					UInt_t             nChannels,
					Float_t fLow, Float_t fHigh);
  void   CreateStorage();
};

#endif
