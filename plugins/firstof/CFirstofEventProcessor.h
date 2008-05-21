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
#ifndef __CFIRSTOFEVENTPROCESSOR_H
#define __CFIRSTOFEVENTPROCESSOR_H

#ifndef __EVENTPROCESSOR_H
#include <EventProcessor.h>
#endif

#ifndef __STL_VECTOR
#include <vector>
#ifndef __STL_VECTOR
#define __STL_VECTOR
#endif
#endif


// Forward class definitions:

class CParameter;
class CEvent;
class CBufferDecoder;
class CAnalyzer;

/*!
  The CFirstofEventProcessor class works only on unpacked parameters.
  It searches a list of parameters for the first parameter that was defined in an event.
  The value and index into the list of that parameter are defined as two new parameters.
  These are referred to as the value and hit parameters respectively.

  Normally, this event processor is created and added dynamically by the firstof command 
  which is part of this plugin.
*/
class CFirstofEventProcessor : public CEventProcessor
{
  // object data:
private:
  std::vector<int>    m_sourceIndices; // parameter indices of source parameters.
  int                 m_valueIndex;    // index of value output parameter.
  int                 m_hitIndex;      // index of hit output parameter.

  // constructors and other canonical operations.

public:
  CFirstofEventProcessor(CParameter* pValue, CParameter* pHit,
			 std::vector<CParameter*>&       sourceParameters);
  CFirstofEventProcessor(const CFirstofEventProcessor&   rhs);
  virtual ~CFirstofEventProcessor();

  CFirstofEventProcessor& operator=(const CFirstofEventProcessor& rhs);
  int operator==(const CFirstofEventProcessor& rhs) const;
  int operator!=(const CFirstofEventProcessor& rhs) const;

  // Declarations of the CEventProcessor interface parts that we will implement:

  virtual Bool_t operator()(const Address_t pEvent,
			    CEvent&         rEvent,
			    CAnalyzer&      rAnalyzer,
			    CBufferDecoder& rDecoder);
};

#endif
