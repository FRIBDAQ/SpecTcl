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

#ifndef __CMADC32UNPACKER_H
#define __CMADC32UNPACKER_H

#ifndef __CMODULEUNPACKER_H
#include "CModuleUnpacker.h"
#endif


/*!
   This module is responsible for unpacking the Mesytec MADC-32 32 channel ADC.
   These modules have a virtual slot number.  It is therefore possible to 
   to do perfect matching on the data stream.. and to deal with complete supression
   of the module in the data stream.

   One wrinkle is that the module actually recognized 33 channels.  Channel 0-31
   are the conversions of the various channels of the ADC module.  Channel 32 is
   the timestamp/event-number.

   this allows the user to create strip charts of something vs time (e.g. counts of
   hits in some place)
*/
class CMADC32Unpacker : public CModuleUnpacker
{
  // canonicals:
public:
  CMADC32Unpacker();
  virtual ~CMADC32Unpacker();

  // The unpacker entry:

public:
  virtual unsigned int operator()(CEvent&                        rEvent,
				  std::vector<unsigned short>&   event,
				  unsigned int                   offset,
				  CParamMapCommand::AdcMapping*  pMap);


  
};


#endif
