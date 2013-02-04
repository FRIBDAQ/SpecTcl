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

#ifndef __C785UNPACKER_H
#define __C785UNPACKER_H

#ifndef __CMODULEUNPACKER_H
#include "CModuleUnpacker.h"
#endif



/*!
  This unpacker is responsible for unpacking CAEN 32 channel digitizers.  These modules
  include the CAEN V775, 785, 792, and 862 modules.  These modules have a 
  virtual slot number.  Furthermore it is possible that the readout of a specific
  module may be completely supressed.  Fortunately the header of a module is
  quite unambiguous and this unpacker can deal with that case just fine.
*/
class C785Unpacker : public CModuleUnpacker
{
public:
  // Canonicals:

  C785Unpacker();
  virtual ~C785Unpacker();

  // The unpacker entry:

public:
  virtual unsigned int operator()(CEvent&                       rEvent,
				  std::vector<unsigned short>&  event,
				  unsigned int                  offset,
				  CParamMapCommand::AdcMapping* pMap);


};

#endif
