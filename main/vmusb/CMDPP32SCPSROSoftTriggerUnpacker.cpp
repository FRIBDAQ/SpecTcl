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

static const uint32_t TYPE_DATA(1);
static const uint32_t TYPE_RO(2);
static const uint32_t TYPE_EOE(3);

// Fields in the data words:
static const uint32_t DATA_MODIDMASK   (0x3f000000);
static const uint32_t DATA_MODIDSHFT   (24);
static const uint32_t DATA_TRIGMASK    (0x00800000);
static const uint32_t DATA_TRIGSHFT    (23);
static const uint32_t DATA_CHMASK      (0x007c0000);
static const uint32_t DATA_CHSHFT      (18);
static const uint32_t DATA_PILEUPMASK  (0x00020000);
static const uint32_t DATA_PILEUPSHFT  (17);
static const uint32_t DATA_OVERFLOW    (0x00010000);
static const uint32_t DATA_OVERFLOWSHFT(16);
static const uint32_t DATA_VALUEMASK   (0x0000ffff);

// Fields in the trailer.
static const uint32_t EOE_TIMESTAMPMASK (0x3fffffff); // timestamp

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
    while (1) {
        uint32_t vme1 = getLong(event, offset);

        if (vme1 == 0xffffffff) {	// if no header, there will be just the two words of 0xffffffff
            return offset + 2;
        }
    
    		int id      = pMap -> map[34];
        if (id != -1) {
    				rEvent[id] = vme1;
    		} else {
    				cerr << __func__ << ": No matching ID for MDPP-32 SCP SRO Software Trigger data!" << endl;
    		}

				offset += 2;

        uint32_t vme2 = getLong(event, offset);
    		id      = pMap -> map[35];
        if (id != -1) {
    				rEvent[id] = vme2;
    		} else {
    				cerr << __func__ << ": No matching ID for MDPP-32 SCP SRO Software Trigger data!" << endl;
    		}

				offset += 2;

        // Get the 'header' and be sure it actually is a header and for our module id.
        uint32_t header = getLong(event, offset);
    
        uint32_t type   = (header & ALL_TYPEMASK) >> ALL_TYPESHFT;
        if (type != TYPE_DATA) { return offset; }
    
        int modid = (header & DATA_MODIDMASK) >> DATA_MODIDSHFT;
        if (modid != pMap -> vsn) { return offset; }
    
    		int channel = (header & DATA_CHMASK) >> DATA_CHSHFT;
    		int value   = header & DATA_VALUEMASK;
    		id      = pMap -> map[channel];
        if (id != -1) {
    				rEvent[id] = value;
    		} else {
    				cerr << __func__ << ": No matching ID for MDPP-32 SCP SRO Software Trigger data!" << endl;
    		}
    
    		// zeropad part
        offset += 2;

				uint32_t zeropad = getLong(event, offset);
				if (zeropad != 0) {
    				cerr << __func__ << ": Impossible things happening! Are you sure it's MDPP-32 SCP SRO Software Trigger data?!" << endl;

						return offset + 2;
				}

				// rollover counter (32 MSB)
				offset += 2;
				
				uint32_t rolloverCounter = getLong(event, offset);
				type = (rolloverCounter & ALL_TYPEMASK) >> ALL_TYPESHFT;
				if (type != TYPE_RO) {
    				cerr << __func__ << ": Impossible things happening! Are you sure it's MDPP-32 SCP SRO Software Trigger data?!" << endl;

						return offset + 2;
				}

        uint32_t rolloverCount = rolloverCounter & EOE_TIMESTAMPMASK;
        id = pMap -> map[32];
        if (id != -1) {
            rEvent[id] = rolloverCount;
        } else {
    				cerr << __func__ << ": No matching ID for MDPP-32 SCP SRO timestamp!" << endl;
    		}

				offset += 2;
    
        uint32_t trailer = getLong(event, offset);
        type = (trailer & ALL_TYPEMASK) >> ALL_TYPESHFT;
        if (type != TYPE_EOE) {
    				cerr << __func__ << ": Impossible things happening! MDPP-32 SCP SRO roll over counter is not followed by timestamp! (0x" << hex << trailer << dec << ")" << endl;
    			  return offset;
    		}
    
        // Timestamp is stored in 33th element
        uint64_t timestamp = trailer & EOE_TIMESTAMPMASK;
        id = pMap -> map[33];
        if (id != -1) {
            rEvent[id] = timestamp;
        } else {
    				cerr << __func__ << ": No matching ID for MDPP-32 SCP SRO timestamp!" << endl;
    		}

				offset += 2;
		}
    
    // There will be a 0xffffffff longword for the BERR at the end of the
    // readout.
    return offset + 2;
}
