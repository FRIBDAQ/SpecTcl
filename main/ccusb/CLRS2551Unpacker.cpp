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
	
     @file CLRS2551Unpacker.h
     @brief Unpacks 12 channel LeCroy scaler module.

*/
#include <config.h>
#include "CLRS2551Unpacker.h"
#include <Event.h>

/**
 * unpack
 *  Unpack the 12 channels of the module.
 *
 * @param p    - Translating pointer to the part of the raw event we'll unpack.
 * @param pModuleInfo - Module information structure for the module.
 * @param rEvent      - Event we're unpacking into.
 * @return int - 12 - number of words unpacked.
 */
int
CLRS2551Unpacker::unpack(
    TranslatorPointer<UShort_t> p, const CParamMapCommand::ParameterMap* pModuleInfo,
    CEvent& rEvent
)
{
  const std::vector<int>& parameterMap = pModuleInfo->s_parameterIds;
  TranslatorPointer<ULong_t> pl(p);
  for (int i = 0; i < 12; i++) {
    uint32_t datum = *pl; ++pl;
    rEvent[parameterMap[i]] = datum & 0x00ffffff; // Q/X are in the upper bits somewhere.
  }
  return 24;			// two words for each channel.
}
