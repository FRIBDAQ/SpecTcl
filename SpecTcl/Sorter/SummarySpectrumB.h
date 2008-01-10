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

//  CSummarySpectrumW.h:
//
//    This file defines the CSummarySpectrumW class.
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
  Revision 5.2  2005/06/03 15:19:24  ron-fox
  Part of breaking off /merging branch to start 3.1 development

  Revision 5.1.2.1  2004/12/21 17:51:26  ron-fox
  Port to gcc 3.x compilers.

  Revision 5.1  2004/11/29 16:56:09  ron-fox
  Begin port to 3.x compilers calling this 3.0

  Revision 4.2  2003/04/01 19:53:46  ron-fox
  Support for Real valued parameters and spectra with arbitrary binnings.

*/

#ifndef __SUMMARYSPECTRUMB_H  //Required for current class
#define __SUMMARYSPECTRUMB_H
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
   A summary spectrum is a special 2-d spectrum that allows you to 
   view several parameters simultaneously.  Each x axis channel
   represents a parameter and the Y axis the spectrum for that 
   parameter.  The idea is that you can take the detectors for a large
   detector array and histogram them all in a summary spectrum.  The
   channels that are not working for some reason or are not gain matched
   will stand out like sore thumbs against the relatively uniform
   pattern of gainmatched, and working detectors.

   Axis scaling is managed a bit differently than for 'ordinary' spectra.
   - The X axis is always a unit-less axis that goes from [0, nParam).
   - The Y axis can be an arbitrary fixed cut (same for all parameters), 
     and will therefore have a separate CAxis item for each parameter
     as the mapping between that cut and the parameter may be different
     for each parameter (suppose for example, the user decides to use
     scaled parameters with the scaling doing gain matching e.g.).

*/
class CSummarySpectrumB  : public CSpectrum
{
  UInt_t              m_nYScale;     //!< Number of Y channels. 
  UInt_t              m_nXChannels;  //!< Number of X chanels.  
  STD(vector)<UInt_t>      m_vParameters; //!< STD(Vector) of parameter id's.
public:

			//Constructor(s) with arguments

  CSummarySpectrumB(const STD(string)& rName, UInt_t nId,
		    STD(vector)<CParameter> rrParameters,
		    UInt_t nYScale); //!< axis represents [0,nYScale-1]
  CSummarySpectrumB(const STD(string)& rName, UInt_t nId,
		    STD(vector)<CParameter> rrParameters,
		    UInt_t nYScale,
		    Float_t fYLow,
		    Float_t fYHigh); //!< Axis represents [fYlow, fYHigh].


  virtual  ~ CSummarySpectrumB( ) { }       //Destructor	
private:
			//Copy constructor [illegal]

  CSummarySpectrumB(const CSummarySpectrumB& acspectrum1dl); 

			//Operator= Assignment Operator [illegal] 

  CSummarySpectrumB operator= (const CSummarySpectrumB& aCSpectrum1D);

			//Operator== Equality Operator [Not too useful but:]
public:
  int operator== (const CSummarySpectrumB& aCSpectrum)
  { return (
	    (CSpectrum::operator== (aCSpectrum))         &&
	    (m_nYScale      == aCSpectrum.m_nYScale)     &&
	    (m_nXChannels   == aCSpectrum.m_nXChannels)  &&
	    (m_vParameters  == aCSpectrum.m_vParameters) 
	    );
  }                             
  // Selectors 

public:
  UInt_t getYScale() const {
    return m_nYScale;
  }
  UInt_t getXChannels() const {
    return m_nXChannels;
  }
  UInt_t getnParams() const {
    return m_vParameters.size();
  }
  UInt_t getParameterId(UInt_t n) const {
    return m_vParameters[n];
  }

  virtual SpectrumType_t getSpectrumType() {
    return keSummary;
  }


  // Mutators (available to derived classes:

protected:
  void setYScale(UInt_t Scale) {
    m_nYScale = Scale;
  }
  void setXChannels(UInt_t Chans) {
    m_nXChannels = Chans;
  }
  //
  //  Operations:
  //   
public:                 
  virtual   void Increment (const CEvent& rEvent)  ;
  virtual   ULong_t operator[](const UInt_t* pIndices) const;
  virtual   void    set(const UInt_t* pIndices, ULong_t nValue);
  virtual   Bool_t UsesParameter (UInt_t nId) const;

  virtual void GetParameterIds(STD(vector)<UInt_t>& rvIds);
  virtual void GetResolutions(STD(vector)<UInt_t>&  rvResolutions);
  virtual CSpectrum::SpectrumDefinition& GetDefinition();

  virtual Bool_t needParameter() const;
  // Utility functions.
protected:
  void CreateStorage();
  void FillParameterArray(STD(vector)<CParameter> Params);
  CSpectrum::Axes CreateAxes(STD(vector)<CParameter> Parameters,
			     UInt_t             nChannels,
			     Float_t fyLow, Float_t fyHigh);
  
};

#endif
