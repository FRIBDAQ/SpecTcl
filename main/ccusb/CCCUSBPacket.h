#ifndef CCCUSBPACKET_H
#define CCCUSBPACKET_H

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

#include <TranslatorPointer.h>
#include "ParamMapCommand.h"

class CEvent;

/*!
   Defines an interface for unpackers of a specific module unpacker.
*/
class CCCUSBPacket 
{
public:
  virtual int unpack(TranslatorPointer<UShort_t> p,
		     const CParamMapCommand::ParameterMap* pModuleInfo,
		     CEvent&  rEvent) = 0;

protected:
  static int bitsInMask(UShort_t mask);
};


#endif
