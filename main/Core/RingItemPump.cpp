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
 * @file RingItemPump.cpp
 *    Implements the ring item pump methods.  
*/
#include <config.h>
#include <stddef.h>
#include <stdint.h>
#include <stdexcept>

#ifdef WITH_MPI
#include <mpi.h>
#endif
#include <tcl.h>

#include "CRingBufferDecoder.h"
#include "Globals.h"

static const size_t MAX_MESSAGE_SIZE = 16*1024;


// The internals are all not even compiled unless MPI is enabled:
#ifdef WITH_MPI


// Internal methods. Only needed if MPI is included so:


/**
 * (static) transmitChunks - 
 * Breaks up the item into 
 * send chunks that are, at most MAX_MESSAGE_SIZE and transmits
 * them to a designated receiver.  The first chunk is assumed
 * to have enough information for the receiver to sort out
 * how big the entire item is (in the case of ring items,
 * that's in the first uint32_t of the first chunk).
 * 
 * @param buf - pointer to the data.
 * @param size - # bytes in the entire item.
 * @param dest - The rank within the gMPIParallel communicator
 *   to which the data will be sent.
*/
static void
transmitChunks(const void* buf, size_t size, int dest) {
    const uint8_t *p = reinterpret_cast<const uint8_t*>(buf);
    while (size > 0) {
        // Figure out how much data to send:
        int chunksize = MAX_MESSAGE_SIZE;
        if (size < chunksize) chunksize = size;

        if (MPI_Send(p, chunksize, MPI_UINT8_T, 
            MPI_RING_ITEM_TAG, dest, gRingItemComm
        ) != MPI_SUCCESS) {
            throw std::runtime_error("Failed to send chunk of a ring item to worker");
        }
        // Figure out if there is another chunk and where it starts.

        size -= chunksize;
        p += chunksize;
    }  
}

// Internals having to do with the event pump.
// We need an event to post to the main thread:

typedef struct _RingItemEvent {
    Tcl_Event s_base;
    size_t    s_dataSize; // bytes in s_pData.
    void*     s_pData;    // allocated via Tcl_(Re)Alloc
} RingItemEvent, *pRingItemEvent;

/**
 * For each ring item in the event, calls dispatchEvent
 * for the current buffer decoder which _must_ be a 
 * CRingBufferDecoder or derived class.
 * @param pEvent - actually a pRingItemEvent  that has the
 *     data we need to pass.
 * @param flags - integer flags that we just ignore.
 * @return int  - We return 1 indicating we're done with
 * the event - we are responsible for deleting the s_pData
 * data and Tcl will delete the event storage itself.
 *
 *  @note pEvent's payload is handled like it may have
 *  more than one physics ring item. 
*/
static int
RingItemEventHandler(Tcl_Event* pEvent, int flags) {
    pRingItemEvent pInfo = reinterpret_cast<pRingItemEvent>(pEvent);
    size_t nBytes = pInfo->s_dataSize;
    union {
        uint8_t* pBytes;
        uint32_t* pLongs;
    } p;
    p.pBytes = reinterpret_cast<uint8_t*>(pInfo->s_pData);
    // Throws if it's not a CRingBufferDecoder:
    CRingBufferDecoder& rDecoder = dynamic_cast<CringBufferDecoder&>(*gpBufferDecoder);
    while (nBytes) {
        rDecoder.dispatchEvent(p.pBytes);

        nBytes   -= *p.pLongs;   // Assume size is first.
        p.pBytes += *p.pLongs;
    }

    Tcl_Free(reinterpret_cast<char*>(pInfo->s_pData);
    return 1;)
}

static void
initEvent(pRingItemEvent, pEvent) {
    pEvent->s_base.proc = RingItemEventHandler;
    pEvent->s_base.nextPtr = nullptr;
    pEvent->s_dataSize = 0;
    pEvent->s_pData    = nullptr;
}

/**
 * addembleTargetedData
 *    Assembles data that are sent to us via targeted
 * MPI_Send (Not MPI_Bcast).  At present, we assume that the
 * data consist of only a single Ringitem, however the event
 * handler makes no such assumption.  In the future, a heading
 * that describes the size of the total message may be added
 * to allow this to receive multiple ring items.
 * 
 * @param pEvent - A pRingItemEvent. This is fully filled in
 * by this method.
*/
static void
assembleTargetedData(pRingItemEvent, pEvent) {
    // Fill in the header an init the data fields:

    initEvent(pEvent);

    // The first receive is a bit special as that gives us the size:
    // It may also be the only recieve -- if the event fits in 
    // MAX_MESSAGE_SIZE.
    //
    pEvent->s_pData = Tcl_Alloc(MAX_MESSAGE_SIZE);
    if (!pEvent->s_pData) {
        throw std::runtime_error("Tcl_Alloc faile d in assembleTargetedData");
    }
    MPI_Status status;
    if (MPI_Recv
        (pEvent->s_pData, MAX_MESSAGE_SIZE, MPI_UINT8_T,
         MPI_RING_ITEM_TAG, 0, gRingItemComm, &status
        ) != MPI_SUCCESS) {
            throw std::runtime_error("Failed to receive a chunk in assembleTargetedData");
    }
    // Figure out what we got:
    
    int receivedBytes;
    MPI_Get_count(&status, MPI_UINT8_T, &receivedBytes);
    
    // How much data shoulid we get (assume we got at least the first long)
    uint32_t* pSize = reinterpret_cast<uint32_t*>(pEvent->s_pData);
    pEvent->s_dataSize = *pSize;
    uint32_t remainingSize = *pSize - receivedBytes;

    // If remaining sioze > 0, then make a big enough buffer and
    // Make a  pointer to where to read the next chunk.

    if (remainingSize > 0) {
        pEvent->s_pData = Tcl_Realloc(*pSize);
        if (!pEvent->s_pData) {
            throw std::runtime_error("Failed to alloc remaining buffer in assembleTargetedData");
        }
        uint8_t* p = reinterpret_cast<uint8_t*>(pEvent->s_pData) + receivedBytes;
        while (remainingSize) {
            if (MPI_Recv(
                p, remainingSize, MPI_UINT8_T, 
                MPI_RING_ITEM_TAG, 0, gRingItemComm, &status
            ) != MPI_SUCCESS) {
                throw std::runtime_error("Failed to receive a chunk in assembleTargetedData");
            }
            MPI_GetCount(&status, MPI_UINT8_t, &receivedBytes)
            p += receivedBytes;
            remainingSize -= receivedBytes;
        }
    }
}

/**
 *  physicsThread
 * 
 *  Thread to request and receive physics items from the
 * distributor and post events for those items to
 * the main thread which, in turn will take care of getting
 * them analyzed.
 * 
 * @param parent- actually a Tcl_ThreadId specifying the thread
 * that started us which is also the thread to which the events
 * are posted.
 * 
*/
static Tcl_ThreadCreateType
physicsThread(ClientData parent) {
    Tcl_ThreadId targetThread = reinterpret_cast<Tcl_ThreadId>(parent);

    // Our main loop is request data/get data.
    // where we defer the get data part to 
    // assembleTargetedData:

    while (1) {
        pRingItemEvent pE = reinterpret_cast<pRingItemEvent>(Tcl_Alloc(sizeof(RingItemEvent)));
        // Send our request for data:

        uint8_t dummy:
        if (!MPI_Send(
            &dummy, 1, MPI_CHAR_T, 
            MPI_RING_ITEM_TAG, 0, gRingItemComm
        ) != MPI_SUCCESS) {
            throw std::runtime_error("Worker failed to request a work item");
        }
        
        assembleTargetedData(pE);) // Fills in all of pE.

        Tcl_ThreadQueueEvent(targetThread, pE->s_base, TCL_QUEUE_TAIL);
    }
}

static Tcl_ThreadCreateType
nonPhysicsThread(ClientData parent) {

}
#endif

// implement the public entries:
// Note for now, sendRingItem does not try to block up more than one 
// item.   
// We also have a max message size for ring items of MAX_MESSAGE_SIZE.  State change
// items fit there as, I think scaler items for most experiments
// where larger ring items are sent in multiple goes and
// the receiver re-assembles the item prior to  dispatching the event.
// In the future, we may allow several ring items to live in a single message
// sent by sendRingItem.

/**
 * sendRingItem:
 *    Get a sollicitation for data from a worker and
 * transmit the data passsed into us to the requestor.  The data are a ring
 * item in our case, though actually the buffer is quite
 * generic.  This, in general, is prefferable to a round-robbin
 * distribution of data because:
 * - The whole thing continues to run in the case a worker hangs.
 * - The system is auto-load-balancing with each worker getting
 * only as much data as it can handle.
 * 
 * @param pItem - pointer to the data to send.
 * @param size  - Size of the data to send.
*/
void
sendRingItem(const void* pItem, size_t size) {
#ifdef WITH_MPI
    // Get the request.  This is just a single byte where we
    // really care about the source rank so that we know where
    // to send data:

    uint8_t dummy_request_buffer;
    MPI_Status status;
    if  (MPI_Recv(
        &dummy_request_buffer, 1, MPI_CHAR, 
        MPI_ANY_SOURCE, MPI_RING_ITEM_TAG, gRingItemComm, 
        &status
    ) != MPI_SUCCESS) {
        throw std::runtime_error("sendRingItem - failed to get request from worker");
    }
    transmitChunks(pItem, size, status.MPI_SOURCE);
#endif
}
/**
 * broadcastRingItem
 *    Ring items are broadcast over the gRingItemComm
 * communicator members when they are not physics items.
 * This allows the Buffer decoder and Analyzers of all workers
 * to update their knowledge of the run being analyzed
 * as well as scaler information.
 * 
 * @param pItem - Item to send. 
 * @param size  - # of bytes in the item to send.
 *     Since scaler items can, theoretically, be arbitrarily sized
 * (think a large DDAS system e.g) items are chunked out
 *  just as sendRingItem does...if required.  The main difference
 * is that workers don't sollicit these they get sprayed to them.
 * (the members of the RingItemComm are the transmitter and workers).
*/
void
broadcastRingItem(const void* pItem, size_t size) {
#ifdef WITH_MPI
    const uint8_t* p = reintrpret_cast<const uint8_t*>(pItem);
    while (size) {
        int chunksize = MAX_MESSAGE_SIZE;
        if (size < chunksize) chunksize = size;
        if (MPI_Bcast(p, chunksize, MPI_UINT8_T, 0, gRingItemComm) != MPI_SUCCESS) {
            throw std::runtime_error("Failed to broadcast chunk to worker");
        }
        // See if there is a next chunk and where it starts:

        size -= chunksize;
        p += chunksize;
    }
#endif
}
/**
 * startItemPump
 *    This actually starts two threads.  A thread which 
 * requests ring items sent to it via sendRingItem and a
 * thread which is alerted to broadcasts sent via broadcastRingitem
 * both need to assemble ring items from chunks.  
 * Both post the same event type to the main thread which
 * just finds the decoder - ensures it's a CRingBufferDecoder
 * and just dispatches the ring item.
*/
void 
startRingitemPump()
{
#ifdef WITH_MPI
    Tcl_ThreadId targetThread = Tcl_GetCurrentThread();
    Tcl_ThreadId requestingThread; 
    Tcl_ThreadId bcastReceiveThread;
    Tcl_CreateThread(&requestingThread, 
        physicsThread, reinterpret_cast<ClientData>(targetThread), 
        TCL_THREAD_STACK_DEFAULT, TCL_THREAD_NOFLAGS
    );
    Tcl_CreateThread(&bcastReceiveThread, 
        nonPhysicsThread, reinterpret_cast<ClientData>(targetThread), 
        TCL_THREAD_STACK_DEFAUL, TCL_THREAD_NOFLAGS
    );
#endif
}