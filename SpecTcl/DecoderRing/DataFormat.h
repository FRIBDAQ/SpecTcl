#ifndef __DATAFORMAT_H
#define __DATAFORMAT_H

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

/*!
  \file DataFormat.h
  This file contains typedefs for the structures that will be put into
  ring buffers for event data.  Event data Ring buffers are filled with items.
  An item has the structure:

\verbatim
 +----------------------------------+
 |  Size of item in bytes (32 bits) |
 +----------------------------------+
 |  32 bit type code of item        |
 +----------------------------------+
 |  body (size - 8 bytes of data    |
 +----------------------------------+
\endverbatim

  Where the 32 bit type code is really a 16 bit type code stored in the lower 16 bits of the 
32 bit word in the native byte ordering of the originating system.  This allows it to serve as 
a byte order indicator, as data type 0 is not legal, and the top bits of the type code must
be zero.

*/

/*
   Define the type codes for the items.
   Applications can add user specific types if they use values that are at least
   FIRST_USER_TYPE
*/


#ifndef __CRT_STDINT_H
#include <stdint.h>
#ifndef __CRT_STDINT_H
#define __CRT_STDINT_H
#endif
#endif

#ifndef __CRT_TIME_H
#include <time.h>
#ifndef __CRT_TIME_H
#define __CRT_TIME_H
#endif
#endif

// state change item type codes:

static const uint32_t BEGIN_RUN(1);
static const uint32_t END_RUN(2);
static const uint32_t PAUSE_RUN(3);
static const uint32_t RESUME_RUN(4);

// Documentation item type codes:

static const uint32_t PACKET_TYPES(10);
static const uint32_t MONITORED_VARIABLES(11);

// Scaler data:

static const uint32_t INCREMENTAL_SCALERS(20);

// Physics events:

static const uint32_t PHYSICS_EVENT(30);
static const uint32_t PHYSICS_EVENT_COUNT(31);

// User defined item codes

static const uint32_t FIRST_USER_ITEM_CODE(32768); /* 0x8000 */


// Longest allowed title:

#ifndef TITLE_MAXSIZE
#define TITLE_MAXSIZE 80
#endif



/*!  All ring items have common header structures: */

typedef struct _RingItemHeader {
  uint32_t     s_size;
  uint32_t     s_type;
} RingItemHeader, *pRingItemHeader;

/*!
  This  is the most basic item.. a generic item.  It consists only of the
  header and a generic body
*/

typedef struct _RingItem {
  RingItemHeader s_header;
  uint8_t        s_body[1];
} RingItem, *pRingItem;


/*!
  Run state changes are documented by inserting state change items that have the
  structure shown below:

*/
typedef struct _StateChangeItem {
  RingItemHeader  s_header;
  uint32_t        s_runNumber;
  uint32_t        s_timeOffset;
  time_t          s_Timestamp;
  char            s_title[TITLE_MAXSIZE+1];
} StateChangeItem, *pStateChangeItem;

/*!
   Scaler items contain run time counters.
*/

typedef struct _ScalerItem {
  RingItemHeader  s_header;
  uint32_t        s_intervalStartOffset;
  uint32_t        s_intervalEndOffset;
  time_t          s_timestamp;
  uint32_t        s_scalerCount;
  uint32_t        s_scalers[1];
} ScalerItem, *pScalerItem;

/*!
  The various documentation Events are just a bunch of null terminated strings that
  are back to back in the body of the ring buffer. item.
*/

typedef struct _TextItem {
  RingItemHeader s_header;
  uint32_t       s_timeOffset;
  time_t         s_timestamp;
  uint32_t       s_stringCount;
  char           s_strings[1];
} TextItem, *pTextItem;

/*!
  For now a physics event is just a header and a body of uint16_t's.
*/

typedef struct _PhysicsEventItem {
  RingItemHeader s_header;
  uint16_t       s_body[1];
} PhysicsEventItem, *pPhysicsEventItem;

/*!
   Clients that sample physics events will often
   need to know how many physics events have been produced
   so that they can figure out the sampling fraction.
*/
typedef struct __PhysicsEventCountItem {
  RingItemHeader s_header;
  uint32_t       s_timeOffset;
  time_t         s_timestamp;
  uint64_t       s_eventCount;	/* Maybe 4Gevents is too small ;-) */
} PhysicsEventCountItem, *pPhysicsEventCountItem;


#endif
