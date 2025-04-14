/*
    This software is Copyright by the Board of Trustees of Michigan
    State University (c) Copyright 2024.

    You may use this software under the terms of the GNU public license
    (GPL).  The terms of this license are described at:

     http://www.gnu.org/licenses/gpl.txt

     Author:
             Genie Jhang
	     Facility for Rare Isotope Beams
	     Michigan State University
	     East Lansing, MI 48824-1321
*/

#include "CMDPP32SCPSROSoftTriggerUnpacker.h"
#include <Event.h>
#include <stdint.h>

using namespace std;

// All longwords have a type in the top two bits:
static const uint32_t ALL_TYPEMASK(0xc0000000);
static const uint32_t ALL_TYPESHFT(30);

static const uint32_t TYPE_HEADER(1);
static const uint32_t TYPE_DATA(0);
static const uint32_t TYPE_TRAILER(3);

// Fields in the headers:
static const uint32_t HDR_IDMASK(0xff0000);
static const uint32_t HDR_IDSHFT(16);

// Fields in the data words:
static const uint32_t DATA_SUBHDRMASK  (0x30000000);
static const uint32_t DATA_CHANNEL     (0x10000000);
static const uint32_t DATA_EXTSTAMP    (0x20000000);

static const uint32_t DATA_PILEUPMASK  ( 0x1000000);
static const uint32_t DATA_PILEUPSHFT  (24);
static const uint32_t DATA_OVERFLOWMASK(  0x800000);
static const uint32_t DATA_OVERFLOWSHFT(23);
static const uint32_t DATA_CHMASK      (  0x7f0000);
static const uint32_t DATA_CHSHFT      (16);
static const uint32_t DATA_VALUEMASK   (    0xffff);
static const uint32_t DATA_EXTSTAMPMASK( 0xfffffff);
static const uint32_t DATA_EXTSTAMPSHFT(30);

// Fields in the trailer.
static const uint32_t TRAILER_COUNTMASK (0x3fffffff);

/////////////////////////////////////////////////////////////////////////////
// Canonical functions.
//

/*!
  Constrution is a no-op.
*/
CMDPP32SCPSROSoftTriggerUnpacker::CMDPP32SCPSROSoftTriggerUnpacker()
{
}

/*!
  Destruction is a no-op.
*/
CMDPP32SCPSROSoftTriggerUnpacker::~CMDPP32SCPSROSoftTriggerUnpacker()
{
}

/*!
  Perform the unpack.
  - If the offset does not 'point' to a header corresponding to our module,
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
CMDPP32SCPSROSoftTriggerUnpacker::operator()(CEvent&                       rEvent,
                               std::vector<unsigned short>&  event,
                               unsigned int                  offset,
                               CParamMapCommand::AdcMapping* pMap)
{
		uint64_t eventTimestamp = 0;

    while (1) {
        uint32_t firstItem = getLong(event, offset);

        uint32_t secondItem = getLong(event, offset + 2);

        if (firstItem == 0xffffffff && secondItem == 0xffffffff) {	// if no header, there will be just the two words of 0xffffffff
            return offset + 4;
        }

        // Get the 'header' and be sure it actually is a header and for our module id.
        uint32_t header = getLong(event, offset);
    
        uint32_t type   = (header & ALL_TYPEMASK) >> ALL_TYPESHFT;
        if (type != TYPE_DATA) { return offset; }

        int id = (header & HDR_IDMASK) >> HDR_IDSHFT;
        if (id != pMap -> vsn) { return offset; }

        offset += 2;

        int channel = 0;

        uint32_t datum = getLong(event, offset);
        if (((datum & ALL_TYPEMASK) >> ALL_TYPESHFT) == TYPE_DATA) {
            if ((datum & DATA_SUBHDRMASK) == DATA_CHANNEL) {
                channel = (header & DATA_CHMASK) >> DATA_CHSHFT;
                int value   = header & DATA_VALUEMASK;
                int id      = pMap -> map[channel];
                if (id != -1) {
                    rEvent[id] = value;
                } else {
                  cerr << __func__ << ": No matching ID for MDPP-32 SCP SRO Software Trigger data!" << endl;
                }
            }
        }

    		// extended timestamp
        offset += 2;

				uint64_t timestamp = 0;

        if ((datum & DATA_SUBHDRMASK) == DATA_EXTSTAMP) {
            uint32_t extstamp = getLong(event, offset) & DATA_EXTSTAMPMASK;
		  			timestamp = extstamp << DATA_EXTSTAMPSHFT;
        }

    		// timestamp
        offset += 2;

        if (((datum & ALL_TYPEMASK) >> ALL_TYPESHFT) == TYPE_TRAILER) {
            timestamp |= (datum & TRAILER_COUNTMASK);

            int id      = pMap -> map[channel + 32];
            if (id != -1) {
                rEvent[id] = timestamp;
            } else {
                cerr << __func__ << ": No matching ID for MDPP-32 SCP SRO Software Trigger data!" << endl;
            }

            if (eventTimestamp == 0) {
                eventTimestamp = timestamp;
                id = pMap -> map[64];
                if (id != -1) {
                    rEvent[id] = eventTimestamp;
                } else {
                    cerr << __func__ << ": No matching ID for MDPP-32 SCP SRO Software Trigger data!" << endl;
                }
            }
        }
		}
    
    // There will be a 0xffffffff longword for the BERR at the end of the
    // readout.
    return offset + 2;
}
