#ifndef __CC1205UNPACKER_H
#define __CC1205UNPACKER_H


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
#ifndef __CCCUSBPACKET_H
#include "CCCUSBPacket.h"
#endif

class CEvent;

/*!
   This class unpacks data from a CAEN C1205 QDC.   Data from this module
   consists of a header, followed by a series of data words, followed by
   an optional overflow word. completed by a separator word.  The format
   of each of these is documented in sections 3.2.7-3.2.10 of the
   CAEN 1205 module. (REV5 puts this at page 11).

*/
class CC1205Unpacker :public CCCUSBPacket
{
public:
  int  unpack(TranslatorPointer<UShort_t> p,
	      const CParamMapCommand::ParameterMap* pModuleInfo,
	      CEvent& rEvent);   
};

#endif
