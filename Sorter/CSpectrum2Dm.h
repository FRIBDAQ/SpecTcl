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

#ifndef __CSPECTRUM2DM_H
#define __CSPECTRUM2DM_H

#ifndef __SPECTRUM_H
#include "Spectrum.h"
#endif

#ifndef __CAXIS_H
#include "CAxis.h"
#endif

#ifndef __PARAMETER_H
#include <Parameter.h>
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


/*!
   This class provides base class support for a 2-d spectrum
   that is multiply incremented. This spectrum is quite a bit different
   from a gamma-2d spectrum which is also multiply incremented.  In this
   case the intent is to capture a spectrum that is the sum of 2-d's that
   have parameters that have been hardware (or software) gain matched.
   The specctrum is defined on a series of parameter pairs.  Each pair
   is allowed to increment the spectrum.. if the gate is made.
*/

class CSpectrum2Dm : public CSpectrum
{
  // Data types:
public:
  typedef STD(vector)<UInt_t>  Parameters;
  typedef STD(vector)<CAxis>   Axes;
  
  
  // We're going to expose a few of our data structures to the
  // derived classes just to make access a bit quicker when we need
  // to get around to incrementing... bad idea but for now it's motivated
  // by practicality, not by wisdom.
  // 
protected:
  UInt_t     m_xChannels;
  UInt_t     m_yChannels;
  Parameters m_parameterList;
  Axes       m_axisMappings;

  // Construtors and other Canonicals:
public:
  CSpectrum2Dm(STD(string)              name,
	       UInt_t                   id,
	       STD(vector)<CParameter>& parameters,
	       UInt_t                   xscale,
	       UInt_t                   yscale);
  CSpectrum2Dm(STD(string)              name,
	       UInt_t                   id,
	       STD(vector)<CParameter>& parameters,
	       UInt_t                   xchans,
	       UInt_t                   ychans,
	       Float_t  xlow, Float_t   xhigh,
	       Float_t  ylow, Float_t   yhigh);

private:
  CSpectrum2Dm(const CSpectrum2Dm& rhs);
  CSpectrum2Dm& operator=(const CSpectrum2Dm& rhs);
  int operator==(const CSpectrum2Dm& rhs) const;
  int operator!=(const CSpectrum2Dm& rhs) const;
public:

  //   These are virtual member functions that can be 
  //   shared by all implementation so fCSpectrum2Dm<type>
  //
  virtual Bool_t UsesParameter(UInt_t nId) const;
  virtual void   GetParameterIds(STD(vector)<UInt_t>& rvIds);
  virtual void   GetResolutions(STD(vector)<UInt_t>& rvResolutions);


protected:
  CSpectrum::SpectrumDefinition GetCommonDefinition();

private:
  CSpectrum::Axes CreateAxisVector(const CParameter& xParam,
				   UInt_t      nxChannels,
				   Float_t     fxLow, Float_t fxHigh,
				   const CParameter& yParam,
				   UInt_t      nyChannels,
				   Float_t     fyLow, Float_t fyHigh);

  void  CreateMappings(STD(vector)<CParameter>& parameters, 
		      Float_t xlow, Float_t xhigh,
		      Float_t ylow, Float_t yhigh);
};


#endif
