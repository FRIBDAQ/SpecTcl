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
#include "CC1205Unpacker.h"
#include <Event.h>
#include <vector>

using namespace std;

/*
 * Constants that define the bits in each type of word that can apear
 * in the data:
 */

static const uint32_t dataMask(0xffffff); // 24 bit mask.

static const uint32_t typeMask    (0xc00000);   // Type of datum is here.
static const uint32_t typeHeader  (0x800000);   // Word is header.
static const uint32_t typeData    (0x000000);   // Word is parameter data.
static const uint32_t typeOverflow(0xc00000);   // Word has overflow mask.
static const uint32_t typeTrailer (0x400000);   // Word is a trailer.

// Header fields/values:

static const uint32_t headerCSRMask  (0x007fff);
static const uint32_t headerEvNumMask(0x0f0000); 

// Data word format:

static const uint32_t dataValueMask(0x003fff); // Data value mask
static const uint32_t rangeMask    (0x00c000);
static const uint32_t rangeLow     (0x000000);
static const uint32_t rangeMid     (0x004000);
static const uint32_t rangeHigh    (0x008000);
static const uint32_t rangeShift   (14);
static const uint32_t rangeOverflow(0x00c000);
static const uint32_t channelMask  (0x0f0000);
static const uint32_t channelShift (16);

// Overflow word:

static const uint32_t overflowFlags(0x00ffff);


// trailer:

static const uint32_t trailerOnes(0x0000ff);



/*!
  Unpack a block of data that looks like:
\verbatim
  +------------------------------------------+
  |                 id                       |
  +------------------------------------------+
  |   Header                                 |
  +------------------------------------------+
  |   data words....                         |
  
  +------------------------------------------+
  | Optional overflow word                   |
  +------------------------------------------+
  |  Separator word                          |
  +------------------------------------------+

\endverbatim

  @param p  - Translator pointer that gets us at the data
              (the id is presumed to have been previously validated).
  @param pModuleInfo - Pointer to the module info (which includes the
               channel-> parameter map.
  @param rEvent - Reference to the channel -> parametr map.
  @return int
  @retval Number of words actually consumed (1 + m_numWords)

*/
int
CC1205Unpacker::unpack(TranslatorPointer<UShort_t> p,
			    const CParamMapCommand::ParameterMap* pModuleInfo,
			    CEvent& rEvent)
{
  ++p;				// Caller has already checked the marker word.
  const vector<int>& parameterMap = pModuleInfo->s_parameterIds;
  int numWords = 1;		// Number of words in packet.
  TranslatorPointer<uint32_t> pEvent(p); // All the words I care about are 32 bit

  uint32_t datum;
  do {
    datum = *pEvent; ++pEvent;
    
    // We're only going to pay attention to the data words for now:

    uint32_t type = datum & typeMask;
    if (type == typeData) {
      // Extract the channel number and the range.  
      // 4 (overflow) is ignored.
      // channel *3 is the starting point in the channel array
      // add the range to that to get the actual parameter:

      uint32_t channel = (datum & channelMask) >> channelShift;
      uint32_t range   = (datum & rangeMask);
      if (range != rangeOverflow) {
	int paramIndex = channel * 3;
	paramIndex     += (range >> rangeShift); // Parameter index.
	if (paramIndex < parameterMap.size()) {
	  int parno = parameterMap[paramIndex];
	  if (parno >= 0) {
	    uint32_t conversion = datum & dataValueMask;
	    rEvent[parno] = conversion;
	  }
	}
      }
    }
    numWords++;
  } while ((datum & typeMask) != typeTrailer);
  
  return numWords;

}

