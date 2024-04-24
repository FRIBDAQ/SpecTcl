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
#include "EventList.h"
#include "Event.h"
#include <SpecTcl.h>

#include <TclPump.h>
#include <tcl.h>
#include <stdexcept>
#ifdef WITH_MPI
#include <mpi.h>
#else
typedef int MPI_Type;               // Helps to minimze the #ifdefery.
#endif

#define HISTOGRAMER_RANK 1         // Histogramer runs rank 1.
#define EVENT_TAG 2                // Tag used for event messages.

// Internal definitions:
// An event looks like an array of parameters:

typedef _MPIParameter {
    int    number;
    double value;
} MPIParameter, *pMPIParameter;
static MPI_type ParameterType;

// Register our custom data types;

static bool typesRegistered(false);
static void RegisterTypes() {
    if (!typesRegistered) {
#ifdef WITH_MPI

        {
            // MPIParameter:

            int lengths[2] = {1,1};
            int types[2] = {MPI_INTEGER, MPI_DOUBLE};
            int offsets[2] = {offsetof(MPIParameter, number), offsetof(MPIParameter, value)};

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
static MPI_Type
getParameterType() {
    RegisterTypes();
    return ParameterType;
}

///////////////////////////////// Sender side private functions //////////////////////////////
// Send an event:

static void
SendEventToHistogramer(const CEvent& event) {
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
        MPIParameter param = {number: dope[i], value: event[i]};
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
MPIHistogramEvents(const CEventList& events) {
    for (auto& event : events) {
        SendEventToHistogramer(event);
    }
}
////////////////////////////// Receiver side privae functions //////////////////////////

// Marshall an event from MPI Messages.  note that we get the size from anybody
// but then explictly get the parameters from the source that sent the size.
// this simplifies the logic.  We produce an event list with one event.
// Using a parameter as the event list saves copies.
// Caller must have sized the event list to 1.
static void
MPIReceiveEvent(CEventList& eventList) {
    CEvent& event(*eventList[0]);

#ifdef MPI
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
        throw std::runtime_exception("Failed to receive event parameters");
    }
    // Build the event:

    for (auto& par: params) {
        event[par.number] = par.value;
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
void HistogramEvents(CEVentList& events) {
    if (isMpiApp()) {
        auto pipeline = SpecTcl::getInstance()->GetEventSinkPipeline();
        (*pipeline)(events);
    } else {
        MPIHistogramEvents(events);
    }
}