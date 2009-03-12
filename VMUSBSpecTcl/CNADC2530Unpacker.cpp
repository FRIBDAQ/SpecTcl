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
#include "CNADC2530Unpacker.h"
#include <Event.h>
#include <stdint.h>

using namespace std;

// Constants:

static const uint32_t TYPE_MASK  (0x07000000);
static const uint32_t CHAN_MASK  (0x00070000);
static const uint32_t CHAN_SHIFT (16);
static const uint32_t VALUE_MASK (0x00001fff);
static const uint32_t TYPE_HEADER(0x02000000);
static const uint32_t TYPE_DATA  (0x00000000);
static const uint32_t TYPE_END   (0x04000000);




//////////////////////////////////////////////////////////////////////
// Canonical members.

/*! 
  Constructor is a no-op.

*/
CNADC2530Unpacker::CNADC2530Unpacker()
{}

/*! 
  As is the destructor.
*/
CNADC2530Unpacker::~CNADC2530Unpacker()
{}

//////////////////////////////////////////////////////////////////////
//  The unpacker function.

/*!
   Unpack a module.  We don't have virtual slot so if the data type field matches
   that of a NADC 2530 header, we'll unpack it as described by our parameter map.
   - We'll use the trailer to stop unpacking rather than looking at the 
     module channel count.

     \param rEvent  - The event we are unpacking.
     \param event   - References the vector containing the assembled event
                      (the internal segment headers have been removed).
     \param offset  - Index in event to our chunk.
     \param pMap    - Pointer to our parameter map.  This contains our VSN and map of channel->
                      parameter id (index in rEvent).

     \return unsigned int 
     \retval offset to the first word of the event not processed by this member.

     \note - the data are in little endian form.

*/
unsigned int
CNADC2530Unpacker::operator()(CEvent&                       rEvent,
			      vector<unsigned short>&       event,
			      unsigned int                  offset,
			      CParamMapCommand::AdcMapping* pMap)
{
  uint32_t header = getLong(event, offset);
  if ((header & TYPE_MASK) != TYPE_HEADER) {
    return offset;
  }

  offset += 2;

  uint32_t datum = getLong(event, offset);
  offset += 2;
  while ((datum & TYPE_MASK) == TYPE_DATA) {

    uint32_t chan  = (datum & CHAN_MASK) >> CHAN_SHIFT;
    uint32_t value = (datum & VALUE_MASK);

    int id = pMap->map[chan];
    if (id != -1) {
      rEvent[id] = value;
    }

    datum = getLong(event, offset);
    offset += 2;
  }
  // There's an extra 16 bits on the end of all this:

  
  return offset+1;
}



