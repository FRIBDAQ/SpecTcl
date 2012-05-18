#ifndef __CRATEEVENTPROCESSOR_H
#define __CRATEEVENTPROCESSOR_H

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


#ifndef __EVENTPROCESSOR_H
#include <EventProcessor.h>
#endif

#ifndef __TRANSLATORPOINTER_H
#include <TranslatorPointer.h>
#endif


#ifndef __TREEPARAMETER_H
#include <TreeParameter.h>
#endif

#ifndef __CRT_STDINT_H
#include <stdint.h>
#endif


/*!
   This is a trivial event processor.  All it does it
   set rEvent[0] to 1.  That allows a strip chart spectrum
   of event rate vs. time to be created by making rEvent[0] the y
   axis of the strip chart and a timestamp the x axis.

   We also compute the calibrated time as a tree parameter.
   named cTime  which will be the time in milliseconds.

*/
class CRateEventProcessor : public CEventProcessor
{
private:
  CTreeParameter m_milliseconds;
  CTreeParameter m_timestamp;
  double         m_calibrationFactor;
  int32_t        m_timeBase;
  bool           m_needTimeBase;
public:
  CRateEventProcessor();
  virtual ~CRateEventProcessor();

  // Event processor interface implemented functions:

  virtual Bool_t OnBegin(CAnalyzer& rAnalyzer,
			 CBufferDecoder& rDecoder);

  virtual Bool_t operator()(const Address_t pEvent,
			    CEvent&         rEvent,
			    CAnalyzer&      rAnalyzer,
			    CBufferDecoder& rDecoder);
};

#endif
