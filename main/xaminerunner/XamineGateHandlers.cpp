/*
    This software is Copyright by the Board of Trustees of Michigan
    State University (c) Copyright 2017.

    You may use this software under the terms of the GNU public license
    (GPL).  The terms of this license are described at:

     http://www.gnu.org/licenses/gpl.txt

     Authors:
             Ron Fox
             Giordano Cerriza
	     NSCL
	     Michigan State University
	     East Lansing, MI 48824-1321
*/

/** @file:  XamineGateHandlers.cpp
 *  @brief: Implement all the cool stuff defined in XamienGateHanders.h
 *
 */
#include <XamineGateHandlers.h>
#include <client.h>
#include <TCLInterpreter.h>
#include <TCLObject.h>

#include <iostream>
#include <stdlib.h>
#include <stdexcept>

/**
 * Internal data structures:
 */
// The extended event struct:

typedef struct _XamineTclEvent {
    Tcl_Event       s_event;
    msg_XamineEvent s_xamineEventInfo;
} XamineTclEvent, *pXamineTclEvent;

static pXamineTclEvent allocEvent(XamineGateHandler::PollThread::handler h)
{
    pXamineTclEvent pResult = reinterpret_cast<pXamineTclEvent>(Tcl_Alloc(
        sizeof(XamineTclEvent)
    ));
    // Not a lot we can do on an allocation failure:
    
    if (!pResult) {
        std::cerr << "Failed allocation of XamineTclEvent in poll thread\n";
        exit(EXIT_FAILURE);
    }
    pResult->s_event.proc = h;
    pResult->s_event.nextPtr = nullptr;
    
    return pResult;
}

///////////////////////////////////////////////////////////////////////////////
//   PollThread implementation.

/*
 *  The PollThread class encapsulates a thread that polls for events from
 *  Xamine and dispatches them to an event handler in the thread running
 *  the interpreter.  To use this class:
 *  -   Invoke the static start method.  This will return an instance
 *      of this class which will send events to your handler that will execute
 *      in the thread passed in (normally this is just the Tcl_GetCurrentThread
 *      result).
 *  -  When ready to terminate, call the stop instance method.  This will signal
 *     the desire to stop to the running thread and join it so that when
 *     stop() returns, you know the poll thread is fuly exited.
 *  - Delete t he PollThread object that was returned from the start method.
 *    This must only be done on a stopped thread, but the destructor has no
 *    mechanisms to throw exceptions or otherwise indicate an error.
 */

/**
 * PollThread - constructor
 *    @param interp - the id of the thread to signal events to.
 *    @param pHandler - The event handler to execute the events in that thread.
 */
XamineGateHandler::PollThread::PollThread(Tcl_ThreadId interp, handler pHandler) :
    m_interpreterThread(interp),
    m_myHandler(pHandler),
    m_myThread(nullptr),
    m_keepRunning(true)
{
    
}
/**
 * PollThread - destructor
*/
XamineGateHandler::PollThread::~PollThread()
{}

/**
 * operator()
 *    When the thread is finally up and running, this is the object context
 *    thread proc.  We just poll for events from Xamine. If we get one, we
 *    use Tcl_ThreadQueueEvent to communicate it to the handler  thread.
 */
void
XamineGateHandler::PollThread::operator()()
{
    pXamineTclEvent pEvent = allocEvent(m_myHandler);
    
    while (m_keepRunning) {
        int status = Xamine_PollForEvent(1, &(pEvent->s_xamineEventInfo));
        if (status > 0) {
            Tcl_ThreadQueueEvent(
                m_interpreterThread, &(pEvent->s_event), TCL_QUEUE_TAIL
            );
            pEvent = allocEvent(m_myHandler);
            
        } else if (status < 0) {
            std::cerr << "Polling XamineEvent Failed - maybe Xamine exited";
            return;                          // Probably best recovery.
        } 
    }
    // We were told to exit so free pEvent.
    
    Tcl_Free(reinterpret_cast<char*>(pEvent));
}
/**
 * stop
 *    Set our m_keepRunning flag false and join with the thread.
 *    When this method exits the thread is either not longer running or somehow
 *    has detached.
 */
void
XamineGateHandler::PollThread::stop()
{
    if (m_myThread == nullptr) {
        throw std::runtime_error("Asked to stop a non-running PollThread");
    }
    m_keepRunning = false;
    
    int result;
    Tcl_JoinThread(m_myThread, &result);
    m_myThread = nullptr;
}
/**
 * start
 *    Called to start a poll thread:
 *    - The PollThread object is created.
 *    - A Tcl_CreateThread call is made to create the thread with the
 *      threadEntry as the entry point passing the PollThread instance pointer
 *      as its CientData.  It's then up to that method to establish object context
 *      and invoke the operator() method.
 *
 * @param interp - thread to notify when Xamine events arrive.
 * @param handler - The event handler to call for Xamine events.
 * @return PollThread*  - pointer to the created object.  Note that
 *                  there may be time before the thread is actually running.
 */
XamineGateHandler::PollThread*
XamineGateHandler::PollThread::start(Tcl_ThreadId interp, handler pHandler)
{
    PollThread* pResult = new PollThread(interp, pHandler);
    Tcl_CreateThread(
        &(pResult->m_myThread), threadEntry,
        reinterpret_cast<ClientData>(pResult), TCL_THREAD_STACK_DEFAULT,
        TCL_THREAD_JOINABLE
    );
    
    return pResult;
}
/**
 * threadEntry
 *    Called in the context of the poll thread as the entry point of the thread
 *    We establish object context and invoke operator():
 *
 *  @param pClientData -Actually a pointer to the PollThread object to run.
 *  @return Tcl_ThreadCreateType - TCL_THREAD_CREATE_RETURN.
 */
Tcl_ThreadCreateType
XamineGateHandler::PollThread::threadEntry(ClientData pClientData)
{
    PollThread* pObject = reinterpret_cast<PollThread*>(pClientData);
    
    (*pObject)();
    
    TCL_THREAD_CREATE_RETURN;   // As per examplen in TclThread docs.
}
