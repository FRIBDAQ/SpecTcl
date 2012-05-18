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

#ifndef __CNADC2530UNPACKER_H
#define __CNADC2530UNPACKER_H

#ifndef __CMODULEUNPACKER_H
#include "CModuleUnpacker.h"
#endif



/*!
  This unpacker works on the Hytec NADC 2530 adc.  That's an 8 channel
  peak sensing adc.  These modules don't have a virtual slot number in their
  data structure.  As long as the header looks right we'll unpack them and trust
  that we've got the right one.  Therefore, users should disable zero supression
  when reading these modules.

*/
class CNADC2530Unpacker : public CModuleUnpacker
{
public:
  // Canonicals:
  
  CNADC2530Unpacker();
  virtual ~CNADC2530Unpacker();

  // The unpacker entry:

public:
  virtual  unsigned int operator()(CEvent&                       rEvent,
				   std::vector<unsigned short>&  event,
				   unsigned int                  offset,
				   CParamMapCommand::AdcMapping* pMap);


};

#endif
