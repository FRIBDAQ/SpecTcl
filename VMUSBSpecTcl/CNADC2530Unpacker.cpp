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

static const uint32_t TYPE_MASK     (0x0f000000);
static const uint32_t CHAN_MASK     (0x00070000);
static const uint32_t CHAN_SHIFT    (16);
static const uint32_t VALUE_MASK    (0x00001fff);
static const uint32_t TYPE_HEADER   (0x02000000);
static const uint32_t TYPE_DATA     (0x00000000);
static const uint32_t TYPE_END      (0x04000000);
static const uint32_t TYPE_TSHIGH   (0x05000000);
static const uint32_t TYPE_TSLOW_WRONG    (0x0c000000);
static const uint32_t TYPE_TSLOW    (0x06000000);
static const uint32_t TIMESTAMP_MASK(0x00ffffff);
static const uint32_t TIMESTAMP_SHIFT(24);




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

   - The NADC data is wrapped in a three word header that's the reverse of a packet:
     first word is a virtual slot number 
     the second word is the VM-USB ND mask
     the third word is the VM-USB ND (low order contents of NADC address count register).
     of data items.  This is used to unpack the data. 
   - The NADC header and trailer are completely ignored, but the header is assumed
     to be present (skipped explicitly).
   - Unpacking stops when either a trailer is hit or the number of data words
     is completely used up.
   - Parameter offset 0 is always a 48 bit timestamp

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
  // Match our virtual slot number:

  uint16_t id = event[offset];
  if (id != pMap->vsn) {
    return offset;		// Not our data
  }
  offset++;

  // Next is the NDMask and ND.
  // after the word count:

  uint16_t sizeMask   = event[offset];
  offset++;
  uint16_t packetSize = event[offset];
  offset++;
  packetSize = packetSize & sizeMask; // number of subsequent longs:

  unsigned int nextPacket = offset + packetSize * sizeof(uint32_t)/sizeof(uint16_t);


  uint64_t  timestamp(0);
  uint32_t  tsLow;
  uint32_t  tsHigh;
  bool      foundTrailer(false);

  // So we've used up 6 words of the event; the remainder are either
  // adc data words or a trailer.  Note the ADC is a multihit
  // adc.  This unpacker is built for spectroscopy appplications and therefore
  // takes the largest energy in each channel:

  while (packetSize && !foundTrailer) {
    uint32_t datum = getLong(event, offset);
    offset        += 2;
    packetSize--;

    uint32_t type = datum & TYPE_MASK;

    // We're interested in timestamp 1/2's the
    // channel data and the 
    // trailer

    switch(type) {

    case TYPE_DATA:
      {
	uint32_t adc    = datum & VALUE_MASK;
	uint32_t chan   = (datum & CHAN_MASK) >> CHAN_SHIFT;
	chan++; 			// The channels start at map[1].
	int param       = pMap->map[chan];
	if (param != -1) {	// It's mapped.
	  if (!rEvent[param].isValid()     ||
	      (rEvent[param] < adc)) { // Not yet set or we have a bigger adc value:
	    rEvent[param] = adc;
	  }
	}
      }
      break;

    case TYPE_TSHIGH:		// high 1/2 of timestamp is first.
      tsHigh = datum & TIMESTAMP_MASK;
      break;

    case TYPE_TSLOW:		// now we have both timestamp 1/2s:
    case TYPE_TSLOW_WRONG:	// Until firmware gates the right tag.
      tsLow = datum & TIMESTAMP_MASK;
      timestamp  = tsHigh;
      timestamp  = timestamp << TIMESTAMP_SHIFT;
      timestamp |= tsLow;
      
      // Stuff parameter 0 wioth timestamp if it's defined
      
      if (pMap->map[0] != -1) {
	rEvent[pMap->map[0]] = timestamp;
      }
      break;

    case TYPE_END:		// End loop
      foundTrailer = true;
      break;
    default:
      break;
    }

  }

  return nextPacket;		// Pointing to the next chunklet of data.
}



