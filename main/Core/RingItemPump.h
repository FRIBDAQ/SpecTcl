/*
    This software is Copyright by the Board of Trustees of Michigan
    State University (c) Copyright 2008

    You may use this software under the terms of the GNU public license
    (GPL).  The terms of this license are described at:

     http://www.gnu.org/licenses/gpl.txt

     Author:
             Ron Fox
	     NSCL
	     Michigan State University
	     East Lansing, MI 48824-1321
*/
/**
 *  @file RingItemPump.h
 *    This header defines the entry points to the ring item pump.
 * The ring item pump is actually two pumps- one for individually  sent items
 * and one for broadcast items.  
 * 
 * As with all pumps, the idea is that each item received is turned into
 * a Tcl event which is dispatched into the main thread.  In our case, the
 * dispatched event, just calls CRingBufferDecoder::dispatchEvent.  Since
 * the mpiSpecTcl only supports the ring bufferr decoder branch we know
 * that this works.
 *   Note that buffer decoder helper changes are sent as artificial RING_FORMAT items.
 * so that they will resuolt in the correct handling of future items.
*/
#ifndef RINGITEMPUMP_H
#define RINGITEMPUMP_H
#include <stdlib.h>
// Event sending:

void sendRingItem(const void* pItem, size_t size);
void broadcastRingItem(const void* pItem, size_t size);

void startRingItemPump();
void stopRingItemPump();
#endif