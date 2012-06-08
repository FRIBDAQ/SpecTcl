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

#ifndef __CRATEPROCESSOR_H
#define __CRATEPROCESSOR_H


#ifndef __EVENTPROCESSOR_H
#include <EventProcessor.h>
#endif

#ifndef __CRT_STDINT_H
#include <stdint.h>
#ifndef __CRT_STDINT_H
#define __CRT_STDINT_H
#endif
#endif


// forward classes:

class CSpectrum;
class CAnalyzer;
class CBufferDecoder;


/*!
   This class computes rate information for a spectrum.  Any
   1-d spectrum can have a rates processor attached to it.
   A rates processor hooks into scaler buffers (which have time base
   information), and computes the total number of counts in the spectrum
   as well as the count increments since the last scaler buffer.
   
   The class supports fetching the values of the spectrum object, 
   the total counts in the spectrum as of the last computation,
   and the increments since th last computation.

*/
class CRateProcessor  : public CEventProcessor
{
private:
  CSpectrum* m_pSpectrum;
  uint64_t   m_totalCounts;
  uint64_t   m_increments;
public:
  // Construtors and cannonicals.

  CRateProcessor(CSpectrum& spectrum);
  CRateProcessor(const CRateProcessor& rhs);
  virtual ~CRateProcessor();

  CRateProcessor& operator=(const CRateProcessor& rhs);
  int operator==(const CRateProcessor& rhs);
  int operator!=(const CRateProcessor& rhs);

  // Selectors

  CSpectrum* getSpectrum();
  uint64_t   getTotals()     const;
  uint64_t   getIncrements() const;
  
  // Virtual function overrides:
  
  virtual Bool_t OnBegin (CAnalyzer& analyzer, CBufferDecoder& decoder);
  virtual Bool_t OnOther (UInt_t nType, CAnalyzer& analyzer, CBufferDecoder& decoder);


  // Utilties:
private:
  void     disable();
  void     adjustSpectrum();
  void     clear();
  uint64_t sum1d();
  uint64_t sum2d();
};


#endif
