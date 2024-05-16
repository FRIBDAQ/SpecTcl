#ifndef CCONSTPROCESSOR_H
#define CCONSTPROCESSOR_H

/*
    This software is Copyright by the Board of Trustees of Michigan
    State University (c) Copyright 2008

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

/*!
   This event processor performs the calculation of const parameters.
   It is driven by the data in the current event and the data structures that
   are encapsulated by CConstData.
*/
class CConstProcessor : public CEventProcessor
{
  // Realize the singleton pattern:


public:
  static CConstProcessor*  getInstance();

  // only need the function call operator.

public:
  virtual Bool_t operator()(const Address_t pEvent,
			    CEvent& rEvent,
			    CAnalyzer& rAnalyzer,
			    CBufferDecoder& rDecoder); // Physics Event.
};


#endif
