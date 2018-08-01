/*
    This software is Copyright by the Board of Trustees of Michigan
    State University (c) Copyright 2014.

    You may use this software under the terms of the GNU public license
    (GPL).  The terms of this license are described at:

     http://www.gnu.org/licenses/gpl.txt

     Authors:
             Ron Fox
             Jeromy Tompkins
             NSCL
             Michigan State University
             East Lansing, MI 48824-1321

 */
//  CSpectrumS.h:
//    This file defines the CSpectrumS class.
//  
//  Modified by:
//    Tim Hoagland
//    NSCL / WIENER
//    s04.thoagland@wittenberg.edu
//
// 
//  Based off of CSpectrum1DL.h written by Ron Fox
//
//    Ron Fox
//    NSCL
//    Michigan State University
//    East Lansing, MI 48824-1321
//    mailto:fox@nscl.msu.edu
//
//  Copyright 1999 NSCL, All Rights Reserved.
//
/////////////////////////////////////////////////////////////
/*
  Change Log:
  
    
  May 5 2005 - StripChart spectra has been verified to work.  Thus making
               this the first working version of this file. -Tim Hoagland


*/

#ifndef SPECTRUMS_H  //Required for current class
#define SPECTRUMS_H
                               //Required for base classes
#include "Spectrum.h"
#include <string>
#include <vector>
#include <histotypes.h>
#include <stdint.h>


//  Foward Class definitions:

class CParameter;               
class TH1I;                        //                 
                
/*!
   Implements 1d StripChart histograms with Longword channel buckets.  The histogram
   works on arbitrary parameters (reals, integers, mapped integers).  The
   Axis of the spectrum can represent an arbitrary range of parameter space.

*/

class CSpectrumS  : public CSpectrum
{
  Int_t m_nChannels;	      //!< Number of channels.
  UInt_t m_nChannel;          //!< Time channel
  UInt_t m_nParameter;	      //!< Count to add to channel
  int    m_nOffset;           //!< Spectrum channel Offest

public:

  CSpectrumS(const std::string&         rName, 
	       UInt_t                   nId,
	       const CParameter&        rParameters,
	       const CParameter&        nChannel,
	       UInt_t                   nChannels);

  CSpectrumS(const std::string&         rName,
	       UInt_t                   nId,
	       const CParameter&        rParameters,
 	       const CParameter&        nChannel,
	       UInt_t                   nChannels,
	       Float_t                  fLow, 
	       Float_t                  fHigh);


  virtual  ~ CSpectrumS( ) ;
private:
			//Copy constructor [illegal]

  CSpectrumS(const CSpectrumS& acspectrumS); 

			//Operator= Assignment Operator [illegal] 

  CSpectrumS operator= (const CSpectrumS& aCSpectrumS);

			//Operator== Equality Operator [Not too useful but:]
public:
  int operator== (const CSpectrumS& aCSpectrumS)
  { return (
	    (CSpectrum::operator== (aCSpectrumS)) &&
	    (m_nChannels == aCSpectrumS.m_nChannels) 
	    // (m_nParameter == aCSpectrumS.m_nParameter)
	    );
  }                             
  // Selectors 

public:

  UInt_t getParameter() const
  {
    return m_nParameter;
  }
  virtual SpectrumType_t getSpectrumType() {
    return keStrip;
  }

  int getOffset() const {
    return m_nOffset;
  }
  UInt_t getChannels() const {
    return m_nChannels;
  }

  // Mutators (available to derived classes:

protected:
  void setScale (UInt_t am_nScale)
  { 
    m_nChannels = am_nScale;
  }
  void setParameter (UInt_t am_nParameter)
  { 
    
    m_nChannel = am_nParameter;
  }

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
  void ShiftDataUp (int64_t nShift);
  void ShiftDataDown(int64_t nShift);
  virtual void setStorage(Address_t pStorage);
  virtual Size_t StorageNeeded() const;


  // Utility functions:
protected:
  void CreateChannels();	//!< Create storage.
};

#endif
