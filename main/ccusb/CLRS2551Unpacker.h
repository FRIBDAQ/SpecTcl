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
#ifndef CLRS2551UNPACKER_H
#define CLRS2551UNPACKER_H


#include "CCCUSBPacket.h"
class CEvent;

/**
 * @class CLRS2551Unpacker - unpacks the 12 channels of an LRS 2551 scaler module.
 *
 */
class CLRS2551Unpacker : public CCCUSBPacket
{
public:
  int unpack(
        TranslatorPointer<UShort_t> p, const CParamMapCommand::ParameterMap* pModuleInfo,
	CEvent& rEvent
  );
};

#endif
