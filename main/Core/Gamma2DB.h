
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


//  CGamma2DB.h:
//
//    This file defines the CGamma2DB class.
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


#ifndef GAMMA2DB_H  //Required for current class
#define GAMMA2DB_H

#include <CGammaSpectrum.h>
#include <CAxis.h>
#include "Spectrum.h"
#include <string>
#include <vector>
#include <histotypes.h>

//  Forward class definition:

class CParameter;
class TH2C;                   // Root Byte spectrum.

class CGamma2DB : public CGammaSpectrum
{
  UInt_t m_nXScale;		//!< X channel count.
  UInt_t m_nYScale;		//!< Y Channel count.
  TH2C*  m_pRootSpectrum;
  
public:

			//Constructor(s) with arguments

  CGamma2DB(const std::string& rName, UInt_t nId,
	       std::vector<CParameter>& rParameters,
	       UInt_t nXScale, UInt_t nYScale);

  CGamma2DB(const std::string& rName, UInt_t nId,
	    std::vector<CParameter>& rParameters,
	    UInt_t nXScale, UInt_t nYScale,
	    Float_t xLow, Float_t xHigh,
	    Float_t yLow, Float_t yHigh);


  // Constuctor for use by derived classes
  //  CGamma2DB(const std::string& rName, UInt_t nId,
  //    std::vector<CParameter>& rParameter);

  virtual  ~ CGamma2DB( ) ;
private:
			//Copy constructor [illegal]

  CGamma2DB(const CGamma2DB& acspectrum1dl); 

			//Operator= Assignment Operator [illegal] 

  CGamma2DB operator= (const CGamma2DB& aCGamma1D);

			//Operator== Equality Operator [Not too useful but:]
public:
  int operator== (const CGamma2DB& aCGamma)
  { return (
	    (CGamma2DB::operator== (aCGamma)) &&
	    (m_nXScale == aCGamma.m_nXScale) &&
	    (m_Parameters == aCGamma.m_Parameters) &&
	    (m_nYScale == aCGamma.m_nYScale)
	    );
  }
  // Selectors 

public:
  UInt_t getXScale() const
  {
    return m_nXScale;
  }
  UInt_t getYScale() const
  {
    return m_nYScale;
  }

  virtual SpectrumType_t getSpectrumType() {
    return keG2D;
  }
  // Mutators (available to derived classes:

protected:
  void setXScale (UInt_t am_nScale)
  { 
    m_nXScale = am_nScale;
  }
  void setYScale(UInt_t nScale)
  {
    m_nYScale = nScale;
  }

  //
  //  Operations:
  //   
public:                 
  virtual   ULong_t operator[](const UInt_t* pIndices) const;
  virtual   void    set(const UInt_t* pIndices, ULong_t nValue);
  
  virtual void GetResolutions(std::vector<UInt_t>& rvResolutions);
  virtual   Size_t Dimension (UInt_t n) const;  
  virtual   UInt_t Dimensionality () const {
    return 2;
  }

  virtual void Increment(std::vector<std::pair<UInt_t, Float_t> >& rParameters);
  virtual void Increment(std::vector<std::pair<UInt_t, Float_t> >& xParameters,
			 std::vector<std::pair<UInt_t, Float_t> >& yParameters);

   virtual void setStorage(Address_t pStorage);
   virtual Size_t StorageNeeded() const;
private:
  static Axes CreateAxisVector(std::vector<CParameter>& rParams,
			       UInt_t nXchan, UInt_t nYchan,
			       Float_t xLow, Float_t xHigh,
			       Float_t yLow, Float_t yHigh);
  void CreateStorage();
  void SetParameterVector(std::vector<CParameter>& rParameters);
};

#endif
