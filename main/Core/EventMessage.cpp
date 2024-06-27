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
#include "DataFormat.h"
#ifdef WITH_MPI
#include <mpi.h>
#else
typedef int MPI_Datatype;               // Helps to minimze the #ifdefery.
#endif

#define HISTOGRAMER_RANK MPI_EVENT_SINK_RANK         // Histogramer runs rank 1.
#define EVENT_TAG MPI_RING_ITEM_TAG             // Tag used for event messages.
#define RECEIVER_EVENTLIST_SIZE 1       // Size of the event list we receive from one sender.

// Internal definitions:
// An event looks like an array of parameters:

typedef struct _MPIParameter {
    int    number;
    double value;
} MPIParameter, *pMPIParameter;
static MPI_Datatype ParameterType;

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

///////////////////////////////// Sender side private functions //////////////////////////////
// Send an event:

static void
SendEventToHistogramer(CEvent& event) {
#ifdef WITH_MPI
    // We use the dope vector to only send the valid parameters:

    DopeVector& dope(event.getDopeVector());
    auto valids = dope.size();
     
    // Send the size:

    if (MPI_Send(
        &valids, 1, MPI_UNSIGNED, HISTOGRAMER_RANK, EVENT_TAG, MPI_COMM_WORLD
    ) != MPI_SUCCESS) {
        throw std::runtime_error("Failed to send event size to histogramer");
    }
    // Marshall the event:
    std::vector<MPIParameter> parameters;
    for (int i = 0; i < valids; i++) {
        MPIParameter param = {number: dope[i], value: event[dope[i]]};
        parameters.push_back(param);
    }
    if (MPI_Send(
        parameters.data(), valids, getParameterType(), 
            HISTOGRAMER_RANK, EVENT_TAG, MPI_COMM_WORLD
        ) != MPI_SUCCESS) {
            throw std::runtime_error("Failed to send event parameters to histogramer");
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

// Marshall an event from MPI Messages.  note that we get the size from anybody
// but then explictly get the parameters from the source that sent the size.
// this simplifies the logic.  We produce an event list with one event.
// Using a parameter as the event list saves copies.
// Caller must have sized the event list to 1.
static void
MPIReceiveEvent(CEventList& eventList) {
    CEvent& event(*eventList[0]);

#ifdef WITH_MPI
    // Get the size and sender:

    MPI_Status status;
    unsigned    size;
    if (MPI_Recv(
            &size, 1, MPI_UNSIGNED, MPI_ANY_SOURCE, 
            EVENT_TAG, MPI_COMM_WORLD, &status) 
        != MPI_SUCCESS) {
        throw std::runtime_error("Failed to receive the event size.");
    }
    // Create the parameter vector:

    std::vector<MPIParameter> params(size);
    int sender = status.MPI_SOURCE;

    // Now get the parameter values and numbers:

    if (MPI_Recv(
            params.data(), size, getParameterType(), 
            sender, EVENT_TAG, MPI_COMM_WORLD, &status
        ) != MPI_SUCCESS) {
        throw std::runtime_error("Failed to receive event parameters");
    }
    // Build the event:

    for (auto& par: params) {
        event[par.number] = par.value;
    }

#endif
}

// This is the form of the event that is posted to the interpreter to be relayed for analysis.

typedef struct _EventEvent {                // A Tcl Event that has a physics event....
    Tcl_Event header;                        // What Tcl sees.
    CEventList* pEvents;                    // What we got from MPIReceiveEvent
} EventEvent, *pEventEvent;
static Tcl_ThreadId mainThread;              // Where we post events.
static Tcl_ThreadId pumpThread;              // Thread accepting data from the workers.
static bool pumping(false);                         //  Flag to keep running the pump.

// This is the event handler.  It runs in the mainThread and just:
// 1. Passes events on to the event sink pipeline.
// 2. Deletes the CEvent part of the EventEvent we have.
// 3. Returns 1 to indicate we're done with this event.
//  @param p - Pointer to what is actually an EventEvent.
//  @param flags - event handling flags which we studiously ignore

static int EventEventHandler(Tcl_Event* p, int flags) {
    pEventEvent pEvent = reinterpret_cast<pEventEvent>(p);
    auto pipeline = SpecTcl::getInstance()->GetEventSinkPipeline();
    if (pipeline) {
        (*pipeline)(*pEvent->pEvents);
    }
    delete pEvent->pEvents;  //Event list destructor destroys the events too.
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
    result->pEvents = new CEventList(RECEIVER_EVENTLIST_SIZE);         // We're sending one event around.
    (*result->pEvents)[0] = new CEvent;
    return result;
}

// The eventpump thread itself:

static Tcl_ThreadCreateType
EventPumpThread(ClientData pData) {
    // The MPI part is hidden inside of MPIReceive event so we don't need #ifdeffery:

    while (pumping) {
        pEventEvent pEvent = createTclEvent();
        MPIReceiveEvent(*pEvent->pEvents);

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
        Tcl_JoinThread(pumpThread, &exitStatus);  // Don't carea aboput the exit status.
    }
}
