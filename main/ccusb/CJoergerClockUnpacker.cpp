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
	
     @file CJoergerClockUnpacker.cpp
     @brief No-OP unpacker for Joerger clock (emits no data).

*/
#include "CJoergerClockUnpacker.h"
/**
 * unpack
 *   Return 0 - indicating no data was consumed.
 */
int
CJoergerClockUnpacker::unpack(
    TranslatorPointer<UShort_t> p, const CParamMapCommand::ParameterMap* pModuleInfo,
    CEvent& rEvent
)
{
  return 0;
}
