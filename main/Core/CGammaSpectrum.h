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


///////////////////////////////////////////////////////////
//  CGammaSpectrum.h
//  Implementation of the Class CGammaSpectrum
//  Created on:      22-Apr-2005 12:55:02 PM
//  Original author: Ron Fox
///////////////////////////////////////////////////////////

#ifndef CGAMMASPECTRUM_H
#define CGAMMASPECTRUM_H

#include "Spectrum.h"
#include <histotypes.h>
#include <vector>
#include <string>

// Forward definitions:

class CGateContainer;
class CFold;
class CEvent;
class CParameter;

/**
 * Base class for gamma spectra.  This allows a gamma spectrum to be passed into
 * the fold directly.
 * @created 22-Apr-2005 12:55:02 PM
 * @author Ron Fox
 * @version 1.0
 * @updated 22-Apr-2005 02:40:48 PM
 */
class CGammaSpectrum : public CSpectrum
{
protected:
  std::vector<UInt_t> m_Parameters;	//!< The parameter list we care about.
  std::vector<UInt_t> m_yParameters;    //!< for the y acis for the 2d deluxe.

  CFold          *m_pFold;      //!< Pointer to a CFold object that is the fold for this spectrum.

public:
  // Constructors and other canonical operations:
  //

  CGammaSpectrum(const std::string& rName, UInt_t nId,  CSpectrum::Axes Maps,
		 std::vector<CParameter>& Parameters,
		 CGateContainer* pGate = pDefaultGate);
  CGammaSpectrum(const std::string& rName, UInt_t nId,  CSpectrum::Axes Maps,
		 std::vector<CParameter>& xParameters,
		 std::vector<CParameter>& yParameters,
		 CGateContainer* pGate = pDefaultGate);
  CGammaSpectrum(const std::string& rName, UInt_t nId, 
		 std::vector<CParameter>& Parameters,
		 CGateContainer* pGate = pDefaultGate);
  CGammaSpectrum(const std::string& rName, UInt_t nId,
		 std::vector<CParameter>& xParameters,
		 std::vector<CParameter>& yParameters,
		 CGateContainer* pGate = pDefaultGate);
  virtual ~CGammaSpectrum();

  // Copy like canonicals are not defined:

private:
  CGammaSpectrum(const CGammaSpectrum& rhs);
  CGammaSpectrum& operator=(const CGammaSpectrum& rhs);
  int operator==(const CGammaSpectrum& rhs);
  int operator!=(const CGammaSpectrum& rhs);
public:

  virtual void Increment(const CEvent& rEvent);
  virtual void Increment(std::vector<std::pair<UInt_t, Float_t> >& rParameters) =0;
  virtual void Increment(std::vector<std::pair<UInt_t, Float_t> >& xParameters,
			 std::vector<std::pair<UInt_t, Float_t> >& yParameters) = 0;
  Bool_t       haveFold();
  void         Fold(CGateContainer* pGate);
  CFold*       getFold();
  virtual void GetParameterIds(std::vector<UInt_t>& rvIds);
  virtual Bool_t UsesParameter(UInt_t nId) const;
  UInt_t getnParams() const
    {
      return m_Parameters.size();
    }
  UInt_t getParameterId (UInt_t n) const {
      return m_Parameters[n];
    }
 

  virtual Bool_t needParameter() const;

protected:
  void CreateParameterList(std::vector<std::pair<UInt_t, Float_t> >& outList, 
			   const CEvent& rEvent);
  void CreateYParameterList(std::vector<std::pair<UInt_t, Float_t> >& outList,
			    const CEvent& rEvent);
  void CreateParameterVector(std::vector<CParameter>& Parameters);
  void CreateYParameterVector(std::vector<CParameter>& Parameters);

  
};


#endif
