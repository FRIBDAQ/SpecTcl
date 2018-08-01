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

#ifndef CMAPVALUEPROCESSOR_H
#define CMAPVALUEPROCESSOR_H

#include <EventProcessor.h>
#include <map>




/*!
  Map incoming parameter values to outgoing parameter values.
  this event processor is normally created by the mapvalue command.
  See CMapValueCommand for more information about that.
 */
class CMapValueProcessor : public CEventProcessor
{
private:
  UInt_t               m_incomingParameter;
  UInt_t               m_outgoingParameter;
  std::map<int, float> m_valueMap;

public:
  CMapValueProcessor(UInt_t inParam, UInt_t outParam,
		     std::map<int, float>& valueMap);
  virtual ~CMapValueProcessor();

public:
  virtual Bool_t operator()(const Address_t pEvent,
			    CEvent&         rEvent,
			    CAnalyzer&      rAnalyzer,
			    CBufferDecoder& rDecoder);
};

#endif
