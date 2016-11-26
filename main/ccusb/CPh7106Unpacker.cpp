/*******************************************************************************
*
* CAEN SpA - System Integration  Division
* Via Vetraia, 11 - 55049 - Viareggio ITALY
* +390594388398 - www.caen.it
*
  
    You may use this software under the terms of the GNU public license
    (GPL).  The terms of this license are described at:

     http://www.gnu.org/licenses/gpl.txt

     Author:
             Ron Fox
	
     @file CPh7106Unpacker.h
     @brief unpacking class fror the PH7106 discrimnator/latch.

*/
#include <config.h>
#include "CPh7106Unpacker.h"
#include <Event.h>
#include <vector>
#include <stdint.h>

/**
 * unpack
 *   unpack a single word of data into the parameter that is 
 *   indicated by the module's parameter map.
 *
 * @param p   - Pointer to the chunk of the event we will decode.
 * @param pModuleInfo - POinter to the module info which ihncludes channel -> parameter map.
 * @param rEvent      - Event that's being built.
 * @return int        - Number of words consumed (1).
 */
int
CPh7106Unpacker::unpack(
    TranslatorPointer<UShort_t> p, const CParamMapCommand::ParameterMap* pModuleInfo,
    CEvent& rEvent
)
{
  uint16_t datum = *p;
  const std::vector<int>& parameterMap = pModuleInfo->s_parameterIds;
  rEvent[parameterMap[0]] = datum; // There's only one channel.

  return 1;
}


