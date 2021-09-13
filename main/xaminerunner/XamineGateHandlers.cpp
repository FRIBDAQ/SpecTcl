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
#include <clientops.h>

#include <TCLInterpreter.h>
#include <TCLObject.h>
#include <Exception.h>
#include <ErrnoException.h>
#include <TclDict.h>

#include <iostream>
#include <stdlib.h>
#include <stdexcept>

/**
 * Internal data structures:
 */
// The extended event struct:

typedef struct _XamineTclEvent {
    Tcl_Event       s_event;
    XamineGateHandler* s_pObject;
    msg_XamineEvent s_xamineEventInfo;
} XamineTclEvent, *pXamineTclEvent;

static pXamineTclEvent allocEvent(
    XamineGateHandler* pObj, XamineGateHandler::PollThread::handler h
)
{
    pXamineTclEvent pResult = reinterpret_cast<pXamineTclEvent>(Tcl_Alloc(
        sizeof(XamineTclEvent)
    ));
    // Not a lot we can do on an allocation failure:
    
    if (!pResult) {
        std::cerr << "Failed allocation of XamineTclEvent in poll thread\n";
        exit(EXIT_FAILURE);
    }
    pResult->s_event.proc     = h;
    pResult->s_event.nextPtr  = nullptr;
    pResult->s_pObject        = pObj;
    
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
 *    @param pGates  - Gate handler object.
 *    @param pHandler - The event handler to execute the events in that thread.
 */
XamineGateHandler::PollThread::PollThread(
    Tcl_ThreadId interp, XamineGateHandler* pGates, handler pHandler
) :
    m_interpreterThread(interp),
    m_myHandler(pHandler),
    m_myThread(nullptr),
    m_keepRunning(true),
    m_pGate(pGates)
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
    pXamineTclEvent pEvent = allocEvent(m_pGate, m_myHandler);
    
    while (m_keepRunning) {
        int status = Xamine_PollForEvent(1, &(pEvent->s_xamineEventInfo));
        if (status > 0) {
            Tcl_ThreadQueueEvent(
                m_interpreterThread, &(pEvent->s_event), TCL_QUEUE_TAIL
            );
            pEvent = allocEvent(m_pGate, m_myHandler);
            
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
 * @param pGates - the gate handler object.
 * @param handler - The event handler to call for Xamine events.
 * @return PollThread*  - pointer to the created object.  Note that
 *                  there may be time before the thread is actually running.
 */
XamineGateHandler::PollThread*
XamineGateHandler::PollThread::start(
    Tcl_ThreadId interp, XamineGateHandler* pGates, handler pHandler
)
{
    PollThread* pResult = new PollThread(interp, pGates, pHandler);
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
/////////////////////////////////////////////////////////////////////////////
// XamineGateHandler implementation.

/**
 * XamineGateHandler - constructor
 *
 * @param interp - INterpreter object.
 */
XamineGateHandler::XamineGateHandler(CTCLInterpreter& interp) :
    CTCLObjectProcessor(interp, "Xamine::gate", kfTRUE),
    m_pPoller(0),
    m_pHandlerScript(0)
{
    
}
/**
 * XamineGateHandler - Destructor.
 */
XamineGateHandler::~XamineGateHandler()
{
     if (m_pPoller) {
        m_pPoller->stop();          // Kill the polling thread if it's running.
     }
     delete m_pHandlerScript;
     delete m_pPoller;
     
}
/**
 * operator()
 *   This method determinest that there are sufficient command parameters and
 *   dispatches to the appropriate command handler method for processing.
 *   Note that we set up exception based error handling so that std::string,
 *   std::exception, and CException derived exceptions are caught and turned
 *   into meaningful errors while others get a bland vanilla message encouraging
 *   the set of explicitly caught exceptions to be broadened.
 *
 * @param interp - interpreter running the command.
 * @praam objv   - The words that make up the command after Tcl has done all
 *                  command line substitutions.
 * @return int - TYCL_OK on success or TCL_ERROR on failure.
 */
int
XamineGateHandler::operator()(
    CTCLInterpreter& interp, std::vector<CTCLObject>& objv
)
{
    bindAll(interp, objv);
    try {
        requireAtLeast(objv, 2, "Xamine::gate needs at least a subcommand");
        std::string sub(objv[1]);
        if (sub == "poll") {
            poll(interp, objv);
        } else {
            std::string msg = "Invalid subcommand : ";
            msg += sub;
            throw msg;
        }
    }
    catch (std::string msg) {
        interp.setResult(msg);
        return TCL_ERROR;
    }
    catch (std::exception& msg) {
        interp.setResult(msg.what());
        return TCL_ERROR;
    }
    catch (CException& e) {
        interp.setResult(e.ReasonText());
        return TCL_ERROR;
    }
    catch (...) {
        interp.setResult("Unexpected exception in Xamine::gate");
        return TCL_ERROR;
    }
    return TCL_OK;
}
///////////////////////////////////////////////////////////////////////////////
// Private command handlers:

/**
 *poll
 *   Poll for a gate.  The time to block waiting for a gate is
 *   an additional paramteer.  It must be an integer number of seconds.
 *   The result will either be a dict that describes the gate that was received,
 *   or empty if either no event was received or an event was received but it was
 *   not a gate.
 */
void
XamineGateHandler::poll(CTCLInterpreter& interp, std::vector<CTCLObject>& objv)
{
    requireExactly(objv, 3, "Usage: Xamine::gate poll nseconds");
    int seconds = objv[2];
    msg_XamineEvent event;
    
    int status = Xamine_PollForEvent(seconds, &event);
    if (status > 0) {
        CTCLObject dict;
        dict.Bind(interp);
        eventToDict(interp, dict, event);
        interp.setResult(dict);  
    }  else if (status < 0) {
        throw CErrnoException("Failed to read Xamine event on poll");
    }
    // If here the poll timed out...and we're all set.
}

////////////////////////////////////////////////////////////////////////////////
// Private utilities.

/**
 * eventToDict
 *   In this implementation, we only support gate objects.  For all others,
 *   We returnb an empty dictionary.  If asked we may later support
 *   the button box.
 * @param interp - interpreter being used to build the dict.
 * @param result - object into which the dict will be built.
 * @param event  - the event struct that Xamine poduced.
 * @note - result is assumed to already be bound to interp.
 *
 * We build a dict with the keys:
 *     -   spectrum    - Xamine spectrum binding id.
 *     -   type        - Type of graphical object
 *     -   name        - object name "" if no name.
 *     -   points      - list of {x y} point pairs.
 */
void
XamineGateHandler::eventToDict(
    CTCLInterpreter& interp, CTCLObject& result, msg_XamineEvent& event
)
{
    if (event.event == Gate) {
        msg_object& gate(event.data.gate);
        Tcl::DictPut(interp, result, "spectrum", static_cast<int>(gate.spectrum));
        Tcl::DictPut(
            interp, result, "type", grobjType(gate.type).c_str()
        );
        if (gate.hasname) {
            Tcl::DictPut(interp, result, "name", gate.name);
        } else {
            Tcl::DictPut(interp, result, "name", "");
        }
        CTCLObject points;
        points.Bind(interp);
        formatPoints(interp, points, gate.npts, gate.points);
        Tcl::DictPut(interp, result, "points", points);
        
    } else {
        return;                          // Not a gate.
    }
}
/**
 * grobjType
 *    Turn a graphical object type into a string.
 * @param type -graphical object type.
 * @return std::string - identifying the type.
 */
std::string
XamineGateHandler::grobjType(grobj_type type)
{
    if (type == generic) {
        return "generic";
    } else if (type == cut_1d) {
        return "cut";
    } else if (type == summing_region_1d) {
        return "summingRegion1d";
    } else if (type == marker_1d) {
        return "marker1d";
    } else if (type == contour_2d) {
        return "contour";
    } else if (type == band) {
        return "band";
    } else if (type == summing_region_2d) {
        return "summingRegion2d";
    } else if (type == marker_2d) {
        return"marker2d";
    } else if (type == peak1d) {
        return "peak1d";
    } else if (type == fitline) {
        return "fit";
    } else if (type == pointlist_1d) {
        return"points1d";
    } else if (type == pointlist_2d) {
        return "points2d";
    } else {
        return "Unknown";
    }
}
/**
 * formatPoints
 *    Format points into a list of x/y pairs
 * @param interp - interpreter to use in formatting
 * @param obj    - :Resulting obj.
 * @param npts  - Number of points to format.
 * @param points - The points themselves.
 */
void
XamineGateHandler::formatPoints(
    CTCLInterpreter& interp, CTCLObject& obj, int npts, msg_point* points
)
{
    for (int i = 0; i < npts; i++) {
        CTCLObject item;
        CTCLObject x;
        CTCLObject y;
        item.Bind(interp);
        x.Bind(interp);
        y.Bind(interp);
        
        x = points[i].x;
        y = points[i].y;
        item += x;
        item += y;
        obj += item;
    }
}