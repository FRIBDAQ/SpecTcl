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
#include <config.h>
#include <CMapValueProcessor.h>
#include <Event.h>

using namespace std;

/*!
   Construction of the Map value processor is just a matter
   of storing the input parameters in member variables.
   \param inParam  - SpecTcl parameter number of the input parameter.
   \param outParam - SpecTcl parameter number of the output parameter.
   \param valueMap - Map that maps input parameters to output parameters.

*/
CMapValueProcessor::CMapValueProcessor(UInt_t          inParam,
				      UInt_t          outParam,
				      map<int,float>& valueMap) :
  m_incomingParameter(inParam),
  m_outgoingParameter(outParam),
  m_valueMap(valueMap)
{
}

/*!
  No action required.
*/
CMapValueProcessor::~CMapValueProcessor()
{}

/*!
   Map an input parameter to an output parameter.  For the output parameter to be
   generated;
   - The input parameter must be defined for this event.
   - The input parameter value must have a map entry in m_valueMap.

   \param pEvent  - Pointer to the raw event (unused)
   \param rEvent  - Event array reference, we'll get our input parameter from here
                    and store our output parameter here as well.
   \param rAnalyzer - Reference to the analyzer object.
   \param rDecoder  - Reference to the buffer decoder.

   \return Bool_t
   \retval kfTRUE.

*/
Bool_t
CMapValueProcessor::operator()(const Address_t pEvent,
			    CEvent&         rEvent,
			    CAnalyzer&      rAnalyzer,
			    CBufferDecoder& rDecoder)
{
  // The size check prevents event expansion...

  if (m_incomingParameter < rEvent.size()   &&
      rEvent[m_incomingParameter].isValid()) {

    int inValue = rEvent[m_incomingParameter];
    map<int, float>::iterator i = m_valueMap.find(inValue);
    if (i != m_valueMap.end()) {
      rEvent[m_outgoingParameter] = i->second;
    }
  }
  return kfTRUE;
}
