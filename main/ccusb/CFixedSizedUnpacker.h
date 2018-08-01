#ifndef CFIXEDSIZEDUNPACKER_H
#define CFIXEDSIZEDUNPACKER_H

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

#ifndef CCCUSBPACKET_H
#include "CCCUSBPacket.h"
#endif

class CEvent;

/*!
   This class will work with a wide variety of module types.
   It assumes that the data from a module is a fixed sized block of channels
   read in order from channel 0 -> channel n
*/


class CFixedSizedUnpacker : public CCCUSBPacket
{
private:
  int     m_numWords;

public:
  CFixedSizedUnpacker(int numWords);
  int  unpack(TranslatorPointer<UShort_t> p,
	      const CParamMapCommand::ParameterMap* pModuleInfo,
	      CEvent& rEvent);  
};

#endif
