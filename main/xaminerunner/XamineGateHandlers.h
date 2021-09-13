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

/** @file:  XamineGateHandlers.h
 *  @brief: Classes that handle Xamine Gates.
 *  
 */
#ifndef XAMINEGATEHANDLERS_H
#define XAMINEGATEHANDLERS_H

#include <TCLObjectProcessor.h>
#include <tcl.h>
#include <clientops.h>

class CTCLInterpreter;
class CTCLObject;

/**
 * @class  XamineGateHandler
 *     This class  provides the the xamine::gate command ensemble which has
 *     subcommands:
 *     -  poll         :  block for some short time to read a gate if one comes in.
 *     -  startpolling : Set up event driven gate handling by starting the poll thread.
 *     -  stoppolling  : Turn off event drinve gat handling.
 *     -  ispolling    : queries the state of event polling.
 *     -  sethandler   : set the gate handler that is called when a gate has been
 *                      gotten from the event driven polling thread.
 *     -  add          : Add a new gate to Xamine.
*/
class XamineGateHandler : public CTCLObjectProcessor
{
private:
    // Define the polling thread class.
    //
public:
    class PollThread {
    public:
        typedef int (*handler)(Tcl_Event* pEvent, int flags);
    private:
        Tcl_ThreadId   m_interpreterThread;
        handler        m_myHandler;
        Tcl_ThreadId   m_myThread;
        bool           m_keepRunning;
        XamineGateHandler* m_pGate;
    public:
        PollThread(
            Tcl_ThreadId interp, XamineGateHandler* pGates, handler pHandler
        );
        ~PollThread();
        void operator()();
        void stop();
        static PollThread* start(
            Tcl_ThreadId interp, XamineGateHandler* pGates, handler pHandler
        );
        static Tcl_ThreadCreateType threadEntry(ClientData pClientData);
    };
private:
    //
    //  Local data and types of the enxlosing class:
    //
    PollThread* m_pPoller;
    CTCLObject* m_pHandlerScript;
public:
    XamineGateHandler(CTCLInterpreter& interp);
    virtual ~XamineGateHandler();
    int operator()(CTCLInterpreter& interp, std::vector<CTCLObject>& objv);
    
    // Subcommand handlers:
private:
    void poll(CTCLInterpreter& interp, std::vector<CTCLObject>& objv);
    void startPolling(CTCLInterpreter& interp, std::vector<CTCLObject>& objv);
    void stopPolling(CTCLInterpreter& interp, std::vector<CTCLObject>& objv);
    void isPolling(CTCLInterpreter& interp, std::vector<CTCLObject>& objv);
    void setHandler(CTCLInterpreter& interp, std::vector<CTCLObject>& objv);
    void add(CTCLInterpreter& interp, std::vector<CTCLObject>& objv);
    
    // Utilities:
private:
    static int EventHandler(Tcl_Event* pEvent, int flags);
    void   eventToDict(
        CTCLInterpreter& interp, CTCLObject& result, msg_XamineEvent& event
    );
    std::string grobjType(grobj_type type);
    void formatPoints(
        CTCLInterpreter& interp, CTCLObject& obj, int npts, msg_point* points
    );
};

#endif
