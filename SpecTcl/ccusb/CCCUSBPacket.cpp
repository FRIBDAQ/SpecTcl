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
#include "CCCUSBPacket.h"


/*
  The table below is the number of bits set in a 4 bit mask.
  e.g. bitCount[5] = 2 because 5 = 101b.

  this is used to speed up the couting of bits in a 16 bit mask by
  turning it in to 4 table lookups rather than 16 bit checks.

*/

static int bitCount[16] = {
  0,                            // 0000
  1,                            // 0001
  1,                            // 0010
  2,                            // 0011
  1,                            // 0100
  2,                            // 0101
  2,                            // 0110
  3,                            // 0111
  1,                            // 1000
  2,                            // 1001
  2,                            // 1010
  3,                            // 1011
  2,                            // 1100
  3,                            // 1101
  3,                            // 1110
  4 };                          // 1111

////////////////////////////////////////////////////////////////////////////////////
/*
   Returns the number of bits in 16 bit mask that is passed into us.
   We use the bitCount array to speed this up to 4 table lookups from
   16 bit tests.
   Parameters:
      mask   - The mask in which to count bits.
*/
int
CCCUSBPacket::bitsInMask(UShort_t mask)
{
  int count = 0;

  // The loop below is faster than a counted loop because
  // - there's no increment, and we have to shift the mask anyway.
  // - We'll short cut out of the loop in there are upper nybbles that don't
  //   have bits set.
  //
  while(mask) {
    count += bitCount[mask & 0xf];  // Count lowest 4 bits.
    mask   = mask >> 4;		    // Next 4 bits.
  }
  return count;
}
