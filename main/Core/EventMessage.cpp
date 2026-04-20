/*
    This software is Copyright by the Board of Trustees of Michigan
    State University (c) Copyright 2014.

    You may use this software under the terms of the GNU public license
    (GPL).  The terms of this license are described at:

     http://www.gnu.org/licenses/gpl.txt

     Authors:
             Ron Fox
             Jeromy Tompkins 
	     NSCL
	     Michigan State University
	     East Lansing, MI 48824-1321
*/
#include <config.h>
#include "EventMessage.h"
#include "EventList.h"
#include "Event.h"
#include "Globals.h"
#include <SpecTcl.h>

#include <TclPump.h>
#include <tcl.h>
#include <stdexcept>
#include <iostream>
#include <DataFormat.h>
#include "CTreeParameterVector.h"
#include "CTreeParameter.h"
#include <numeric>

using namespace ufmt;
#ifdef WITH_MPI
#include <mpi.h>
#else
typedef int MPI_Datatype;               // Helps to minimze the #ifdefery.
#endif

#define HISTOGRAMER_RANK MPI_EVENT_SINK_RANK         // Histogramer runs rank 1.
#define EVENT_TAG MPI_RING_ITEM_TAG             // Tag used for event messages.
#define RECEIVER_EVENTLIST_SIZE 1       // Size of the event list we receive from one sender.


/**
 * Musings on how to handle the vector parameters.
 * The parameter values have already been sent via the 
 * tree parameters that make up each vector.  However, depending on 
 * the data each worker sees, the mapping between those and
 * parameter ids may be different worker to worker. 
 * 
 * What the histogrammer therefore needs to know is fpr each
 * vector, for each event, the set of parameter ids to stuff
 * into that vector on its end.
 *  
 * Therefore for each vector we need something like:
 */

 #define MAX_VECTOR_NAME 128
 typedef struct _VectorNameAndLength {
    char name[MAX_VECTOR_NAME];             // sets a limit on the name of the vector.
    int  elementCount;          // Number of elements this event.
 } VectorNameAndLength, *pVectorNameAndLength;

 /**
 and then an array of elementCount elements that has the ids
 of the parameters to push into that vector.
 Note that the vectors are known to the histogrammer it's 
 CTreeParameterVector::numVectors()  - call that nvec.
 The messaging can be:

 nvec VectorNameAndLength structs followed by 
 sum(elementcount) integers that provide the mapping between event id and items to push into the vectors
 in the histogrammer.

 A future cool, but interesting to implement optimization is to only send the maps as needed and
 for the histogrammer to cache them indexed by worker so that all that needs to be sent event by event
 is the MPIParameter vector.
 
 Internal definitions:
 An event looks like an array of parameters:
*/

typedef struct _MPIParameter {
    int    number;
    double value;
} MPIParameter, *pMPIParameter;


static MPI_Datatype ParameterType;
static MPI_Datatype VectorType;


// Issue #205 - We accumulate the parameters from several
// events into the accumulatedEvents vector.
// the # of parameters in each event is accumulated into accumulatedSizes.
// The final send is three messages:
//  integer - size of the accumulatedSizes vector.
//  array of accumulatied sizes (size determined by the first message).
//  array of accumulated events - total size detemined by sum of accumluated sizes.
static const size_t BATCH_SIZE(100);     // Number of events in a batch.
static std::vector<MPIParameter> accumulatedEvents;
static std::vector<unsigned>          accumulatedSizes;
//
// Add the vector stuff too.
// Note the number of events together with the
// number of vector parameters together with the sizes
// in the vector mappings allow us to untangle this 
// on an event by event basis.
//
static std::vector<VectorNameAndLength> vectorNames;
static std::vector<unsigned>            vectorMappings;

// Register our custom data types;

static bool typesRegistered(false);
static void RegisterTypes() {
    if (!typesRegistered) {
#ifdef WITH_MPI

        {
            // MPIParameter:

            int lengths[2] = {1,1};
            MPI_Datatype types[2] = {MPI_INTEGER, MPI_DOUBLE};
            MPI_Aint offsets[2] = {offsetof(MPIParameter, number), offsetof(MPIParameter, value)};

            if (MPI_Type_create_struct(2, lengths, offsets, types, &ParameterType) != MPI_SUCCESS) {
                throw std::runtime_error("Failed to create MPIParameter data type");
            }
            if (MPI_Type_commit(&ParameterType) != MPI_SUCCESS) {
                throw std::runtime_error("Failed to commit MPIParameter data types");
            }
        
        

        }
        {
            // Vector type:

            int lengths[2] = {MAX_VECTOR_NAME, 1};
            MPI_Datatype types[2] = {MPI_CHAR, MPI_INTEGER};
            MPI_Aint offsets[2]   = {offsetof(VectorNameAndLength, name), offsetof(VectorNameAndLength, elementCount)};

            if (MPI_Type_create_struct(2, lengths, offsets, types, &VectorType) != MPI_SUCCESS) {
                throw std::runtime_error("Failed to create MPI vector type");
            }
            if (MPI_Type_commit(&VectorType) != MPI_SUCCESS) {
                throw std::runtime_error("Failed to commit MPI vector type");
            }

        }
#endif
        typesRegistered = true;
    }
}
// Return the type associated with MPIParameterType.
static MPI_Datatype
getParameterType() {
    RegisterTypes();
    return ParameterType;
}

static MPI_Datatype
getVectorType() {
    RegisterTypes();
    return VectorType;
}

///////////////////////////////// Sender side private functions //////////////////////////////
// Send an event:

#
// Flush events to the histogrammer from the accumulatedSizEs and accumulatedEvents
// vectors.  

// Note that we just accumulate events until we have BATCH_SIZE of them and then FlushToHistogrammer
// is called to actually do the send.
//
// The final send is several messages:
//  integer - size of the accumulatedSizes vector.
//  array of accumulatied event sizes (size determined by the first message).
//  array of accumulated events - total size detemeined by sum of accumluated sizes.
//  How to send vector valued parameters?
#ifdef WITH_MPI                     // only called if compiled with MPI enabled.
static void flushEventsToHistogrammer() {
    // do nothing if there are no events to flush (e.g. called before sending statechage).

    if (accumulatedSizes.size() > 0) {
        // Send the number of events
        unsigned nEvents = accumulatedSizes.size();
        if (MPI_Send(
            &nEvents, 1, MPI_UNSIGNED, HISTOGRAMER_RANK, EVENT_TAG, MPI_COMM_WORLD) !=
            MPI_SUCCESS
        ) {
            throw std::runtime_error("Failed to send # of events in a batch to the histogrammer");
        }

        // Send the event sizes.

        if(MPI_Send(
            accumulatedSizes.data(), accumulatedSizes.size(), MPI_UNSIGNED, 
            HISTOGRAMER_RANK, EVENT_TAG, MPI_COMM_WORLD
            ) != MPI_SUCCESS
        ) {
            throw std::runtime_error("Failed to send event sizes array -> histogrammer");
        }

        // send the event data.

        if (MPI_Send(
            accumulatedEvents.data(), accumulatedEvents.size(), getParameterType(),
                HISTOGRAMER_RANK, EVENT_TAG, MPI_COMM_WORLD
            ) != MPI_SUCCESS
        ) {
            throw std::runtime_error("Failed to send event paramter soup -> histogrammer");
        }

        // Now send the vector name struct:

        if (MPI_Send(
            vectorNames.data(), vectorNames.size(), getVectorType(), 
            HISTOGRAMER_RANK, EVENT_TAG, MPI_COMM_WORLD)
            != MPI_SUCCESS
        ) {
            throw std::runtime_error("Failed to send vector name block");
        }
        // Send the vector mappgins:

        if (MPI_Send(
            vectorMappings.data(), vectorMappings.size(), MPI_UNSIGNED,
            HISTOGRAMER_RANK, EVENT_TAG, MPI_COMM_WORLD)
            != MPI_SUCCESS) {
            throw std::runtime_error("Failed to send vector parameter map");
        }

        // Clear the accumulated event storage.
        accumulatedEvents.clear(); 
        accumulatedSizes.clear();    
        vectorNames.clear();
        vectorMappings.clear();
    }
}
#endif

static void
SendEventToHistogramer(CEvent& event) {
#ifdef WITH_MPI
    // We use the dope vector to only send the valid parameters:

    DopeVector& dope(event.getDopeVector());
    auto valids = dope.size();
    accumulatedSizes.push_back(valids);                   // number of items in the event.
    for (int i =0; i < valids; i++) {
        accumulatedEvents.push_back({number: (int)dope[i], value: event[dope[i]]});
    }
    // Now deal with the tree parmaeter vectors:
    // If the total number of events is BATCH_SIZE, flush the batched events to the histogrammer:

    std::map<std::string, CTreeParameterVector::pTreeVectorInfo>::iterator p;
    for (p = CTreeParameterVector::begin(); p != CTreeParameterVector::end(); ++p) {
        VectorNameAndLength v;
        strncpy(v.name, p->first.c_str(), MAX_VECTOR_NAME);   // Hopefully it'll never cut off.
        v.name[MAX_VECTOR_NAME-1] = 0;                        // lazy quick way to ensure null termination.
        v.elementCount = p->second->s_event.size();

        vectorNames.push_back(v);

        // Now the mappings:

        for (auto e : p->second->s_event) {
            vectorMappings.push_back(e->getId());
        }
    }

    if (accumulatedSizes.size() == BATCH_SIZE) {
        flushEventsToHistogrammer();
    }
#endif
}

// send an event list to the histogramer:

static void
MPIHistogramEvents(CEventList& events) {
    for (int i =0; i < events.size(); i++) {
        SendEventToHistogramer(*events[i]);
        events[i]->clear();     //   Invalidate the parameters.
    }
}
////////////////////////////// Receiver side private functions/structs  //////////////////////////
// This is the form of the event that is posted to the interpreter to be relayed for analysis.

// Definitions the receiver needs:

typedef struct _EventEvent {                // A Tcl Event that has a physics event....
    Tcl_Event header;                        // What Tcl sees.
    unsigned    numEvents;                   // Actual number of events.
    CEventList* pEvents;                    // What we got from MPIReceiveEvent
    pVectorNameAndLength pVectorCounts;   // Soup of name and counts.
    unsigned*             pVectorMaps;     // Mappings for vector element ids.
} EventEvent, *pEventEvent;
static Tcl_ThreadId mainThread;              // Where we post events.
static Tcl_ThreadId pumpThread;              // Thread accepting data from the workers.
static bool pumping(false);                         //  Flag to keep running the pump.
/**
 * reconstructVectors
 *    GIven an event, reconstruct the vectors associated with that event.
 * Note this is destructive in that CTreeParameterVector::BeginEvent is called.
 * 
 * @param pTclEvent - pointer to the Tcl event (has the information needed to map)
 * @param event     - The reconstucted physics event.
 * @param nameIdx   - Index into the name/count array at which this event starts.
 * @param mapIdx    - Index into the parameter Id map where we start.
 * @return int      - Where the next event starts in the map.
 * @note all of this vector reconstruction stuff assumes that all vectors got made
 *  in all workers _and_ the histogramer.  Otherwise it falls apart because
 *  CTreeParamterVector::numVectors() is not the same everywhere.
 */
static int
reconstructVectors(pEventEvent pTclEvent, CEvent& event, int nameIdx, int mapIdx) {
    auto numVecs = CTreeParameterVector::numVectors();
    CTreeParameter::setEvent(event);
    CTreeParameterVector::BeginEvent();                 // Clear out any prior vector.
    

    for (int vec = 0; vec < numVecs; vec++) {

        int     elements = pTclEvent->pVectorCounts[nameIdx].elementCount;
        CTreeParameterVector v = CTreeParameterVector::find(pTclEvent->pVectorCounts[nameIdx].name);
        for (int i = 0; i < elements; i++) {
            v.push_back(event[pTclEvent->pVectorMaps[mapIdx]]);
            mapIdx++;
        }

        nameIdx++;
    }
    return mapIdx;                          // It's been incremented past the event.
}

// Marshall an event from MPI Messages.  note that we get the size from anybody
// but then explictly get the parameters from the source that sent the size.
// this simplifies the logic.  We produce an event list with one event.
// Using a parameter as the event list saves copies.
// Caller must have sized the event list to 1.
// Per issue #205 - we recieve a batch of events.
// This comes in three messages:
// 1. UNSIGNED number of events in the batch
// 2. Array of event sizes (number of parameters in each event).
// 3. Soup of parameters (MPIParameter).
//
///  Thes are marshalled back into events in the event list.
static void
MPIReceiveEvent(EventEvent& tclEvent) {
    // 

#ifdef WITH_MPI
    CEventList& eventList = *tclEvent.pEvents;
    MPI_Status status;
    // Get the number of events we've got and create the receivers for them:

    unsigned nEvents;
    if (MPI_Recv(
            &nEvents, 1, MPI_UNSIGNED, MPI_ANY_SOURCE,
            EVENT_TAG, MPI_COMM_WORLD, &status
        ) != MPI_SUCCESS
    ) {
        throw std::runtime_error("Failed to receive # of events.");
    }
    tclEvent.numEvents  = nEvents;
    // Create the CEvents to receive the data:
    // the sender is programmed such that nEvents is never zero.
    for (int i=0; i < nEvents; i++) {
        auto pe = new CEvent();
        eventList[i] = pe;
    }

    // Get the sizes 

    int sender = status.MPI_SOURCE;
    std::vector<unsigned> sizes;
    sizes.resize(nEvents);
    
    unsigned    size;
    if (MPI_Recv(
            sizes.data(), nEvents, MPI_UNSIGNED, sender,
            EVENT_TAG, MPI_COMM_WORLD, &status) 
        != MPI_SUCCESS) {
        throw std::runtime_error("Failed to receive the event sizes.");
    }
    // Create the parameter vectors:

    //Figure out the total event size by summing data:

    auto totalParameterCount = std::accumulate(sizes.begin(), sizes.end(), 0);

    std::vector<MPIParameter> params(totalParameterCount);
    

    // Now get the parameter values and numbers:

    if (MPI_Recv(
            params.data(), totalParameterCount, getParameterType(), 
            sender, EVENT_TAG, MPI_COMM_WORLD, &status
        ) != MPI_SUCCESS) {
        throw std::runtime_error("Failed to receive event parameters");
    }
    // Build the events we already allocated them in the event list.
    
    int index(0);
    for (int i = 0; i < nEvents; i++) {
        CEvent& e(*eventList[i]);              // Event we're filling in. notational sugar.
        unsigned n = sizes[i];                  // number of paramters in the event.
        for (int j = 0; j < n; j++) {
            e[params[index].number] = params[index].value;  // Fill in a parameter with its value.
            index++;
        }
    

    }
    // Now figure out how big the messagse with vector counts is and
    // allocated/read it directly into the event structure. We do this
    // by simply multiplying the event list size (nEvents) by the number
    // of vector that are defined by the sizeof VectorNameAndLength.

    int vectorNameCount = nEvents * CTreeParameterVector::numVectors();   // Needed for mpi recv.
    size_t vectorNameBytes = vectorNameCount * sizeof(VectorNameAndLength); // for the tcl alloc.
    tclEvent.pVectorCounts = reinterpret_cast<pVectorNameAndLength>(Tcl_Alloc(vectorNameBytes));
    if (!tclEvent.pVectorCounts) {
        throw std::runtime_error("Failed to allocated vector name/size array");
    }
    if (MPI_Recv(
        tclEvent.pVectorCounts, vectorNameCount, getVectorType(),
        sender, EVENT_TAG, MPI_COMM_WORLD, &status
    ) != MPI_SUCCESS) {
        throw std::runtime_error("Failed to receive vector name/count block");
    }
    // Now I need to see how many mappings there will be and receive those.
    // that's just summing over the elementCounts of te items I just got:
    
    int vectorMappings(0);
    for (int i =0; i < vectorNameCount; i++) {
        vectorMappings += (tclEvent.pVectorCounts[i]).elementCount;
    }
    tclEvent.pVectorMaps = reinterpret_cast<unsigned*>(Tcl_Alloc(vectorMappings * sizeof(unsigned)));
    if (!tclEvent.pVectorMaps) {
        throw std::runtime_error("Failed to allocated vector mappings");
    }
    if (MPI_Recv(
        tclEvent.pVectorMaps, vectorMappings, MPI_UNSIGNED, 
        sender, EVENT_TAG, MPI_COMM_WORLD, &status
    ) != MPI_SUCCESS) {
        throw std::runtime_error("Failed to receive vector mappings");
    }

#endif
}



// This is the event handler.  It runs in the mainThread and just:
// 1. Passes events on to the event sink pipeline.
// 2. Deletes the CEvent part of the EventEvent we have.
// 3. Returns 1 to indicate we're done with this event.
//  @param p - Pointer to what is actually an EventEvent.
//  @param flags - event handling flags which we studiously ignore

static int EventEventHandler(Tcl_Event* p, int flags) {
    pEventEvent pEvent = reinterpret_cast<pEventEvent>(p);
    auto pipeline = SpecTcl::getInstance()->GetEventSinkPipeline();
    // needs some TCL to 
    // 1. do on an event by event basis and
    // 2. reconstruct the vectors for each event before running the
    //    sink pipeline.

    int nameindex=0; 
    int mapindex=0;
    if (pipeline) {
        CEventList& events(*pEvent->pEvents);
        for (int i =0; i < pEvent->numEvents; i++) {
            CEvent& event(*events[i]);
            CEventList oneEvent;
            oneEvent[0] = &event;
            // Reconstruct the vectors for this event:
            mapindex   = reconstructVectors(pEvent, event, nameindex, mapindex);
            nameindex += CTreeParameterVector::numVectors();
            (*pipeline)(oneEvent);
            events[i] = nullptr;   // The event list destructor deletes the event.
        }
        
    }
    
    
    // destructor destroys the events too.
    delete pEvent->pEvents;
    pEvent->pEvents = nullptr;
    Tcl_Free(reinterpret_cast<char*>(pEvent->pVectorCounts));
    pEvent->pVectorCounts = nullptr;
    Tcl_Free(reinterpret_cast<char*>(pEvent->pVectorMaps));
    pEvent->pVectorMaps = nullptr;
    
    return 1;
}

// Create an event (we need to initialize the header and new into existence the pEvents field):

static pEventEvent
createTclEvent() {
    pEventEvent result = reinterpret_cast<pEventEvent>(Tcl_Alloc(sizeof(EventEvent)));
    if (!result) {
        std::cerr << "Tcl_Alloc failed to make an EventEvent in createTclEvent\n";
        std::cerr << "It's hopeless to continue\n";
        Tcl_Exit(-1);
    }
    result->header.proc = EventEventHandler;
    result->header.nextPtr = nullptr;
    result->numEvents      = 0; 
    result->pEvents = new CEventList(RECEIVER_EVENTLIST_SIZE);         // We're sending one event around.
    result->pVectorCounts = nullptr;                                   // allocated later.
    result->pVectorMaps    = nullptr;                                   // allocated later.
    
    // MPIReceiveEvent will create the CEvent objects themselves.
    return result;
}

// The eventpump thread itself:

static Tcl_ThreadCreateType
EventPumpThread(ClientData pData) {
    // The MPI part is hidden inside of MPIReceive event so we don't need #ifdeffery:

    while (pumping) {
        pEventEvent pEvent = createTclEvent();
        MPIReceiveEvent(*pEvent);

        // Post the thread and notify:

        Tcl_ThreadQueueEvent(
            mainThread, reinterpret_cast<Tcl_Event*>(pEvent), TCL_QUEUE_TAIL
        );
        Tcl_ThreadAlert(mainThread);

    }
    TCL_THREAD_CREATE_RETURN;
}

//////////////////////// Internal functions to support passing state changes
// to the event sink pipeline.
//

// This is the message sent for state change operations:

typedef struct _StateChangeMessage {
    unsigned s_runNumber;
    int      s_isBegin;
    char     s_title[TITLE_MAXSIZE+1];
} StateChangeMessage, *pStateChangeMessage;

typedef struct _StateChangeEvent {
    Tcl_Event s_base;
    StateChangeMessage s_info;
} StateChangeEvent, *pStateChangeEvent;
/**
 *   Return the data type for the StateChangeMessage struct.
 * creates it if needed else returns it from its cache.
 */
#ifdef WITH_MPI
static MPI_Datatype
StateChangeType() {
    static bool mustCreate(true);
    static MPI_Datatype result;
    // Flush the event buffers:

    
    if (mustCreate) {
        MPI_Aint offsets[3] = {
            offsetof(StateChangeMessage, s_runNumber),
            offsetof(StateChangeMessage, s_isBegin),
            offsetof(StateChangeMessage, s_title)
        };
        MPI_Datatype memberTypes[3] = {
            MPI_UNSIGNED, MPI_INT, MPI_CHAR
        };
        int counts[3] = {
            1, 1, TITLE_MAXSIZE+1
        };
        if (MPI_Type_create_struct(3, counts, offsets, memberTypes, &result) != MPI_SUCCESS) {
            throw std::runtime_error("Unable to create the StateChange message type");
        }
        if (MPI_Type_commit(&result) != MPI_SUCCESS) {
            throw std::runtime_error("Unable to commit the state change message type");
        }

        mustCreate = false;
    }
    return result;
}
#endif



/**
 * StateChangeEventHandler
 *     Scheduled from the state change pump thread when a state change message was received.
 */

int
StateChangeEventHandler(Tcl_Event* pRaw, int flags) {
    pStateChangeEvent pEvent = reinterpret_cast<pStateChangeEvent>(pRaw);
    auto pipeline = SpecTcl::getInstance()->GetEventSinkPipeline();

    if (pEvent->s_info.s_isBegin) {
        pipeline->OnBegin(pEvent->s_info.s_runNumber, pEvent->s_info.s_title);
    } else {
        pipeline->OnEnd(pEvent->s_info.s_runNumber, pEvent->s_info.s_title);
    }
    return 1;
}

/**
 * StateChangePump - thread to loop on accepting messages for state changes from Root.
 */
static Tcl_ThreadCreateType
StateChangePump(ClientData pData) {
#ifdef WITH_MPI
    Tcl_ThreadId mainThread = reinterpret_cast<Tcl_ThreadId>(pData);
    while(1) {
        pStateChangeEvent pEvent = reinterpret_cast<pStateChangeEvent>(Tcl_Alloc(sizeof(StateChangeEvent)));
        if (!pEvent) {
            std::cerr << "Tcl_Alloc  failed to allocate a state change event\n";
            Tcl_Exit(-1);
        }
        pEvent->s_base.proc = StateChangeEventHandler;
        pEvent->s_base.nextPtr = nullptr;
        MPI_Status status;
        if (MPI_Recv(
            &pEvent->s_info, 1, StateChangeType(), 
            MPI_ROOT_RANK, MPI_STATE_CHANGE_TAG, MPI_COMM_WORLD, 
            &status) != MPI_SUCCESS) {
            throw std::runtime_error("Failed to mpi_recv a state change item.");
        }
        // If the run number is 0xffffffff it's an end:

        if(pEvent->s_info.s_runNumber == -1) break; 

        Tcl_ThreadQueueEvent(mainThread, reinterpret_cast<Tcl_Event*>(pEvent), TCL_QUEUE_TAIL);
        Tcl_ThreadAlert(mainThread);
    }
#endif
    TCL_THREAD_CREATE_RETURN;
}


/**
 *  MPISendStateChange  
 *    Send a state change message to the pump for those messages in the event sink pipeline.
 * @param run - run number.
 * @param title - Title string (at most TITLE_MAXSIZE characters + null terminator).
 * @param begin - True if this is a begin run item false if an end.
 */
static void MPISendStateChange(unsigned run, const char* title, bool begin) {
#ifdef WITH_MPI
    flushEventsToHistogrammer();              // Flush the accumulated events first.
    StateChangeMessage msg;
    msg.s_runNumber = run;
    msg.s_isBegin = begin;
    strncpy(msg.s_title, title, TITLE_MAXSIZE +1);

    if(MPI_Send(
        &msg, 1, StateChangeType(), 
        MPI_EVENT_SINK_RANK, MPI_STATE_CHANGE_TAG, MPI_COMM_WORLD
        ) != MPI_SUCCESS) {
        throw std::runtime_error("Failed to send a state change message to the event sink.");
    }
#endif
}
////////////////////////////////// API public functions ///////////////////////////////
/**
 * HistogramEvents
 *   How this operates depends on the environment of the program.
 * In Serial SpecTcl, we just invoke the function call operator on the event sink pipeline.
 * In mpiSpecTcl under mpiRun, we message the histogram process (which runs the event sink pipeline)
 * Sending it events in the event list we've been given:
 * 
 *  @param events - References the events to histogram.
 * 
*/
void HistogramEvents(CEventList& events) {
    if (isMpiApp()) {
        MPIHistogramEvents(events);    // Parallel
    } else {                           // Serial
        auto pipeline = SpecTcl::getInstance()->GetEventSinkPipeline();
        if (pipeline) {
            (*pipeline)(events);    
        }
    }
}
/**
 *  BeginRun
 *    The decoder invokes this to send a begin run to the event sink pipeline.
 *   In the MPI case, this  results in MPISendStateChange which sends a state change
 *   message to the event sink. In non-MPI this just invokes the event sink pipeline's
 *    OnBegin method.
 * 
 * @param run - run number
 * @param title - Pointer to the title string.
 * 
 */
void
BeginRun(unsigned run, const char* title) { 
    if (isMpiApp()) {
        MPISendStateChange(run, title, true);
    } else {
         auto pipeline = SpecTcl::getInstance()->GetEventSinkPipeline();
         pipeline->OnBegin(run, title);
    }
}

/**
 * EndRun
 *    The decoder invokes this to send an end of run along the event sink pipeline.
 *  Operation is similar to BeginRun above:
 *  
 * @param run - run number
 * @param title - Pointer to the title string.
 */
void
EndRun(unsigned run,const char* title) {
    if (isMpiApp()) {
        MPISendStateChange(run, title, false);
    } else {
         auto pipeline = SpecTcl::getInstance()->GetEventSinkPipeline();
         pipeline->OnEnd(run, title);
    }
}
/**
 * startHistogramPump
 *    If the histogram pump is not already running, start it.  If it is, stop it first.
 *    @note the global TclNotifier is defined in TclPump and is the handle to the notifier needed
 *    in the pump thread to alert it to queued events.
 * 
 * 
*/
void startHistogramPump() {
    RegisterTypes();
    mainThread = Tcl_GetCurrentThread();
    if (pumping) {
        stopHistogramPump();
    }
    pumping = true;
    if (Tcl_CreateThread(
           &pumpThread, EventPumpThread, nullptr, TCL_THREAD_STACK_DEFAULT, TCL_THREAD_JOINABLE
        ) != TCL_OK) {
            std::cerr << "Could not start event pump for getting events for enent pipeline\n";
            Tcl_Exit(-1);
    }    
}

/** 
 * stopHistogramPump 
 *     Stops the histogram pump.  This is done by
 *     setting pumping to false and sending an event with no parameters
 *     then joining the thread.
 * @note if the pump is not runnning this is a no-op.
*/
void
stopHistogramPump() {
    if (pumping) {
        int exitStatus;
        pumping = false;          // Stop on next message.
        CEventList fakeEvents(1);
        fakeEvents[0] = new CEvent;
        HistogramEvents(fakeEvents);
        flushEventsToHistogrammer();
        Tcl_JoinThread(pumpThread, &exitStatus);  // Don't carea aboput the exit status.
    }
}


/** 
 * startStateChangePump
 *    Start the state change pump thread... should be called in the event sink.
 */
void
startStateChangePump() {
    auto target = Tcl_GetCurrentThread();
    Tcl_ThreadId thePump;
    Tcl_CreateThread(
        &thePump, StateChangePump, reinterpret_cast<ClientData>(target), 
        TCL_THREAD_STACK_DEFAULT, TCL_THREAD_NOFLAGS
    );
}
/**
 * stopStateChangePump
 *     Stop the state change pump.  Must be called in MPI_ROOT_THREAD.
 */
void
stopStateChangePump() {
    BeginRun(-1, "");
}