#ifndef CRATEEVENTPROCESSOR_H
#define CRATEEVENTPROCESSOR_H

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


#include <EventProcessor.h>
#include <TranslatorPointer.h>
#include <TreeParameter.h>
#include <stdint.h>


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
