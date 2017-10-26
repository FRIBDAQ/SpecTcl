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

// Author:
//    Ron Fox
//    NSCL
//    Michigan State University
//    East Lansing, MI 48824-1321
//    mailto:fox@nscl.msu.edu
//
// Copyright 2007 NSCL, All rights reserved.
//
//////////////////////////////////////////////////////////////


#ifndef __GAMMA2DD_H
#define __GAMMA2DD_H

#ifndef __CGAMMASPECTRUM_H
#include "CGammaSpectrum.h"
#endif

#ifndef __CAXIS_H
#include <CAxis.h>
#endif

#ifndef __SPECTRUM_H
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

// Forward class definition:

class CParameter;
class TH2I;
class TH2W;
class TH2C;

/*!
   The Gamma2DD spectrum class is a templated class that
   implements the Gamma 2d deluxe spectrum of Dirk.  This is
   in response to BZ enhancement request # 291 described at:
   http://daqbugs.nscl.msu.edu/bugzilla/show_bug.cgi?id=291

   This spectrum is like a 2-d gamma spectrum but has
   an independently specified set of parameters for both the
   X and Y axis.   Increments are for all possible pairs
   of X/Y parameters, with folds operating as expected as well.

  The class is a template class with the data type of the spectrum
  the template parameter.  There is very little type dependent code
  so this is easily possible.

*/
template<typename T, typename R>
class CGamma2DD : public CGammaSpectrum
{
private:
  UInt_t m_nXscale;
  UInt_t m_nYscale;
  std::vector<CParameter> m_xParameters;
  std::vector<CParameter> m_yParameters;
  R*                      m_pRootSpectrum;

public:
  // Constructors/destructors and canonicals.

  CGamma2DD(const std::string& rName, UInt_t nId,
	     std::vector<CParameter>&  xParameters,
	     std::vector<CParameter>&  yParameters,
	     UInt_t xChannels, UInt_t yChannels);
  
  CGamma2DD(const std::string& rName, UInt_t nId,
	     std::vector<CParameter>& xParameters,
	     std::vector<CParameter>& yParameters,
	     UInt_t nXChannels, UInt_t nYChannels,
	     Float_t xLow, Float_t xHigh,
	     Float_t yLow, Float_t yHigh);

  virtual ~CGamma2DD() ;

private:
  CGamma2DD(const CGamma2DD<T, R>& rhs);
  CGamma2DD& operator=(const CGamma2DD<T, R>& rhs);
public:
  int operator==(const CGamma2DD<T, R>& rhs);

  // Selectors:

public:
  UInt_t getXScale() const;
  UInt_t getYScale() const;
  std::vector<CParameter> getXParameters() const;
  std::vector<CParameter> getYParameters() const;
  virtual SpectrumType_t getSpectrumType();

  // Mutators: Only bother with them if we need them:

  // Operations:

public:
  virtual ULong_t operator[](const UInt_t* pIndices) const;
  virtual void    set(const UInt_t* pIndices, ULong_t value);
  
  virtual void   GetResolutions(std::vector<UInt_t>& rvResolutions);
  virtual Size_t Dimension(UInt_t n) const;
  virtual UInt_t Dimensionality() const;

  virtual void Increment(std::vector<std::pair<UInt_t, Float_t> >& rParameters);
  virtual void Increment(std::vector<std::pair<UInt_t, Float_t> >& rXParameters,
			 std::vector<std::pair<UInt_t, Float_t> >& rYParameters);

  virtual CSpectrum::SpectrumDefinition& GetDefinition();
  virtual void setStorage(Address_t pStorage);
  virtual Size_t StorageNeeded() const;

private:
  // Utility functions:

  static CSpectrum::Axes CreateAxisVector(std::vector<CParameter> xParams,
					  std::vector<CParameter> yParams,
					  UInt_t nXchan, UInt_t nYchan,
					  Float_t xLow, Float_t xHigh,
					  Float_t yLow, Float_t yHigh);

  void CreateStorage();
			 

};

// Needed in case the compiler is not a repository compiler with respect to
// templates.

#ifndef __GAMMA2DD_CXX
#include "Gamma2DD.cpp"
#endif

typedef CGamma2DD<Int_t, TH2I>  CGamma2DDL;
typedef CGamma2DD<Short_t, TH2W> CGamma2DDW;
typedef CGamma2DD<Char_t, TH2C>  CGamma2DDB;
#endif
