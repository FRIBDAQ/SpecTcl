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


// Implementation of the non-pure members of the module unpacker.

#include <config.h>
#include "CModuleUnpacker.h"


CModuleUnpacker::~CModuleUnpacker()
{}



// unpack a longword from the event array...assumption is that localhost is
// little endian.

unsigned long
CModuleUnpacker::getLong(std::vector<unsigned short>& event, 
			 unsigned int offset)
{
  unsigned long low = event[offset];
  unsigned long hi  = event[offset+1];


  return low | (hi << 16);
}
