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
#include <config.h>
#include "CBiggestOfEventProcessor.h"
#include "Parameter.h"
#include "Event.h"
#include "BufferDecoder.h"
#include "Analyzer.h"


using std::vector;

////////////////////////////////////////////////////////////////////////////////////
// Constructors and other canonicals.

/*!
  'normal' constructof for a CBiggestofEventProcessor:
  \param pValue    - Pointer to the parameter definition for the value output parameter.
  \param pHit      - Pointer to the parameter definition for the hit number output parameter.
  \param sourceParameters - List of pointers to the parameter definitions of the
                     source parameters.
*/
CBiggestofEventProcessor::CBiggestofEventProcessor(CParameter*          pValue,
					       CParameter*          pHit,
					       vector<CParameter*>& sourceParameters) :
  m_valueIndex(pValue->getNumber()),
  m_hitIndex(pHit->getNumber())
{
  for(int i = 0; i < sourceParameters.size(); i++) {
    m_sourceIndices.push_back(sourceParameters[i]->getNumber());
  }

}
/*!
   Copy constructor.  This is just a member copy.
*/
CBiggestofEventProcessor::CBiggestofEventProcessor(const CBiggestofEventProcessor& rhs) :
  CEventProcessor(rhs),
  m_sourceIndices(rhs.m_sourceIndices),
  m_valueIndex(rhs.m_valueIndex),
  m_hitIndex(rhs.m_hitIndex)
{}

/*! 

   Destructor is essentially a no-op... but chains to the base class destructor by
   virtue of being virtual.

*/
CBiggestofEventProcessor::~CBiggestofEventProcessor()
{}

/*!
   Assignment..again an item by item copy.
*/
CBiggestofEventProcessor&
CBiggestofEventProcessor::operator=(const CBiggestofEventProcessor& rhs) 
{
  if (this != &rhs) {
    m_sourceIndices = rhs.m_sourceIndices;
    m_valueIndex    = rhs.m_valueIndex;
    m_hitIndex      = rhs.m_hitIndex;
  }
  return *this;
}
/*!
   Equality compares true if all member variables are equal.
   We rely on the fact that std::vector defines operator== for all
   types that also define operator== (and int certainly does).
*/
int
CBiggestofEventProcessor::operator==(const CBiggestofEventProcessor& rhs) const
{
  return ((m_sourceIndices  == rhs.m_sourceIndices)      &&
	  (m_valueIndex     == rhs.m_valueIndex)         &&
	  (m_hitIndex       == rhs.m_hitIndex));
}
/*!
   Inequality is true if equality is not true.
*/
int
CBiggestofEventProcessor::operator!=(const CBiggestofEventProcessor& rhs) const
{
  return !(*this == rhs);
}
///////////////////////////////////////////////////////////////////////////////////
// Implementations of the CEventProcessor interface

/*!
  The function call operator processes events.  We'll search the
  sourceIdices for any defined parameters.  If we find one that is defined,
  we'll save its index in the parameter designated by m_hitIndex and value in
  the parameter designaed by m_valueIndex.

  It's perfectly legal for none of the parameters in the list to be defined,
  that just means this event did not decode any of the parameers in the source list
  this time around.
  
  \param pEvent      - Points to the raw event.  We ignore this and only work on the
                       decoded parameter set.
  \param rEvent      - Reference to the unpacked parameter 'array'.  This is both input
                       and output for us.
  \param rAnalyzer   - References the analyzer that drives the event decode.  We will not
                       need this.
  \param rDecoder    - References the buffer decoder that understands the gross
                       shape of event buffers.  We won't need this either.

  \return Bool_t
  \retval kfTRUE - We'll always return this, indicating success, and allowing the event
                   processing pipeline to continue.

*/
Bool_t
CBiggestofEventProcessor::operator()(const Address_t pEvent,
				  CEvent&         rEvent,
				  CAnalyzer&      rAnalyzer,
				  CBufferDecoder& rDecoder)
{
  int bestIndex = -1;
  float biggest;
  for (int i =0; i < m_sourceIndices.size(); i++) {
    int index = m_sourceIndices[i];
    if (rEvent[index].isValid()) {
      if(bestIndex == -1) {
	biggest = rEvent[index];
	bestIndex = i;
      } 
      else {
	if (rEvent[index] > biggest) {
	  biggest    = rEvent[index];
	  bestIndex = i;
	}
      }
      
    }
    
  }
  if (bestIndex != -1) {
    rEvent[m_valueIndex] = biggest;
    rEvent[m_hitIndex]   = bestIndex;
  }
  return kfTRUE;
}
