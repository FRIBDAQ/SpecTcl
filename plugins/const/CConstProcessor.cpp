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

#include "CConstProcessor.h"
#include "CConstData.h"
#include <SpecTcl.h>
#include "Event.h"

using namespace std;


static void processAndParameters(CEvent& event, 
				 CConstData::ParameterIterator start,
				 CConstData::ParameterIterator stop);
static void processOrParameters(CEvent& event, 
				CConstData::ParameterIterator start,
				CConstData::ParameterIterator stop);

static bool assigned(CEvent& event, UInt_t index);



/*!
   Process events.
   @param pEvent    - Pointer to the raw event, unused.
   @param rEvent    - Reference to the output parameters.. this is read/write.
   @param rAnalyzer - Reference to the analyzer unused.
   @param rDecoder  - Reference to the buffer decoder, unused.
   @return Bool_t
   @retval kfTRUE
*/
Bool_t
CConstProcessor:: operator()(const Address_t pEvent,
			     CEvent& rEvent,
			     CAnalyzer& rAnalyzer,
			     CBufferDecoder& rDecoder)
{
  CConstData& data(CConstData::getInstance());

  processAndParameters(rEvent, data.andBegin(), data.andEnd());
  processOrParameters(rEvent, data.orBegin(), data.orEnd());
  
  return kfTRUE;
}

/*
 *  Static function that returns true if a parameter has beena ssigned to.
 *
 * Parameters:
 *    event  - Reference to the parameter array.
 *    index  - Number of the paramter to check
 */
bool
assigned(CEvent& event, UInt_t index)
{
  return (index < event.size()) && (event[index].isValid());
}
/*
 * Process the and parameters.  The output parameter is set
 * iff all of the input parameters have values.
 * Parameters:
 *    event  - Output event
 *    start  - Iterator to first ParameterDefinition
 *    stop   - Iterator off the end of the parameter definition container.
 */
void processAndParameters(CEvent& event, 
			 CConstData::ParameterIterator start,
			 CConstData::ParameterIterator stop)
{
  while(start != stop) {
    bool increment = true;
    for (int i=0; i < start->s_inputParameters.size(); i++) {
      if (!assigned(event, start->s_inputParameters[i])) {
	increment = false;
	break;
      }
    }

    if (increment) {
      event[start->s_outParameterId] = start->s_outValue;
    }

    stop++;
  }
}
/*
 *  Same as processAnd parameters but the output value is set
 *  if any of the input parameters are defined.
 */
void  processOrParameters(CEvent& event, 
			  CConstData::ParameterIterator start,
			  CConstData::ParameterIterator stop)
{
  while(start != stop) {
    bool increment = false;
    for (int i=0; i < start->s_inputParameters.size(); i++) {
      if (assigned(event, start->s_inputParameters[i])) {
	increment = true;
	break;
      }
    }

    if (increment) {
      event[start->s_outParameterId] = start->s_outValue;
    }

    stop++;
  }
}
