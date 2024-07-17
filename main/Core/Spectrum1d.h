
/*
    This software is Copyright by the Board of Trustees of Michigan
    State University (c) Copyright 2024.

    You may use this software under the terms of the GNU public license
    (GPL).  The terms of this license are described at:

     http://www.gnu.org/licenses/gpl.txt

     Authors:
             Ron Fox
             Jeromy Tompkins
             Giordano Cerizza
             Aaron Chester
             Jin-Hee (Genie) Chang
             Simon Giraud
             NSCL
             Michigan State University
             East Lansing, MI 48824-1321

 */

 
 #ifndef SPECTRUM1D_H
 #define SPECTRUM1D_H

/**
 * @file Spectrum1d.h
 * @brief Issue #128 - Re-d spectrum classes using templates.
 */
#include "Spectrum.h"
#include <string>
#include <vector>
#include <histotypes.h>
#include <stdint.h>


//  Foward Class definitions:

class CParameter;               


/*!
   Implements 1d histograms with Longword channel buckets.  The histogram
   works on arbitrary parameters (reals, integers, mapped integers).  The
   Axis of the spectrum can represent an arbitrary range of parameter space.

*/

template <typename T>
class CSpectrum1D  : public CSpectrum
{
  UInt_t m_nChannels;		//!< Number of channels.
  UInt_t m_nParameter;		//!< Number parameter which is histogrammed
  
public:

  CSpectrum1D(const std::string&   rName, 
	       UInt_t               nId,
	       const CParameter&    rParameter,
	       UInt_t               nChannels);

  CSpectrum1D(const std::string&   rName,
	       UInt_t               nId,
	       const   CParameter&  rParameter,
	       UInt_t               nChannels,
	       Float_t              fLow, 
	       Float_t              fHigh);


  virtual  ~ CSpectrum1D( ) ;    //Destructor	
private:
			//Copy constructor [illegal]

  CSpectrum1D(const CSpectrum1D& aCSpectrum1); 

			//Operator= Assignment Operator [illegal] 

  CSpectrum1D operator= (const CSpectrum1D& aCSpectrum1D);

			//Operator== Equality Operator [Not too useful but:]
public:
  int operator== (const CSpectrum1D& aCSpectrum1D)
  { return (
	    (CSpectrum::operator== (aCSpectrum1D)) &&
	    (m_nChannels == aCSpectrum1D.m_nChannels) &&
	    (m_nParameter == aCSpectrum1D.m_nParameter)
	    );
  }                             
  // Selectors 

public:

  UInt_t getParameter() const
  {
    return m_nParameter;
  }
  virtual SpectrumType_t getSpectrumType() {
    return ke1D;
  }
  // Mutators (available to derived classes:

protected:
  void setScale (UInt_t am_nScale)
  { 
    m_nChannels = am_nScale;
  }
  void setParameter (UInt_t am_nParameter)
  { 
    m_nParameter = am_nParameter;
  }
  // Override base class so we can swoosh storage in/out of TH1I/root spectrum:
  
  virtual void setStorage(Address_t pStorage);


  //
  //  Operations:
  //   
public:

  virtual   void Increment (const CEvent& rE);
  virtual   ULong_t operator[](const UInt_t* pIndices) const;
  virtual   void    set(const UInt_t* pIndices, ULong_t nValue);
  virtual   Bool_t UsesParameter (UInt_t nId) const;

  virtual void GetParameterIds(std::vector<UInt_t>& rvIds);
  virtual void GetResolutions(std::vector<UInt_t>&  rvResolutions);

  virtual Size_t StorageNeeded() const;
  virtual   Size_t Dimension (UInt_t nDimension) const;
  
  // Utility functions:
protected:
  void CreateChannels();	//!< Create storage.
  void CreateRootSpectrum(const char* name, Double_t low, Double_t high, UInt_t bins);
  virtual void deleteRootSpectrum();
};

typedef CSpectrum1D<uint32_t> CSpectrum1DL;
typedef CSpectrum1D<uint16_t> CSpectrum1DW;

// For template classes we must include the implementation:

#include "Spectrum1d.hpp"

 #endif