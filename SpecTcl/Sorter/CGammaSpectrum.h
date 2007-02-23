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

#if !defined(__CGAMMASPECTRUM_H)
#define __CGAMMASPECTRUM_H

#ifndef __SPECTRUM_H
#include "Spectrum.h"
#endif

#ifndef HISTOTYPES_H
#include <histotypes.h>
#endif

#ifndef __STL_VECTOR
#include <vector>
#ifndef __STL_VECTOR
#define __STL_VECTOR
#endif
#endif


#ifndef __STL_STRING
#include <string>
#ifndef __STL_STRING
#define __STL_STRING
#endif
#endif

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
  STD(vector)<UInt_t> m_Parameters;	//!< The parameter list we care about.
  STD(vector)<UInt_t> m_yParameters;    //!< for the y acis for the 2d deluxe.

  CFold          *m_pFold;      //!< Pointer to a CFold object that is the fold for this spectrum.

public:
  // Constructors and other canonical operations:
  //

  CGammaSpectrum(const STD(string)& rName, UInt_t nId,  CSpectrum::Axes Maps,
		 STD(vector)<CParameter>& Parameters,
		 CGateContainer* pGate = pDefaultGate);
  CGammaSpectrum(const STD(string)& rName, UInt_t nId,  CSpectrum::Axes Maps,
		 STD(vector)<CParameter>& xParameters,
		 STD(vector)<CParameter>& yParameters,
		 CGateContainer* pGate = pDefaultGate);
  CGammaSpectrum(const STD(string)& rName, UInt_t nId, 
		 STD(vector)<CParameter>& Parameters,
		 CGateContainer* pGate = pDefaultGate);
  CGammaSpectrum(const STD(string)& rName, UInt_t nId,
		 STD(vector)<CParameter>& xParameters,
		 STD(vector)<CParameter>& yParameters,
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
  virtual void Increment(STD(vector)<STD(pair)<UInt_t, Float_t> >& rParameters) =0;
  virtual void Increment(STD(vector)<STD(pair)<UInt_t, Float_t> >& xParameters,
			 STD(vector)<STD(pair)<UInt_t, Float_t> >& yParameters) = 0;
  Bool_t       haveFold();
  void         Fold(CGateContainer* pGate);
  CFold*       getFold();
  virtual void GetParameterIds(STD(vector)<UInt_t>& rvIds);
  virtual Bool_t UsesParameter(UInt_t nId) const;
  UInt_t getnParams() const
    {
      return m_Parameters.size();
    }
  UInt_t getParameterId (UInt_t n) const {
      return m_Parameters[n];
    }
 

protected:
  void CreateParameterList(STD(vector)<STD(pair)<UInt_t, Float_t> >& outList, 
			   const CEvent& rEvent);
  void CreateYParameterList(STD(vector)<STD(pair)<UInt_t, Float_t> >& outList,
			    const CEvent& rEvent);
  void CreateParameterVector(STD(vector)<CParameter>& Parameters);
  void CreateYParameterVector(STD(vector)<CParameter>& Parameters);

  
};


#endif
