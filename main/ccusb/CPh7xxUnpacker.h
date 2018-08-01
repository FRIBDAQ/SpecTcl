#ifndef CPH7XXUNPACKER_H
#define CPH7XXUNPACKER_H

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

#include "CCCUSBPacket.h"

class CEvent;


/*!
  This class is intended to unpack data from the
  Phillips 7xxx series of digitizers.

*/
class CPh7xxxUnpacker : public CCCUSBPacket
{
  int  unpack(TranslatorPointer<UShort_t> p,
	      const CParamMapCommand::ParameterMap* pModuleInfo,
	      CEvent& rEvent);
};
#endif
