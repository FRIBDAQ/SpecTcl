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
#include "C785Unpacker.h"
#include <Event.h>
#include <stdint.h>

using namespace std;

// Constants:

    // All data words have these bits:

static const uint32_t ALLH_TYPEMASK(0x7000000);
static const uint32_t ALLH_TYPESHIFT(24);
static const uint32_t ALLH_GEOMASK(0xf8000000);
static const uint32_t ALLH_GEOSHIFT(27);

    // High part of header.

static const uint32_t HDRH_CRATEMASK(0x00ff0000);
static const uint32_t HDRH_CRATESHIFT(16);

    // Low part of header.
static const uint32_t HDRL_COUNTMASK(0X3f00);
static const uint32_t HDRL_COUNTSHIFT(8);

    // High part of data:

static const uint32_t DATAH_CHANMASK(0x3f0000);
static const uint32_t DATAH_CHANSHIFT(16);

    // Low part of data

static const uint32_t DATAL_UNBIT(0x2000);
static const uint32_t DATAL_OVBIT(0x1000);
static const uint32_t DATAL_VBIT(0x40000);
static const uint32_t DATAL_DATAMASK(0x0fff);

    //  High part of trailer:- index in event to the first unprocessed word of the event.

static const uint32_t TRAILH_EVHIMASK(0x00ff0000);

    // Word types:

static const uint32_t HEADER(2);
static const uint32_t DATA(0);
static const uint32_t TRAILER(4);
static const uint32_t INVALID(6);



/////////////////////////////////////////////////////////////////////
// Canonicals..

/*!
  Construction is a no-op.

*/
C785Unpacker::C785Unpacker() {}

/*!
   Destruction is a no-op.
*/
C785Unpacker::~C785Unpacker() {}

//////////////////////////////////////////////////////////////////////
//  Virtual function overrides

/*!
   Perform the unpack.
   - If we are not pointing to the header corresopnding to our ADC,
     skip out without doing anything.
   - For all data words, until we see a non data word;
     extract the data -> the parameter index indicated by our parameter map.

     \param rEvent  - The event we are unpacking.
     \param event   - References the vector containing the assembled event
                      (the internal segment headers have been removed).
     \param offset  - Index in event to our chunk.
     \param pMap    - Pointer to our parameter map.  This contains our VSN and map of channel->
                      parameter id (index in rEvent).

     \return unsigned int 
     \retval offset to the first word of the event not processed by this member.


     \note - Overflow and Underflow parameters are not transferred to parameters.
     \note - the data are in little-endian form.
*/
unsigned int
C785Unpacker::operator()(CEvent&                       rEvent,
			 vector<unsigned short>&       event,
			 unsigned int                  offset,
			 CParamMapCommand::AdcMapping* pMap)
{

  // Get the 'header' .. ensure that it is one and that it matches our VSN.

  unsigned long header=  getLong(event, offset);
  if (header == 0xffffffff) {
    return offset+2;		// If immed BERR skip the BERR word and give up
  }

  int           vsn   = (header & ALLH_GEOMASK) >> ALLH_GEOSHIFT;
  if(vsn != pMap->vsn) return offset;

  // Ok this is our data:

  offset += 2;			// Next longword..

  // I've seen cases where all I get is a _trailer_.. in that case
  // we're done so skip the analysis:

  if (((header & ALLH_TYPEMASK) >> ALLH_TYPESHIFT) != TRAILER) {

    unsigned long datum   = getLong(event, offset);
    offset += 2;			// skip even if its not a data long as it's a trailer then.
    
    while (((datum & ALLH_TYPEMASK) >> ALLH_TYPESHIFT) == DATA) {
      bool underflow = (datum & DATAL_UNBIT) != 0;
      bool overflow  = (datum & DATAL_OVBIT) != 0;
      
      // Must have valid and neither of the underflow/overflow bits.
      
      if (!(overflow || underflow)) {
	// Extract channel and the data:
	
	int channel = (datum & DATAH_CHANMASK) >> DATAH_CHANSHIFT;
	int value   = datum & DATAL_DATAMASK;
	int id      = pMap->map[channel];
	if (id != -1) {
	  rEvent[id] = value;
	}      
      }
      
      datum = getLong(event, offset);
      offset += 2;
    }
    // And damned if I havn't seen duplicated trailers as well so:

    while(((datum & ALLH_TYPEMASK) >> ALLH_TYPESHIFT) == TRAILER) {
      datum = getLong(event,offset);
      offset += 2;
    }
    offset -= 2;		// Don't count the non trailer longword.
  }

  // An extra 32 bits of 0xffffffff was read if not in a chain or if at
  // end of chain:

  if (getLong(event, offset) == 0xffffffff) {
    offset += 2;
  }


  return offset;
}
