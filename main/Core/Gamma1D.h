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

/**
 * @file Gamma1D.h
 * @note Definition of templated Gamma 1D Spectrum class. See SpecTcl issue #128
 */
#ifndef GAMMA1D_H
#define GAMMA1D_H

#include "CGammaSpectrum.h"
#include "Spectrum.h"
#include <string>
#include <vector>
#include <histotypes.h>
#include <stdint.h>



class CParameter;

/**
 * @class CGamma1D 
 *    THis is a templated class that replaces the old CGamma1D{L,W} classes with a single code
 * base that covers both.   A gamma 1d spectrum is a multiply incrememented spectrum that also supports
 * folds (by being a base class of CGammaSpectrum.
 */
template <typename T>
class CGamma1D : public CGammaSpectrum {
    UInt_t         m_nScale;	// Spectrum channel count.

 public:

  //Canonicals

  CGamma1D(const std::string& rName, UInt_t nId,
	    std::vector<CParameter>& rrParameters,
	    UInt_t nScale);	//!< Axis from [0,nScale)

  CGamma1D(const std::string& rName, UInt_t nId,
	    std::vector<CParameter>& rrParameters,
	    UInt_t nChannels,
	    Float_t fLow, Float_t fHigh); //!< axis is [fLow,fHigh]

  virtual ~CGamma1D( );

 private:

  //Copy constructor [illegal]
  CGamma1D(const CGamma1D& aCGamma1D);

  //Operator= Assignment operator [illegal]
  CGamma1D operator= (const CGamma1D& aCGamma1D);
  int operator==(const CGamma1D& aCGamma1D) const;
  int operator!=(const CGamma1D& aCGamma1d) const;


  // Selectors

 public:


 virtual SpectrumType_t getSpectrumType();
    
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
  static CSpectrum::Axes MakeAxesVector(std::vector<CParameter> Params,
			      UInt_t             nChannels,
			      Float_t fLow, Float_t fHigh);
  void   CreateStorage();
  void CreateRootSpectrum(const char* name, DFloat_t low, DFloat_t high, UInt_t bins);
  virtual void deleteRootSpectrum();
};

typedef CGamma1D<uint32_t> CGamma1DL;
typedef CGamma1D<uint16_t> CGamma1DW;


// Templated classes need their implementations:

#include "Gamma1D.hpp"

#endif
