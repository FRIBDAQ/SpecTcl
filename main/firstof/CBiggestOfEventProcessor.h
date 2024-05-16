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
#ifndef CBIGGESTOFEVENTPROCESSOR_H
#define CBIGGESTOFEVENTPROCESSOR_H

#include <EventProcessor.h>
#include <vector>


// Forward class definitions:

class CParameter;
class CEvent;
class CBufferDecoder;
class CAnalyzer;

/*!
  The CBiggestofEventProcessor class works only on unpacked parameters.
  It searches a list of parameters for the biggest parameter that was defined in an event.
  The value and index into the list of that parameter are defined as two new parameters.
  These are referred to as the value and hit parameters respectively.

  Normally, this event processor is created and added dynamically by the biggestof command 
  which is part of this plugin.
*/
class CBiggestofEventProcessor : public CEventProcessor
{
  // object data:
private:
  std::vector<int>    m_sourceIndices; // parameter indices of source parameters.
  int                 m_valueIndex;    // index of value output parameter.
  int                 m_hitIndex;      // index of hit output parameter.

  // constructors and other canonical operations.

public:
  CBiggestofEventProcessor(CParameter* pValue, CParameter* pHit,
			 std::vector<CParameter*>&       sourceParameters);
  CBiggestofEventProcessor(const CBiggestofEventProcessor&   rhs);
  virtual ~CBiggestofEventProcessor();

  CBiggestofEventProcessor& operator=(const CBiggestofEventProcessor& rhs);
  int operator==(const CBiggestofEventProcessor& rhs) const;
  int operator!=(const CBiggestofEventProcessor& rhs) const;

  // Declarations of the CEventProcessor interface parts that we will implement:

  virtual Bool_t operator()(const Address_t pEvent,
			    CEvent&         rEvent,
			    CAnalyzer&      rAnalyzer,
			    CBufferDecoder& rDecoder);
};

#endif
