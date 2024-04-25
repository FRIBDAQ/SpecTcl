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

#include "CVMUSBScalerUnpacker.h"
#include <Event.h>
#include <stdint.h>

using namespace std;

/////////////////////////////////////////////////////////////////////////////
// Canonical functions.
//

/*!
  Constrution is a no-op.
*/
CVMUSBScalerUnpacker::CVMUSBScalerUnpacker()
{
}

/*!
  Destruction is a no-op.
*/
CVMUSBScalerUnpacker::~CVMUSBScalerUnpacker()
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
CVMUSBScalerUnpacker::operator()(CEvent&                       rEvent,
                               std::vector<unsigned short>&  event,
                               unsigned int                  offset,
                               CParamMapCommand::AdcMapping* pMap)
{
    uint32_t scaler = getLong(event, offset);
		offset += 2;

		int id = pMap -> map[0];
		if (id != -1) {
			rEvent[id] = scaler;
		}

    scaler = getLong(event, offset);
		offset += 2;

		id = pMap -> map[1];
		if (id != -1) {
			rEvent[id] = scaler;
		}

    return offset;
}
