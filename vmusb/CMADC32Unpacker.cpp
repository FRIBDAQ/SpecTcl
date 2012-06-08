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
#include "CMADC32Unpacker.h"
#include <Event.h>
#include <stdint.h>

using namespace std;

// Constants

// All longwords have a type in the top two bits:

static const uint32_t ALL_TYPEMASK(0xc0000000);
static const uint32_t ALL_TYPESHFT(30);

static const uint32_t TYPE_HEADER(1);
static const uint32_t TYPE_DATA(0);
static const uint32_t TYPE_TRAILER(3);

// Fields in the headers:

static const uint32_t HDR_COUNTMASK(0x7ff);

static const uint32_t HDR_ERRORMASK(0x800); // Buffer overflow.
static const uint32_t HDR_ERRORSHFT(12);

static const uint32_t HDR_IDMASK(0xff0000);
static const uint32_t HDR_IDSHFT(16);

// Fields in the data words:

static const uint32_t DATA_VALUEMASK(0x1fff);

static const uint32_t DATA_ISOVERFLOW(0x4000);

static const uint32_t DATA_CHANNELMASK(0x1f0000);
static const uint32_t DATA_CHANNELSHFT(16);

static const uint32_t DATA_ISPAD(0x04000000);

// Fields in the trailer.

static const uint32_t TRAILER_COUNTMASK(0x3fffffff); // trigger count or timestamp counter.

/////////////////////////////////////////////////////////////////////////////
// Canonical functions.
//

/*!
  Constrution is a no-op.
*/
CMADC32Unpacker::CMADC32Unpacker()
{
}

/*!
  Destruction is a no-op.
*/
CMADC32Unpacker::~CMADC32Unpacker()
{
}

///////////////////////////////////////////////////////////////////////////////////////
//  Virtual function overrides.

/*!
  Perform the unpack.  
  - If the offset does not 'point' to a header corresponding to our ADC,
    return without advancing the offset.
  - For all data words, extract the channel number and parameter value until we
    see a non-data word. For now we suppress overflows.
  - For the trailer, the count field goes in parameter number 32 (numbered from 0).

     \param rEvent  - The event we are unpacking.
     \param event   - References the vector containing the assembled event
                      (the internal segment headers have been removed).
     \param offset  - Index in event to our chunk.
     \param pMap    - Pointer to our parameter map.  This contains our VSN and map of channel->
                      parameter id (index in rEvent).

     \return unsigned int 
     \retval offset to the first word of the event not processed by this member.


     \note - the data are in little-endian form.
     \note - in single event mode, buffer overflows are not possible so we ignore the
             header error flag.
*/
unsigned int 
CMADC32Unpacker::operator()(CEvent&                       rEvent,
			    vector<unsigned short>&       event,
			    unsigned int                  offset,
			    CParamMapCommand::AdcMapping* pMap)
{

  // For now, the MADC and VM-usb are not cooperating on 
  // ending the event on BERR.  For each BERR 'transfer' the
  // VM-USB puts a 16 bit word of 0xffff in the event.
  // we need to ensure that we'll skip these.
  //
  // Get the 'header' and be sure it actually is a header and for our module id.

  uint32_t header = getLong(event, offset);

  if (header == 0xffffffff) {	// ADC had no data there will be just the two words of 0xffffffff
    return offset + 2;
  }


  uint32_t      type   = (header &  ALL_TYPEMASK) >> ALL_TYPESHFT;
  if (type != TYPE_HEADER) return offset;

  int longsRead = 1;		// Count the longwords processed:

  int           id     = (header & HDR_IDMASK) >> HDR_IDSHFT;
  if (id != pMap->vsn) return offset;

  // We've established this is our data.
  // We're going to use the trailer to terminate so we don't need the
  // conversion count field of the header.

  offset += 2;
  unsigned long datum = getLong(event, offset);
  longsRead++;
  offset += 2;
  while (((datum & ALL_TYPEMASK) >> ALL_TYPESHFT) == TYPE_DATA) {
    bool overflow = (datum & DATA_ISOVERFLOW) != 0;
    if (!overflow) {
      int channel = (datum & DATA_CHANNELMASK) >> DATA_CHANNELSHFT;
      int value   = datum & DATA_VALUEMASK;
      int id      = pMap->map[channel];
      if (id != -1) {
	rEvent[id] = value;
      }
    }
    datum   = getLong(event, offset);
    longsRead++;
    offset += 2;
  }
  // The datum should be the trailer.. verify this.. If so,
  // then save the count field ans parameter 32.

  if (((datum & ALL_TYPEMASK) >> ALL_TYPESHFT) == TYPE_TRAILER) {
    uint32_t value = datum & TRAILER_COUNTMASK;
    int      id    = pMap->map[32];
    if (id != -1) {
      rEvent[id] = value;
    }
  }
  else {
    longsRead--;		// Really should not happen!!
  }
    
  // There will be a 0xffffffff longword for the BERR at the end of the
  // readout.

  return offset + 2;
}
