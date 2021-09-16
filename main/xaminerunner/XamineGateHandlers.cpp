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
            Tcl_ThreadAlert(m_interpreterThread);
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
        } else if (sub == "startPolling") {
            startPolling(interp, objv);    
        } else if (sub == "stopPolling") {
            stopPolling(interp, objv);
        } else if (sub == "isPolling") {
            isPolling(interp, objv);
        } else if (sub == "setHandler") {
            setHandler(interp, objv);
        } else if (sub == "add") {
            add(interp, objv);
        } else if (sub == "remove") {
            remove(interp, objv);
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
/**
 * startPolling
 *    Begin the poll thread.
 *  @note if we detect there's already a poll thread, we toss an error.
 */
void
XamineGateHandler::startPolling(
    CTCLInterpreter& interp, std::vector<CTCLObject>& objv
)
{
    requireExactly(objv, 2, "Xamine::gate startPolling : No addtional command parameters");
    if (m_pPoller) {
        throw std::runtime_error("Xamine::gate startPolling - poll thread already running");
    }
    m_pPoller = PollThread::start(Tcl_GetCurrentThread(), this, EventHandler);
}
/**
 * stopPolling
 *    Ask the polling thread to exit and join on it.
 *   @note throws an exception if the poll thread is not running.
 */
void
XamineGateHandler::stopPolling(
    CTCLInterpreter& interp, std::vector<CTCLObject>& objv
)
{
    requireExactly(objv, 2, "Xamine::gate stopPolling - no additional command parameters");
    if (!m_pPoller) {
        throw std::runtime_error("Xamine::gate stopPolling - poll thread not running");
    }
    m_pPoller->stop();
    m_pPoller = nullptr;
}
/**
 * isPolling
 *  sets the result to true if polling is active.
 */
void
XamineGateHandler::isPolling(
    CTCLInterpreter& interp, std::vector<CTCLObject>& objv
)
{
    requireExactly(objv, 2, "Xamine::gate isPolling - no additional command parameters");
    if (m_pPoller) {
        interp.setResult("1");
    } else {
        interp.setResult("0");
    }
}
/**
 * setHandler
 *    Set a new handler script.  The old handler script, if any is returned as the
 *    value.
 *    @note the handler can be set even if the poll loop is not (yet) active.
 */
void
XamineGateHandler::setHandler(
    CTCLInterpreter& interp, std::vector<CTCLObject>& objv
)
{
    requireExactly(objv, 3, "Xamine::gate setHandler handler-script");
    if (m_pHandlerScript) {
        interp.setResult(*m_pHandlerScript);
    }
    delete m_pHandlerScript;
    m_pHandlerScript = nullptr;
    std::string script = objv[2];
    if (script != "") {
        m_pHandlerScript = new CTCLObject;
        m_pHandlerScript->Bind(interp);
        *m_pHandlerScript = objv[2].getObject();
    }
}
/**
 * add
 *    Add a new gate to Xamine.  The form of this command is:
 *    Xamine::gate add specid gateid gatetype name points
 *
 * Where:
 *  -   specid  - is the *XAMINE* spectrum id.  This is the binding id +1
 *  -   gateid  - is a unique per spectrum gate id.
 *  -   gatetype - is the type of gate: 'cut' 'band' 'contour'
 *  -   name    - Is the name to give to the gate.
 *  -   points  - is a list of {x y} points.  Note that for a cut this must be
 *                2 points and only the X coordinates are important.
 *                The point coordinate system is the spectrum >channel> coordinates
 *                not the world coordinates.
 */
void
XamineGateHandler::add(CTCLInterpreter& interp, std::vector<CTCLObject>& objv)
{
    requireExactly(objv, 7, "Xamine::gate add specid gateid gatetype name points");
    int specid = objv[2];
    int gateid = objv[3];
    std::string gateName = objv[5];
    
    // Figure out the gate type:
    
    std::string gateTypeStr = objv[4];
    Xamine_gatetype gateType;
    if (gateTypeStr == "cut") {
        gateType = cut_1d;
    } else if (gateTypeStr == "band") {
        gateType = band;
    } else if (gateTypeStr == "contour") {
        gateType = contour_2d;
    } else {
        std::string msg = "Invalid gate type string: ";
        msg += gateTypeStr;
        throw std::runtime_error(msg);
    }
    
    // Marshall the points:
    
    Xamine_point points [GROBJ_MAXPTS];
    int nPts = marshallPoints(interp, points, objv[6]);
    
    if (Xamine_EnterGate(
        specid, gateid, gateType, gateName.c_str(), nPts, points
    )) {
        throw CErrnoException("Unable to enter a gate in Xamine");
    }
    
}
/**
 * remove
 *    Remove a gate from Xamine.
 *    Xamine::gate remove specid gateid gatetype
 */
void
XamineGateHandler::remove(CTCLInterpreter& interp, std::vector<CTCLObject>& objv)
{
    requireExactly(objv, 5, "Xamine::gate remove specid gateid gate-type");
    int specid = objv[2];
    int gateid = objv[3];
    std::string strType = objv[4];
    Xamine_gatetype gType;
    if (strType == "cut")  {
        gType = cut_1d;
    } else if (strType == "contour") {
        gType = contour_2d;
    } else if (strType== "band") {
        gType = band;
    } else {
        std::string msg("Invalid gate type string: ");
        msg += strType;
        throw std::runtime_error(msg);
    }
    
    if (Xamine_RemoveGate(specid, gateid, gType)) {
        throw std::runtime_error("Failed to add gate to Xamine");
    }
}
////////////////////////////////////////////////////////////////////////////////
// Private utilities.

/**
 * EventHandler
 *
 *   Called by the event loop when the event loop gets an event queued by the
 *   PollThread.
 * @param pEvent - Actually a pointer to a XamineTclEvent
 * @param flags   - Event loop flags (ignored).
 * @return int - 1 to retire the event and free it.
 * @note this is a static method.
 */
int
XamineGateHandler::EventHandler(Tcl_Event* pEvent, int flags)
{
    pXamineTclEvent pXEvent = reinterpret_cast<pXamineTclEvent>(pEvent);
    XamineGateHandler& gateObject(*(pXEvent->s_pObject));
    msg_XamineEvent& evt(pXEvent->s_xamineEventInfo);
    
    // Only worth doing something if there's a handler script:
    
    if (gateObject.m_pHandlerScript) {
        
        // We only process gates:
        
        if (evt.event == Gate) {
            CTCLInterpreter& interp(*(gateObject.getInterpreter()));
            CTCLObject dict;
            dict.Bind(interp);
            gateObject.eventToDict(interp, dict, evt);
            
            CTCLObject cmd = gateObject.m_pHandlerScript->clone();
            cmd.Bind(interp);
            cmd += dict;
            cmd();
        }
    }
    return 1;
}

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
 *     -   id          - Graphical object id.
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
        Tcl::DictPut(interp, result, "id", static_cast<int>(gate.id));
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
/**
 * marshallPoints
 *   @param interp - interpreter being used to figure out the points.
 *   @param poitns - Array of Xamine_points into which the points will be put.
 *   @param ptsObj - Object containing a list of {x y} points.
 *   @return int - number of points in ptsObj
 *   @note We don't care how many points there are but:
 *       -   Each element of ptsObj must be a two element sublist or we throw.
 *       -   Each element of each sublist must be representable as an integer or
 *           we throw (technically CTCLObject throws).
 */
int
XamineGateHandler::marshallPoints(
    CTCLInterpreter& interp, Xamine_point* points, CTCLObject& ptsObj
)
{
    int npts = ptsObj.llength();
    for (int i = 0; i < npts; i++) {
        CTCLObject point = ptsObj.lindex(i);
        point.Bind(interp);
        if (point.llength() != 2) {
            throw std::runtime_error("Invalid point in marshallPoints");    
        }
        
        int x = point.lindex(0);         // These will throw if
        int y = point.lindex(1);         // the objects can't be represented as ints.
        
        points[i].x = x;
        points[i].y = y;
    }
    
    return npts;
}