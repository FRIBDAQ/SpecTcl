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

#ifndef SUMMARYSPECTRUM_H
#define SUMMARYSPECTRUM_H

/**
 *  @file SummarySpectrum.h
 *  @brief Template definition of a summary spectrum - derived from Spectrum2D<T>
 */

#include "Spectrum2d.h"
#include <vector>
#include <stdint.h>

/**
 * @class CSummarySpectrum<T>
 * 
 * Templated class for summary spectra.  We let all of the book-keeping be done by Spectrum2D<T>
 * we only need to manage the parameters, the definition and replace the increment method
 * All other things can be done by the base class:
 */

template <typename T>
class CSummarySpectrum : public  CSpectrum2D<T> {
private:
    std::vector<UInt_t>      m_vParameters;  // The paramketer ids.
    CSpectrum::SpectrumDefinition       m_definition;   // Since we have to return a reference.

public:


  CSummarySpectrum(const std::string& rName, UInt_t nId,
		    std::vector<CParameter> rrParameters,
		    UInt_t nYScale); //!< axis represents [0,nYScale-1]
  CSummarySpectrum(const std::string& rName, UInt_t nId,
		    std::vector<CParameter> rrParameters,
		    UInt_t nYScale,
		    Float_t fYLow,
		    Float_t fYHigh); //!< Axis represents [fYlow, fYHigh].


  virtual  ~CSummarySpectrum( );
private:
			//Copy constructor [illegal]

  CSummarySpectrum(const CSummarySpectrum& acspectrum1dl); 

			//Operator= Assignment Operator [illegal] 

  CSummarySpectrum operator= (const CSummarySpectrum& aCSpectrum1D);

			//Operator== Equality Operator [Not too useful but:]

  int operator== (const CSummarySpectrum& aCSpectrum);
  // Selectors 

public:
  UInt_t getnParams() const {
    return m_vParameters.size();
  }
  UInt_t getParameterId(UInt_t n) const {
    return m_vParameters.at(n);    // Throws on bad n.
  }

  virtual SpectrumType_t getSpectrumType() {
    return keSummary;
  }


 
  //
  //  Operations:
  //   
public:                 
  virtual   void Increment (const CEvent& rEvent)  ;
  virtual   Bool_t UsesParameter (UInt_t nId) const;
  virtual void GetParameterIds(std::vector<UInt_t>& rvIds);
  virtual CSpectrum::SpectrumDefinition& GetDefinition();
  virtual Bool_t needParameter() const;
private:
  void ParametersToIds(const std::vector<CParameter>& pDefs);
  void computeDefinition();
};

typedef CSummarySpectrum<uint32_t> CSummarySpectrumL;
typedef CSummarySpectrum<uint16_t> CSummarySpectrumW;
typedef CSummarySpectrum<uint8_t>  CSummarySpectrumB;

#include "SummarySpectrum.hpp"
#endif