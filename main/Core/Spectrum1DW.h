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

*///  CSpectrum1D.h:
//
//    This file defines the CSpectrum1D class.
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
   Change log:
   $Log$
   Revision 5.2  2005/06/03 15:19:24  ron-fox
   Part of breaking off /merging branch to start 3.1 development

   Revision 5.1.2.1  2004/12/21 17:51:25  ron-fox
   Port to gcc 3.x compilers.

   Revision 5.1  2004/11/29 16:56:08  ron-fox
   Begin port to 3.x compilers calling this 3.0

   Revision 4.2  2003/04/01 19:53:46  ron-fox
   Support for Real valued parameters and spectra with arbitrary binnings.

*/

#ifndef __SPECTRUM1DW_H  //Required for current class
#define __SPECTRUM1DW_H
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

//  Foward Class definitions:

class CParameter;               
                
/*!
   Implements 1d histograms with word channel buckets.  The histogram
   works on arbitrary parameters (reals, integers, mapped integers).  The
   Axis of the spectrum can represent an arbitrary range of parameter space.

*/

class CSpectrum1DW  : public CSpectrum
{
  UInt_t m_nChannels;		//!< Number of channels.
  UInt_t m_nParameter;		//!< Number parameter which is histogrammed

  
public:

  CSpectrum1DW(const std::string&   rName, 
	       UInt_t               nId,
	       const CParameter&    rParameter,
	       UInt_t               nChannels);

  CSpectrum1DW(const std::string&   rName,
	       UInt_t               nId,
	       const   CParameter&  rParameter,
	       UInt_t               nChannels,
	       Float_t              fLow, 
	       Float_t              fHigh);

  //  CSpectrum1DW(const std::string& rName, UInt_t nId,
  //	       const CParameter& rParameter);

  virtual  ~ CSpectrum1DW( ) { }       //Destructor	
private:
			//Copy constructor [illegal]

  CSpectrum1DW(const CSpectrum1DW& acspectrum1dl); 

			//Operator= Assignment Operator [illegal] 

  CSpectrum1DW operator= (const CSpectrum1DW& aCSpectrum1D);

			//Operator== Equality Operator [Not too useful but:]
public:
  int operator== (const CSpectrum1DW& aCSpectrum1D)
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

  // Utility functions:
protected:
  void CreateChannels();	//!< Create storage.
};

#endif
