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
#include <string.h>
#include <stdexcept>
#include <iostream>
#ifdef WITH_MPI
#include <mpi.h>
#endif
#include <tcl.h>
#include <TclPump.h>
#include "CRingBufferDecoder.h"
#include "Globals.h"
#include "DataFormat.h"
#include "TCLAnalyzer.h"
#include "SpecTcl.h"
#include <vector>
#include <stdlib.h>
#include <TCLInterpreter.h>
#include <TCLVariable.h>

static const size_t MAX_MESSAGE_SIZE = 16*1024;


// The internals are all not even compiled unless MPI is enabled:
#ifdef WITH_MPI
static void
transmitChunks(const void* buf, size_t size, int dest);  // forward reference

class BufferedItems {   // So that we can resize the buffer
private:                         // we use realloc not new.
    unsigned m_maxItems;         // Max item count
    unsigned m_numItems;         // No. items now buffered.
    CTCLInterpreter* m_pInterp;
    std::vector<uint8_t> m_buffer; // the buffer.
 public:
    BufferedItems(CTCLInterpreter* pInterp);
    ~BufferedItems();
    void flush();
    void buffer(const void* pData, size_t size);
private:
    int getDestination();
    void updateMaxItems();
};
typedef BufferedItems *pBufferedItems;

/// Implementation of the bufffered items class.

/**
 *  construct a BufferedItems object:
 * @param pInterp - pointer to the interp used to update max items.
 */
BufferedItems::BufferedItems(CTCLInterpreter* pInterp) :
    m_maxItems(0), m_numItems(0), m_pInterp(pInterp) 
{
    updateMaxItems();
}
/**
 * destructor - flush data first.
 */
BufferedItems::~BufferedItems() {
    flush();
}

/**
 * Flush the current items to a destination - this is a no-op if the buffer
 * is empty.
 */
void 
BufferedItems::flush() {
    if (m_numItems) {
        transmitChunks(m_buffer.data(), m_buffer.size(), getDestination());

        // Get ready for the next buffer.
        m_buffer.clear();
        m_numItems = 0;
    }
}
/** buffer
 *    Buffer a ring item for transmission and, if that exceeds the 
 * maximum number of items, flushes.
 * 
 * @param pData - Data to send
 * @param size - number of bytes to buffer.
 * @note if there are no items buffered, we update the maximum chunk count.
 */
void
BufferedItems::buffer(const void* pData, size_t size) {
    if (!m_numItems) {
        updateMaxItems();
    }
    // Todo the insert we need a pair of iterator like thinks for uint8_t:

    const uint8_t* pBegin = reinterpret_cast<const uint8_t*>(pData);
    const uint8_t* pEnd = pBegin + size;
    m_buffer.insert(m_buffer.end(), pBegin, pEnd);
    m_numItems++;

    // If we reached/exceded the threshold flush

    if (m_numItems >= m_maxItems) {
        flush();
    }
}
/** getDestination
 *      Read the next queued data request
 * 
 * @return int - Rank to which to send the next chunks:
 */
int
BufferedItems::getDestination() {
    uint8_t dummy_request_buffer;
    MPI_Status status;
    if  (MPI_Recv(
        &dummy_request_buffer, 1, MPI_CHAR, 
        MPI_ANY_SOURCE, MPI_RING_ITEM_TAG, gRingItemComm, 
        &status
    ) != MPI_SUCCESS) {
        throw std::runtime_error("sendRingItem - failed to get request from worker");
    }
    return status.MPI_SOURCE;
}
/**
 * updateMaxItems
 *    Read the WORKER_CHUNKSIZE_VAR variable special cases:
 * 
 * *  There is no  variable (should not happen) use DEFAULT_MAX_CHUNK_SIZE instead.
 * *  atoi() applied to the value <= 0, apply it to DEFAULT_MAX_CHUNK_SIZE.
 */
void 
BufferedItems::updateMaxItems() {
    CTCLVariable var(m_pInterp, WORKER_CHUNKSIZE_VAR, TCLPLUS::kfFALSE);
    const char* pValue = var.Get();
    if (!pValue) pValue = DEFAULT_MAX_CHUNK_SIZE;

    int result = atoi(pValue);
    if (result <= 0) {
        result = atoi(DEFAULT_MAX_CHUNK_SIZE);
    }
    m_maxItems = result;
}

// The buffer pointer.
static pBufferedItems pBuffer(0);

// Funky singleton getter.

static pBufferedItems getBuffer() {
    if (!pBuffer) {
        pBuffer = new BufferedItems(SpecTcl::getInstance()->getInterpreter());
    }
    return pBuffer;
}


// Internal methods. Only needed if MPI is included so:

 // This type is what is sent via MPI_Bcast so that 
 // the receiver knows what it got:

 typedef struct _BroadcastData {
    uint32_t s_payloadSize;
    uint8_t  s_payload[MAX_MESSAGE_SIZE];
 } BroadcastData, *pBroadcastData;

 static MPI_Datatype
 broadCastDataType() {
    static bool initialized = false;
    static MPI_Datatype result;
    if (!initialized) {
        // Define and register the data type.

        MPI_Datatype types[2] = {MPI_UINT32_T, MPI_UINT8_T};
        int          sizes[2] = {1, MAX_MESSAGE_SIZE};
        MPI_Aint     offsets[2] = {
            offsetof(BroadcastData, s_payloadSize), 
            offsetof(BroadcastData, s_payload)
        };
        if (MPI_Type_create_struct(2, sizes, offsets, types, &result) != MPI_SUCCESS) {
            throw std::runtime_error("Failed to create the BroadcastData struct");
        }
        if (MPI_Type_commit(&result) != MPI_SUCCESS) {
            throw std::runtime_error("Filed to commit BroadcastData struct");
        }
        initialized = true;

    }
    return result;
 }

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
            dest, MPI_RING_ITEM_TAG, gRingItemComm
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
    bool      s_targeted; // Need to request the next event.
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
    CRingBufferDecoder& rDecoder = dynamic_cast<CRingBufferDecoder&>(*gpBufferDecoder);
    rDecoder.setAnalyzer(gpAnalyzer);
    while (nBytes) {
        rDecoder.dispatchEvent(p.pBytes);

        nBytes   -= *p.pLongs;   // Assume size is first.
        p.pBytes += *p.pLongs;
    }
    // If this item was targeted, wew need to request the next one:

    if(pInfo->s_targeted) {
    
        uint8_t dummy;
        int status =MPI_Send(
                &dummy, 1, MPI_CHAR, 
                0, MPI_RING_ITEM_TAG, gRingItemComm
            );
        
        if(status != MPI_SUCCESS) {
            std::cerr << "MPI Status on send error in pump: " << status << std::endl;
            throw std::runtime_error("Worker failed to request a work item");
        }
    }

    Tcl_Free(reinterpret_cast<char*>(pInfo->s_pData));
    return 1;
}

static void
initEvent(pRingItemEvent pEvent) {
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
 * @note assembly is, for the most part, non-zero copy at our end receiving
 *     data directly into the block we queue to the main thread;
 *     unless Tcl_Realloc needs to move our data block.
*/
static void
assembleTargetedData(pRingItemEvent pEvent) {
    // Fill in the header an init the data fields:

    initEvent(pEvent);
    pEvent->s_targeted = true;     // Will need to request next event.

    // The first receive is a bit special as that gives us the size:
    // It may also be the only recieve -- if the event fits in 
    // MAX_MESSAGE_SIZE.
    //
    pEvent->s_pData = Tcl_Alloc(MAX_MESSAGE_SIZE);
    if (!pEvent->s_pData) {
        throw std::runtime_error("Tcl_Alloc failed in assembleTargetedData");
    }
    // Note this recv must allow sends from anywhere as we may send ourselves
    // a message to stop.
    MPI_Status status;
    if (MPI_Recv
        (pEvent->s_pData, MAX_MESSAGE_SIZE, MPI_UINT8_T,
         MPI_ANY_SOURCE, MPI_RING_ITEM_TAG, gRingItemComm, &status
        ) != MPI_SUCCESS) {
            throw std::runtime_error("Failed to receive a chunk in assembleTargetedData");
    }
    // Figure out what we got:
    
    int receivedBytes;
    MPI_Get_count(&status, MPI_UINT8_T, &receivedBytes);
    
    // How much data shoulid we get (assume we got at least the first long)
    uint32_t* pSize = reinterpret_cast<uint32_t*>(pEvent->s_pData);
    if (*pSize == 0) {
        // end marker.
        Tcl_Free(reinterpret_cast<char*>(pEvent->s_pData));
        pEvent->s_pData = nullptr;
        return;
    }
    pEvent->s_dataSize = *pSize;
    uint32_t remainingSize = *pSize - receivedBytes;

    // If remaining sioze > 0, then make a big enough buffer and
    // Make a  pointer to where to read the next chunk.

    if (remainingSize > 0) {
        pEvent->s_pData = Tcl_Realloc(reinterpret_cast<char*>(pEvent->s_pData), *pSize);
        if (!pEvent->s_pData) {
            throw std::runtime_error("Failed to alloc remaining buffer in assembleTargetedData");
        }
        uint8_t* p = reinterpret_cast<uint8_t*>(pEvent->s_pData) + receivedBytes;
        while (remainingSize) {
            int chunksize = remainingSize > MAX_MESSAGE_SIZE ? MAX_MESSAGE_SIZE : remainingSize;
            if (MPI_Recv(
                p, chunksize, MPI_UINT8_T, 
                MPI_ANY_SOURCE, MPI_RING_ITEM_TAG, gRingItemComm, &status
            ) != MPI_SUCCESS) {
                throw std::runtime_error("Failed to receive a chunk in assembleTargetedData");
            }
            MPI_Get_count(&status, MPI_UINT8_T, &receivedBytes);
            p += receivedBytes;
            remainingSize -= receivedBytes;
        }
    }
}
/**
 * receiveBroadcastEvent
 *     Get a single broadcast event into the RingItemEvent's buffer.
 * 
 * @param pEvent - pRingItemEvent whose buffer and size we will fill.
 * 
*/
static void
receiveBroadcastEvent(pRingItemEvent pEvent) {
    
    
    // The first broadcast tells us how much total data we're getting.
    
    BroadcastData buffer;

    if (MPI_Bcast(&buffer, 1, broadCastDataType(), MPI_ROOT_RANK, gRingItemComm) != MPI_SUCCESS) {
        throw std::runtime_error("Could not receive the first broadcasted chunk");
    }
    pEvent->s_dataSize = buffer.s_payloadSize;
    pEvent->s_pData    = reinterpret_cast<void*>(Tcl_Alloc(buffer.s_payloadSize));
    if(!pEvent->s_pData) {
        throw std::runtime_error("receiveBroadcastEvent failed initial buffer allocation");
    }
    memcpy(pEvent->s_pData, buffer.s_payload, buffer.s_payloadSize);
    pRingItemHeader pHdr = reinterpret_cast<pRingItemHeader>(pEvent->s_pData);
    // End marker
    if (pHdr->s_size == 0) {
        Tcl_Free(reinterpret_cast<char*>(pEvent->s_pData));
        pEvent->s_pData = nullptr;
        return;
    }
    uint32_t* pSize = reinterpret_cast<uint32_t*>(buffer.s_payload);
    uint32_t totalsize = *pSize;
    
    // Allocate the whole ring item so we can keep a running destination pointer:
    // and minimize the reallocs:
    pEvent->s_pData = reinterpret_cast<void*>(
        Tcl_Realloc(reinterpret_cast<char*>(pEvent->s_pData), totalsize)
    );
    uint8_t* p = reinterpret_cast<uint8_t*>(pEvent->s_pData);
    p += buffer.s_payloadSize;
    
    while (pEvent->s_dataSize < totalsize) {
        if (MPI_Bcast(&buffer, 1, broadCastDataType(), MPI_ROOT_RANK, gRingItemComm) != MPI_SUCCESS) {
            throw std::runtime_error("receiveBroadcastEvent failed to receive subsequent broadcast chunk");
        }
        memcpy(p, buffer.s_payload, buffer.s_payloadSize);
        p += buffer.s_payloadSize;
        pEvent->s_dataSize += buffer.s_payloadSize;
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

    // Send the request for the initial work item to the dealer.
    // The event handler will send subsequent requsts  when we're done
    // processing the previous request.  See 
    // Issue #148

    uint8_t dummy;
    int status =MPI_Send(
            &dummy, 1, MPI_CHAR, 
            0, MPI_RING_ITEM_TAG, gRingItemComm
        );

    if(status != MPI_SUCCESS) {
        std::cerr << "MPI Status on send error in pump: " << status << std::endl;
        throw std::runtime_error("Worker failed to request a work item");
    }
    // Our main loop is request data/get data.
    // where we defer the get data part to 
    // assembleTargetedData:

    while (1) {
        pRingItemEvent pE = reinterpret_cast<pRingItemEvent>(Tcl_Alloc(sizeof(RingItemEvent)));
        if (!pE) {
            throw std::runtime_error("physicsThread failed to allocate RingItemEvent");
        }
        // Send our request for data:

        
        
        assembleTargetedData(pE); // Fills in all of pE.
        if (!pE->s_pData) {
             break;  // End marker.
        }
        Tcl_ThreadQueueEvent(targetThread, &(pE->s_base), TCL_QUEUE_TAIL);
        Tcl_ThreadAlert(targetThread);
    }
    TCL_THREAD_CREATE_RETURN;
}


/**
 * nonPhysicsThread
 *    This is the thread that handles non-PHYSICS_EVENT data.  It recieves data broadcast
 *  fron the gRingItemComm communicator's rank 0 and queues an event for it to
 * the main thread's interpreter.  
 * 
 * @param parent -Actually a Tcl_ThreadId for the thread we should post the event to.
*/
static Tcl_ThreadCreateType
nonPhysicsThread(ClientData parent) {
    Tcl_ThreadId target = reinterpret_cast<Tcl_ThreadId>(parent);

    while(1) {
        pRingItemEvent pE = reinterpret_cast<pRingItemEvent>(Tcl_Alloc(sizeof(RingItemEvent)));
        if (!pE) {
            throw std::runtime_error("nonPhysicsThread failed to allocate RingItemEvent");
        }
        initEvent(pE);
        pE->s_targeted = false;    // Broadcast events don't need send requests.
        receiveBroadcastEvent(pE);
        if (!pE->s_pData) {
            break;
        }
        Tcl_ThreadQueueEvent(target, &(pE->s_base), TCL_QUEUE_TAIL);
        Tcl_ThreadAlert(target);
    }
    TCL_THREAD_CREATE_RETURN;
}
#endif


////////////////////////// Sender side code  ////////////////////////////////

/**
 * countPhysicsItems  (static)
 *    Per issue #131 - count the number of items in a block of physics
 * items and increment the buffers analyzed counter maintained by the 
 * analyzer which mus be a TclAnalyzer or we are going to just do nothing.
 * 
 * @param pItems  - Pointer to what could be a block of physics items.
 * @param nBytes  - Number of bytes in the block;
 * 
 *
*/
static void 
countPhysicsItems(const void* pItems, size_t nBytes) {
    CTclAnalyzer* pAnalyzer = dynamic_cast<CTclAnalyzer*>(gpAnalyzer);
    if (pAnalyzer) {
        // Count the items:

        typedef union _Ptr
        {
            const RingItemHeader* u_pItem;
            const uint8_t*        u_p8;
        } Ptr;
        Ptr itemPtr;
        itemPtr.u_p8 = reinterpret_cast<const uint8_t*>(pItems);
        UInt_t numItems(0);
        while (nBytes) {
            numItems++;
            nBytes -= itemPtr.u_pItem->s_size;
            itemPtr.u_p8 += itemPtr.u_pItem->s_size;
        }
        // Count them:

        pAnalyzer->IncrementCounter(EventsAnalyzed, numItems);
        pAnalyzer->IncrementCounter(EventsAnalyzedThisRun, numItems);
        pAnalyzer->IncrementCounter(EventsAccepted, numItems);
        pAnalyzer->IncrementCounter(EventsAcceptedThisRun, numItems);
        pAnalyzer->incrementBuffersAnalyzed(numItems);
    }
}
/**
 * updateStatistics (static)
 *    Called when a non physics item is about to be broadcast
 * to the workers.  We care about statistics when we get any of:
 * 
 * -   BEGIN_RUN - set title, run number, state
 *                 and clear statistics.; increment runs analyzed
 * -   END_RUN - Set state
 * -   PHYSICS_EVENT_COUNT - set the last sequence number.
 *
 * The assumption is that there is only one ring item:
 * 
 * @param pItem - Pointer to the ring item.
 * @param size  - Number of bytes in the item.
*/
static void
updateStatistics(const void* pItem, size_t size) {
    CTclAnalyzer* pAnalyzer = dynamic_cast<CTclAnalyzer*>(gpAnalyzer);
    CRingBufferDecoder* pDecoder = dynamic_cast<CRingBufferDecoder*>(gpBufferDecoder);
    if (pAnalyzer && pDecoder) {
        // Must be a TCL Analyzer and a decoder...

        typedef union _Ptr {
            const RingItemHeader* u_pItem;
            const uint8_t*   u_p8;
        } Ptr;
        Ptr itemPtr;
        itemPtr.u_p8 = reinterpret_cast<const uint8_t*>(pItem);
        if (size != itemPtr.u_pItem->s_size) {
            throw std::logic_error("Mismatch between sizes in updateStatistics");
        }
        // What we do next depends on the type:

        switch (itemPtr.u_pItem->s_type) {
        case BEGIN_RUN:
            pAnalyzer->setTitle(pDecoder->getTitle().c_str());
            pAnalyzer->setRunNumber(pDecoder->getRun());
            pAnalyzer->setBuffersAnalyzed(0);
            pAnalyzer->setLastSequence(0);
            pAnalyzer->setRunState(true);

            // Statitics counters:

            pAnalyzer->IncrementCounter(RunsAnalyzed);
            pAnalyzer->ClearCounter(EventsAnalyzedThisRun);
            pAnalyzer->ClearCounter(EventsAcceptedThisRun);
            pAnalyzer->ClearCounter(EventsRejectedThisRun);
            break;
        case END_RUN:
            pAnalyzer->setRunState(false);
            break;
        case PHYSICS_EVENT_COUNT:
            pAnalyzer->setLastSequence(pDecoder->getSequenceNo());
            break;
        }
    }

}


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
 * 
 * NOTE - this is dead code
*/ 
static void
oldsendRingItem(const void* pItem, size_t size) {
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
    countPhysicsItems(pItem, size);           // Update item count per issue #131
#endif
}

/**
 * sendRingItem
 *   Actually, for Issue #130, this just get the buffer and 
 * buffers the ring item.  The buffer will take care of transmitting
 * a block of items when it has enough chunks or when flush() is invoked.
 * 
 */
void sendRingItem(const void* pItem, size_t size) {
    getBuffer()->buffer(pItem, size);
    countPhysicsItems(pItem, size);           // Update item count per issue #131
}
void sendBufferedRingItems() {
    getBuffer()->flush();
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
 * (think a large DDAS system e.g) items are chunked out.
 *  just as sendRingItem does...if required. The main differences are:
 *  -  Workers don't request data but have it broadcast to them.
 *  -  Since MPI_Bcast does not say how much data the receiver actually got,
 *      we use the BroadcastData special type and this, inherently, will require
 *      data copying.  Fortunately, the send rate for these items is much smaller than
 *      for 'directed' sends(sendRingItem).
*/
void
broadcastRingItem(const void* pItem, size_t size) {
#ifdef WITH_MPI

    getBuffer()->flush();

    MPI_Datatype bcType = broadCastDataType();
    BroadcastData buffer;                             // Chunk buffer.
    const uint8_t* p = reinterpret_cast<const uint8_t*>(pItem);
    if (*p != 0) {   // *p==0 on end request.
          updateStatistics(pItem, size);       // Per issue #131 do here as size -> 0.
    }
    while (size) {
        int chunksize = MAX_MESSAGE_SIZE;
        if (size < chunksize) chunksize = size;
        buffer.s_payloadSize = chunksize;
        memcpy(buffer.s_payload, p, chunksize);

        if (MPI_Bcast(&buffer, 1, bcType, MPI_ROOT_RANK, gRingItemComm) != MPI_SUCCESS) { 
            throw std::runtime_error("broadcastRingItem: Failed to broadast a ring item chunk");
        }

        size -= chunksize;
        p += chunksize;
    }
#endif
}
////////////////////////// Pump management ///////////////////////////////
/**
 * startRingItemPump
 *    This actually starts two threads.  A thread which 
 * requests ring items sent to it via sendRingItem and a
 * thread which is alerted to broadcasts sent via broadcastRingitem
 * both need to assemble ring items from chunks.  
 * Both post the same event type to the main thread which
 * just finds the decoder - ensures it's a CRingBufferDecoder
 * and just dispatches the ring item.
*/
void 
startRingItemPump()
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
        TCL_THREAD_STACK_DEFAULT, TCL_THREAD_NOFLAGS
    );
#endif
}
/**
 *  stopRingItemPump
 *    How this behaves depends on the rank:
 *    -  Workers send their physicsThread a ring item header with length zero.
 *      assembledTargeted data will cause this to insert a null in the pointer to he
 *      data which the thread notices and uses to exit.
 *    - The Root thread does the same sort of thing but invokes broadcastRingItem to send it.
 * @note the communicator only has the root thread and the workers.
*/
void
stopRingItemPump() {
#ifdef WITH_MPI
    if (isMpiApp()) {
        RingItemHeader dummyEvent = {s_size: 0, s_type: 0};
        int rank;
        MPI_Comm_rank(gRingItemComm, &rank);
        if (rank == 0) {  
            broadcastRingItem(&dummyEvent, sizeof(RingItemHeader));
        } else {
            MPI_Send(&dummyEvent, static_cast<int>(sizeof(RingItemHeader)), MPI_UINT8_T, 
                rank, MPI_RING_ITEM_TAG, gRingItemComm
            );
        }
    }
#endif
}
