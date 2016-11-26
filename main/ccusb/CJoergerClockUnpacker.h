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
	
     @file CJoergerClockUnpacker.h
     @brief No-OP unpacker for Joerger clock (emits no data).

*/
#ifndef CJOERGERCLOCKUNPACKER_H
#define CJOERGERCLOCKUNPACKER_H

#include "CCCUSBPacket.h"
class CEvent;

/**
 * @class CJoergerClockUnpacker
 *
 *  'Unpackes'  data from a Joerger clock module.
 *  The module produces no data so this is an unpacker
 *  No-op.
 */

class CJoergerClockUnpacker : public CCCUSBPacket
{
public:
  int unpack(
      TranslatorPointer<UShort_t> p, const CParamMapCommand::ParameterMap* pModuleInfo,
      CEvent& rEvent
  );

};

#endif
