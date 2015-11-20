/*
    This software is Copyright by the Board of Trustees of Michigan
    State University (c) Copyright 2005.

    You may use this software under the terms of the GNU public license
    (GPL).  The terms of this license are described at:

     http://www.gnu.org/licenses/gpl.txt

     Author:
             Ron Fox
	     NSCL
	     Michigan State University
	     East Lansing, MI 48824-1321
*/
#include <config.h>
#include "TCLLiveEventLoop.h"
#include "TCLInterpreter.h"
#include "TCLVariable.h"
#include <StateException.h>
#include "TCLApplication.h"
#include "CTCLStdioCommander.h"

#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>

using namespace std;

// Externals:

extern CTCLApplication* gpTCLApplication;


/*  The default stop latency.          */

static const long DEFAULT_STOP_LATENCY(100); // in ms.

/*  Pointer to the singleton instance: */

CTCLLiveEventLoop* CTCLLiveEventLoop::m_pTheInstance(0);



/*-----------------------------------------------------------------*/
/*   Construction like operations                                  */
/*-----------------------------------------------------------------*/

/*!
   Return a pointer to the one and only CTCLLiveEventLoop object.
   If this is the first call, the object is created.
*/

CTCLLiveEventLoop*
CTCLLiveEventLoop::getInstance()
{
  if(!m_pTheInstance) {
    m_pTheInstance = new CTCLLiveEventLoop;
  }
  return m_pTheInstance;
}

/*!
   Construction does require some initialization of 
   object variables.
*/
CTCLLiveEventLoop::CTCLLiveEventLoop() :
  m_pStdinTarget(0),
  m_isRunning(false),
  m_stopLatency(DEFAULT_STOP_LATENCY),
  m_pEventCommander(0)
{
}


/*--------------------------------------------------------------*/
/*  Starting and stopping event processing                      */
/*--------------------------------------------------------------*/

/*!
   Start the event loop on the application's Tcl interpreter.
   note that this function will only return to the caller when
   another thread or an event handler invokes the stop member.

   \throw CStateException 
     - There is no application object.
     - The event loop is already running.

*/
void
CTCLLiveEventLoop::start()
{
  // We are really just going to find the CTCLInterpreter that
  // is associated with the application and call the 'other'
  // start function.  It will do all the state checking etc.
  // The only thing we have to worry about is whether or not
  // this is being run from a program that does not create
  // a CTCLApplication.
  //

  if (gpTCLApplication) {
    CTCLInterpreter* pInterp = gpTCLApplication->getInterpreter();
    start(pInterp);
  }
  else {
    throw CStateException("No CTCLApplication", "CTCLApplication",
			  "Attempting to start the live event loop");
  }
		      
}
/*!
   Start the evetn loop on the specified interpreter.
   \param pInterp - Pointer to the CTCLApplication that will receive
                    commands from stdin

   \throw CStateException 
   - Event loop is already active.
*/
void
CTCLLiveEventLoop::start(CTCLInterpreter* pInterp)
{
  if (!m_isRunning) {
    m_pStdinTarget = pInterp;
    m_pEventCommander = new CTCLStdioCommander(pInterp);
    m_pEventCommander->start();

    m_isRunning = true;
    eventLoop();


  }
  else {
    throw CStateException("running", "stopped",
			  "Attempting to start live event loop");
  }

}

/*!
   This stops the event loop.  The event loop waits for events up to
   some time out specified by setStopLatency. Between events and after
   waits timeout, the software will check to see if m_isRunning is still
   true.  If not the event loop function returns.

   Thus there can be a  latency between stopping the event loop and it
   actually stopping.

   Since serveral mutually unaware actors could be attempting to stop the event loop,
   it is not an error to stop an already stopped event loop.

*/

void
CTCLLiveEventLoop::stop()
{
  m_isRunning = false;
}


/*--------------------------------------------------------------------------------*/
/* Event loop timing/stop latency                                                 */
/*--------------------------------------------------------------------------------*/

/*!
   Set the stop latency, event loop timing.  This determines how long the
   event loop will wait for an event before checking to see if it's time to stop
   running.  The latency is expressed in milliseconds. Note, however that it is
   subject to the clock granularity, and that the less latency you request, 
   the higher the CPU utilization of the loop. 

   \param ms  - Maximum number of milliseconds between checks of
                the stop flag.
   \return long
   \retval Prior latency value.

   \note Changes to the latency to an active event loop have the prior latency
         before becoming effective.
   \note It is legal to set the latency of a stopped event loop.
   \note The latency is not reinitialized to the default value when the event loop is
         started, but retains any prior value.
   
*/
long 
CTCLLiveEventLoop::setStopLatency(long ms)
{
  long prior    = m_stopLatency;
  m_stopLatency = ms;
  return prior;
}
/*!
   Return the stop latency.  See setStopLatency for a description of this parameter.
   \return long
   \retval Stop latency value.

   \note  If the stop latency was recently changed, the value returned may not have
          taken effect yet.  Sorry, that's the way it goes.
*/
long
CTCLLiveEventLoop::getStopLatency() const
{
  return m_stopLatency;
}

/*----------------------------------------------------------------------*/
/*  Utilities:                                                          */
/*    Event loop related functions and the event loop                   */
/*----------------------------------------------------------------------*/

/*
** The event loop. This only returns to the caller when the event loop
** is stopped by call or when the event loop has been destroyed
** by some meddler.
*/
void
CTCLLiveEventLoop::eventLoop()
{

  m_pEventCommander->prompt1();
  while (m_isRunning && (!m_pEventCommander->stopped())) {
    Tcl_ReapDetachedProcs();
    struct Tcl_Time timeout;
    timeout.sec = m_stopLatency/1000;
    timeout.usec= (m_stopLatency % 1000) * 1000;

    if (Tcl_WaitForEvent(&timeout) == -1) {
      // Tcl event loop destroyed.

      m_isRunning = false;
      stopEvents();
      return;
    }
    // Process all queued events.

    while (Tcl_DoOneEvent(TCL_ALL_EVENTS | TCL_DONT_WAIT)) 
      ;

  }
  stopEvents();
}

/*
 *  Turns off event processing by calling Tcl_DeleteFileHandler on STDIN_FILENO.
 * This may be a bad thing if the Tk event loop got started (via e.g..
 * package require Tk.
 */
void
CTCLLiveEventLoop::stopEvents()
{
  delete m_pEventCommander;
  m_pEventCommander = 0;
}




