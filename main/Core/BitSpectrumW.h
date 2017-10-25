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
//  BitSpectrumW.h
//
//    This file defines the CBitSpectrumW class.  That class maintains
//    a 1-d spectrum which is incremented by bits set in bitmasks.
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
/*
  Change Log:
  $Log$
  Revision 5.2  2005/06/03 15:19:22  ron-fox
  Part of breaking off /merging branch to start 3.1 development

  Revision 5.1.2.1  2004/12/21 17:51:24  ron-fox
  Port to gcc 3.x compilers.

  Revision 5.1  2004/11/29 16:56:06  ron-fox
  Begin port to 3.x compilers calling this 3.0

  Revision 4.2  2003/04/01 19:52:40  ron-fox
  Support for Real valued parameters and spectra with arbitrary binnings.

*/

#ifndef __BITSPECTRUMW_H  //Required for current class
#define __BITSPECTRUMW_H
                               //Required for base classes
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

#ifndef __PARAMETER_H
#include "Parameter.h"
#endif
            
class TH1S;             // I*2 spectrum for root.
                
/*!
   Represents a spectrum of longword channels, each channel corresponds
   to a bit in the integerized (mapped) parameter.  If the bit
   is set in the parameter, the corresponding channel is  incremented.
   this version supports low high axes to select from a subset of the bits.

*/
class CBitSpectrumW  : public CSpectrum
{
  UInt_t m_nChannels;		// Spectrum size in channels.
  UInt_t m_nParameter;		// Number parameter which is histogrammed
  CParameter m_PDescription;
  TH1S*      m_pRootSpectrum;
public:

			//Constructor(s) with arguments

  CBitSpectrumW(const std::string& rName, UInt_t nId,
		const CParameter& rParameter,
		UInt_t nChannels);	// Parameter is unmapped.
  CBitSpectrumW(const std::string& rName, UInt_t nId,
		const CParameter& rParameter,
		UInt_t nLow,
		UInt_t nHigh);	// Slice of the space... floats are no good.
		

  virtual  ~CBitSpectrumW( );
private:
			//Copy constructor [illegal]

  CBitSpectrumW(const CBitSpectrumW& acspectrum1dl); 

			//Operator= Assignment Operator [illegal] 

  CBitSpectrumW operator= (const CBitSpectrumW& aCSpectrum1D);

			//Operator== Equality Operator [Not too useful but:]
public:
  int operator== (const CBitSpectrumW& aCSpectrum1D)
  { return (
	    (CSpectrum::operator== (aCSpectrum1D)) &&
	    (m_nChannels == aCSpectrum1D.m_nChannels) &&
	    (m_nParameter == aCSpectrum1D.m_nParameter)
	    );
  }                             
  // Selectors 

public:
  UInt_t getChannels() const
  {
    return m_nChannels;
  }
  UInt_t getParameter() const
  {
    return m_nParameter;
  }
  virtual SpectrumType_t getSpectrumType() {
    return keBitmask;
  }

  // Mutators (available to derived classes:

protected:
  void setChannels (UInt_t am_nChans)
  { 
    m_nChannels = am_nChans;
  }
  void setParameter (UInt_t am_nParameter)
  { 
    m_nParameter = am_nParameter;
  }
  virtual void setStorage(Address_t pStorage);

  //
  //  Operations:
  //   
public:                 
  virtual   void Increment (const CEvent& rEvent)  ;
  virtual   ULong_t operator[](const UInt_t* pIndices) const;
  virtual   void    set(const UInt_t* pIndices, ULong_t nValue);
  virtual   Bool_t UsesParameter (UInt_t nId) const;

  virtual void GetParameterIds(std::vector<UInt_t>& rvIds);
  virtual void GetResolutions(std::vector<UInt_t>&  rvResolutions);
  virtual Size_t StorageNeeded() const;
  
  // Utility functions:
protected:
  void CreateStorage();		//!< Create spec storage
};

#endif
