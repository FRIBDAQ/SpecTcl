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
#include "CPh7xxUnpacker.h"
#include <stdint.h>
#include <vector>
#include "Event.h"

using namespace std;

// Simple utility functions (macros)

// Channel field of the data:

static inline int Channel(uint16_t datum)
{
  return (datum >> 12) & 0xf;
}

// Conversion field of the data:

static inline int Value(uint16_t datum)
{
  return (datum & 0xfff);
}

/*!
   Unpack data from a Ph7xxx module.  The data packet from this module
   is assumed to be of the form:
   
\verbatim
    +----------------------+
    | Packet id            |
    +----------------------+
    | hit pattern          |
    +----------------------+
    |  channel data...     |
    ...                   ...
    +----------------------+

\endverbatim
Assumptions:
  - The module is configured to read the hit pattern.
  - The module is read zero suppressed.
  - The caller has verified the id is correct for us.

  @param p           - Byte order translating pointer to the raw event.
  @param pModuleInfo - Pointer to the module info structure.
                       this includes the channel->parameter map.
  @param rEvent      - Reference to the unpacked paramter vector.
  @return int
  @retval Number of words of data consumed by this unpacker.
*/
int
CPh7xxxUnpacker:: unpack(TranslatorPointer<UShort_t> p,
			 const CParamMapCommand::ParameterMap* pModuleInfo,
			 CEvent& rEvent)
{
  ++p; 				 // Skip over the id.
  uint16_t hitPattern = *p; ++p; // Use the hit pattern to get the data count:
  uint16_t channels   = bitsInMask(hitPattern);

  const vector<int>& parameters   = pModuleInfo->s_parameterIds;

  for (int i =0; i < channels; i++) {
    uint16_t datum = *p; ++p;
    int      channel = Channel(datum);
    if (parameters.size() < channel) {
      int param =   parameters[channel];
      if (param >= 0) {
	rEvent[param] = Value(datum);
      }

    }
  }

  // we consumed the id the bit mask (2) and channels words:

  return 2 + channels;
}
