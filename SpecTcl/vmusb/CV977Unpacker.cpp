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
#include <config.h>
#include "CV977Unpacker.h"
#include <Event.h>
#include <stdint.h>

using namespace std;

///////////////////////////////////////////////////////////////////////////
// Canonicals

/*!
   Construction is a no-op.
*/
CV977Unpacker::CV977Unpacker()
{}

/*!
  Destruction is a no-op.
*/
CV977Unpacker::~CV977Unpacker()
{}

////////////////////////////////////////////////////////////////////////
// Virtual function overrides.

/*!
   Perform the unpack.  The module unconditionally adds a single
   16 bit word to the data stream.
   @param rEvent   - The event we are unpacking into.
   @param event    - Vector containing the raw data for this event from the VM-USB
   @param offset   - Index into the event vector at which we'll find the
                     next parameter.
   @param pMap     - Pointer to our parameter map.  This contains the map
                     that tells us which element of rEvent to stuff.
   @return unsigned int
   @retval offset to the first word of the event not processed by us.
*/
unsigned int
CV977Unpacker::operator()(CEvent&                     rEvent,
			  vector<unsigned short>&     event,
			  unsigned int                offset,
			  CParamMapCommand::AdcMapping* pMap)
{
  uint16_t  datum = event[offset];
  int       id    = pMap->map[0];
  if (id != -1) {
    rEvent[id] = datum;
  }
  offset += 1;
  return offset;
  
  
}
  
