#ifndef CPH7106UNPACKER_H
#define CPH7106UNPACKER_H
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
#include "CCCUSBPacket.h"

class CEvent;

/**
 * @class CPh7106Unpacker
 *   This class unpacks the single hitmask word of data from a 
 *   PH7106 discriminator/latch.
 */

class CPh7106Unpacker : public CCCUSBPacket
{
public:
  int unpack(
      TranslatorPointer<UShort_t> p,  const CParamMapCommand::ParameterMap* pModuleInfo,
	     CEvent& rEvent
  );
};

#endif
