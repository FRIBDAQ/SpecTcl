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
//  CFold.h
//  Implementation of the Class CFold
//  Created on:      22-Apr-2005 02:40:11 PM
//  Original author: Ron Fox
///////////////////////////////////////////////////////////

#if !defined(__CFOLD_H)
#define  __CFOLD_H


#ifndef __HISTOTYPES_H
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

#ifndef __STL_UTILITY
#include <utility>
#ifndef __STL_UTILITY
#define __STL_UTILITY
#endif
#endif


// Forward definitions:

class CGateContainer;
class CGammaSpectrum;
 
/**
 * A fold is used to control the way that a gamma histogram increments.  Gamma
 * spectrum incrementing is done as follows: If the gate on the spectrum is false,
 * no increment occurs. If the gate on the spectrum is true, and there is no fold
 * on the spectrum it is incremented for each parameter (or parameter pair for 2-
 * d)  in the spectrum definition. If a fold is on the spectrum; The fold is asked
 * to callback the gammaincrement function with vectors of type <pair UInt_t,
 * Float_t> where the int is the parameter id and the float is the value of that
 * parameter (only valid values) to do the fold.
 * @created 22-Apr-2005 12:53:34 PM
 * @author Ron Fox
 * @version 1.0
 */
class CFold
{
private:
  /**
   * The gate container referecing the fold gate.
   */
  CGateContainer *m_pGate;

  
public:

  CFold(CGateContainer* m_pGate);
  virtual     ~CFold();

  // Other canonical operations:

  CFold(const CFold& rhs);
  CFold&      operator=(const CFold& rhs);
  int         operator==(const CFold& rhs) const;
  int         operator!=(const CFold& rhs) const;

  // Object operations.

  std::string getFoldName();
  void        setGate(CGateContainer* pGate);
  void        operator()(STD(vector)<STD(pair)<UInt_t, Float_t> >& rEvent, 
                         CGammaSpectrum* pSpectrum);
  void        operator()(STD(vector)<STD(pair)<UInt_t, Float_t> >& xParams,
			 STD(vector)<STD(pair)<UInt_t, Float_t> >& yParams,
			 CGammaSpectrum* pSpectrum);
  
  // Utility member functions.
protected:
  void        CutDownParameters(STD(vector)<STD(pair)<UInt_t, Float_t> >& rNew, 
                                STD(vector)<STD(pair)<UInt_t, Float_t> >& rOld, 
                                UInt_t removeMe);
  void        CutDownParameters(STD(vector)<STD(pair)<UInt_t, Float_t> >& rNew, 
                               STD(vector)<STD(pair)<UInt_t, Float_t> > & rOld, 
                               UInt_t remove1, UInt_t remove2);
  
};








#endif
